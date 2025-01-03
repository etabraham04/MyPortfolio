const mongoose = require('mongoose');
const Artwork = require('../schemas/Art-Model');
const User = require('../schemas/User-Model');

// save artworks
let artworks = require('./gallery.json');
let artistList = [];

// connect to database
mongoose.connect('mongodb://localhost/openGallery');

const db = mongoose.connection;
db.on('error', console.error.bind(console, 'connection error:'));
db.once('open', async function () {
    console.log('Connected to openGallery database.');

    // drop collections
    await mongoose.connection.dropDatabase();
    console.log('Database dropped');

    // create artworks in collection
    await Artwork.create(artworks)
        .then((results) => {
            console.log(
                `${results.length} art works have been added to the collection`
            );
            createUsers(results);
        })
        .catch((err) => console.log(err));

    // create users in collection
    await User.create(artistList)
        .then((results) => {
            console.log(
                `${results.length} users have been added to the collection`
            );
        })
        .catch((err) => console.log(err));

    // close connection
    db.close();
});

// create an array of artists from artwork data
function createUsers(results) {
    let artists = {};

    // create users from results
    for (let i = 0; i < results.length; i++) {
        let username = results[i].Artist.replace(/\s/g, '_').toLowerCase();
        let fullname = results[i].Artist;

        if (!(username in artists)) {
            artists[username] = { fullname: fullname, artworks: [] };
        }
        artists[username].artworks.push(results[i]._id);
    }

    Object.keys(artists).forEach((artist) => {
        artistList.push({
            username: artist,
            fullname: artists[artist].fullname,
            password: artists[artist].password,
            artworks: artists[artist].artworks,
            role: 'artist',
        });
    });

    artistList.push({ fullname: 'Admin', username: 'admin', password: 'admin123', artworks: [] });
}
