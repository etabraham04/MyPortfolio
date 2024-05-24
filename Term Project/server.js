const express = require('express');
const app = express();
const port = 3000;

// mongoose setup
const mongoose = require('mongoose');
const Artwork = require('./schemas/Art-Model');
const User = require('./schemas/User-Model');
const Workshop = require('./schemas/Workshop-Model');

// connect to database
mongoose.connect('mongodb://localhost/openGallery');
app.locals.db = mongoose.connection;

const { db } = app.locals;

db.on('error', console.error.bind(console, 'connection error:'));
db.once('open', function () {
    console.log('Connected to openGallery database.');

    run();
});

function run() {
    // store session on mongodb
    const session = require('express-session');
    const MongoDBStore = require('connect-mongodb-session')(session);
    const store = new MongoDBStore({
        uri: 'mongodb://127.0.0.1:27017/openGallery',
        collection: 'sessiondata',
    });

    // setup session
    app.use(
        session({
            secret: 'art is cool',
            resave: true,
            saveUninitialized: true,
            store: store,
            cookie: {
                maxAge: 1000 * 60 * 30, //  30  minutes
            },
        })
    );
    // setup default user session values
    app.use((req, res, next) => {
        if (!req.session.user) {
            req.session.user = {
                loggedIn: false,
                accountID: null,
                username: null,
                role: 'patron',
            };
        }
        next();
    });

    // set up other middleware
    app.use(express.static('public'));
    app.use(express.json());
    app.use(express.urlencoded({ extended: true }));

    app.set('view engine', 'pug');

    // ROUTERS
    // load home page
    app.get(['/', '/home'], async function (req, res, next) {
        let artworks = [];
        let myArtworks = [];
        let myLikes = [];

        await Artwork.find({}).then((docs) => {
            artworks = docs;
        });

        if (req.session.user.loggedIn) {
            await User.findOne({ _id: req.session.user.accountID.toString() })
            .then((user) => {
                myArtworks = user.artworks;
                myLikes = user.likes;
            });
        }

        res.render('pages/home', {
            artworks: artworks,
            loggedIn: req.session.user.loggedIn,
            myArtworks: myArtworks,
            myLikes: myLikes,
        });
    });

    // account router
    let accountRouter = require('./routers/account-router');
    app.use('/account', accountRouter);

    // prevent usage unless signed in
    app.use((req, res, next) => {
        if (!req.session.user.loggedIn) {
            if (req.method == 'GET') {
                res.redirect('/account/login');
                return;
            } else if (req.method == 'POST') {
                res.status(200).send('not logged in');
                return;
            }
        }
        next();
    });

    // artwork router
    let artworkRouter = require('./routers/artwork-router');
    app.use('/artworks', artworkRouter);
    // search router
    let searchRouter = require('./routers/search-router');
    app.use('/search', searchRouter);
    // workshop router
    let workshopRouter = require('./routers/workshop-router');
    app.use('/workshop', workshopRouter);

    // listen on port
    app.listen(port);

    console.log(`Server is listening at http://localhost:${port}`);
}
