const mongoose = require('mongoose');
const express = require('express');
const Artwork = require('../schemas/Art-Model');
const User = require('../schemas/User-Model');
const Workshop = require('../schemas/Workshop-Model');

let router = express.Router();

// send the session data to the client
router.get('/', async function (req, res, next) {
    res.status(200).json({
        loggedIn: req.session.user.loggedIn,
        accountID: req.session.user.accountID,
        role: req.session.user.role,
    });
});

// user login page
router.get('/login', async function (req, res, next) {
    res.render('pages/login');
});

// authentincate user login
router.post('/login', async function (req, res, next) {
    // search for user in database
    User.findOne({
        username: req.body.username,
        password: req.body.password,
    }).then(async (result) => {
        if (result) {
            // check if userid is already in session data cookie
            let { db } = req.app.locals.db;

            let sessions = await db.collection('sessiondata').find({});

            // if user is already logged in, delete the old session data
            sessions.forEach((doc) => {
                console.log(doc.session.user.accountID);
                if (doc.session.user.accountID) {
                    if (
                        doc.session.user.accountID.toString() ==
                        result._id.toString()
                    ) {
                        db.collection('sessiondata').deleteOne({
                            _id: doc._id,
                        });
                        console.log('deleted session data');
                    }
                }
            });

            // create new session data
            req.session.user.loggedIn = true;
            req.session.user.accountID = result._id;
            req.session.user.username = result.username;
            req.session.user.role = result.role;
        }

        // send default session values to client if not able to log in
        res.status(200).json({
            loggedIn: req.session.user.loggedIn,
            accountID: req.session.user.accountID,
            role: req.session.user.role,
        });
    });
});

// user logout
router.get('/logout', async function (req, res, next) {
    // destroy session
    req.session.destroy();

    res.status(200).send();
});

// user registration page
router.get('/register', async function (req, res, next) {
    res.render('pages/register');
});

// create user from register data, send errors if any
router.post('/register', async function (req, res, next) {
    let user = await User.findOne({ username: req.body.username });

    // if username already in use
    if (user) {
        res.status(200).json({
            loggedIn: req.session.user.loggedIn,
            accountID: req.session.user.accountID,
            role: req.session.user.role,
            error: { errors: [{ message: 'Username already exists' }] },
        });
        return;
    }

    // make new user
    user = new User({
        fullname: req.body.fullname,
        username: req.body.username,
        password: req.body.password,
        role: 'patron',
    });

    // save user to database
    user.save()
        .then((result) => {
            // add user to session data
            req.session.user.loggedIn = true;
            req.session.user.accountID = result._id;
            req.session.user.username = result.username;
            req.session.user.role = result.role;

            // send session data to client
            res.status(200).json({
                loggedIn: req.session.user.loggedIn,
                accountID: req.session.user.accountID,
                role: req.session.user.role,
                error: null,
            });
        })
        .catch((err) => {
            // send error data to client
            res.status(200).json({
                loggedIn: req.session.user.loggedIn,
                accountID: req.session.user.accountID,
                role: req.session.user.role,
                error: err,
            });
        });
});

// EVERYTHING BELOW THIS POINT REQUIRES USER TO BE LOGGED IN
// prevent usage unless signed in
router.use((req, res, next) => {
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

// switch account
router.get('/switch', function (req, res, next) {
    // if user is a patron, switch to artist
    
    if (req.session.user.role == 'patron') {
        User.findOne({ _id: req.session.user.accountID }).then((user) => {
            // if user has no artworks, send new flag to force new artwork creation
            if (user.artworks.length == 0) {
                res.status(200).json({
                    new: true,
                });
                return;
            }

            req.session.user.role = "artist";
            user.role = 'artist';
            user.save();

            res.status(200).json({
                role: req.session.user.role,
            });
        });
    }
    // if user is an artist, switch to patron
    else {
        req.session.user.role = 'patron';
        User.findOne({ _id: req.session.user.accountID }).then((user) => {
            // change user role to patron
            user.role = 'patron';
            user.save();
            res.status(200).json({
                role: req.session.user.role,
            });
        });
    }
});

// follow an artist
router.post('/follow', async function (req, res, next) {
    let artist_id = req.body.id;
    let username;

    // add to artist's account
    User.findOne({ _id: artist_id }).then((user) => {
        // store username for later
        username = user.username;

        // build data object
        let data = {
            id: req.session.user.accountID.toString(),
            username: req.session.user.username,
        };

        // check if user is already following
        let following = false;
        let doc;

        user.followers.forEach((follower) => {
            if (follower.id === data.id) {
                following = true;
                doc = follower;
            }
        });

        // if already following, unfollow
        if (following) {
            user.followers.pull(doc);
            user.save();
        }
        // else follow
        else {
            user.followers.push(data);
            user.save();
        }
    });

    // add to user's account
    User.findOne({ _id: req.session.user.accountID }).then((user) => {
        // build data object
        let data = {
            id: artist_id,
            username: username,
        };

        // check if user is already following
        let following = false;
        let doc;

        user.following.forEach((follower) => {
            if (follower.id === data.id) {
                following = true;
                doc = follower;
            }
        });

        // if already following, unfollow
        if (following) {
            user.following.pull(doc);
            user.save();
        }
        // else follow
        else {
            user.following.push(data);
            user.save();
        }
    });

    res.status(200).send();
});

// get the notifications
router.get('/notifications', async function (req, res, next) {
    let user_id = req.session.user.accountID;

    // send notifications to client
    User.findOne({ _id: user_id }).then((user) => {
        res.status(200).json({
            notifications: user.notifications,
        });
    });
});

// delete the notifications
router.put('/notifications', async function (req, res, next) {
    let user_id = req.session.user.accountID;

    User.findOne({ _id: user_id }).then(async (user) => {
        user.notifications = [];
        await user.save();
        res.status(200).send();
    });
});

// go to artist's page from artwork
router.get('/art/:id', async function (req, res, next) {
    // check if id is valid
    if (mongoose.Types.ObjectId.isValid(req.params.id) === false) {
        res.render('pages/error', {
            error: `Artist ID (${req.params.id}) was invalid`,
        });
        return;
    }

    let artwork_id = req.params.id;

    Artwork.findOne({ _id: artwork_id }).then((artwork) => {
        if (!artwork) {
            res.render('pages/error', { error: 'Artwork ID was not found' });
            return;
        }
        User.findOne({ artworks: artwork_id })
            .then((user) => {
                if (!user) {
                    res.render('pages/error', {
                        error: 'Artist ID was not found',
                    });
                    return;
                }

                // redirect to artist's page
                let artist_id = user._id;
                res.redirect(`/account/${artist_id}`);
            })
            .catch((err) => {
                console.log(err);
            });
    });
});

// go to artist page
router.get('/:id', async function (req, res, next) {
    // check if id is valid
    if (mongoose.Types.ObjectId.isValid(req.params.id) === false) {
        res.render('pages/error', {
            error: `Artist ID (${req.params.id}) was invalid`,
        });
        return;
    }

    // search for user with id
    User.findOne({ _id: req.params.id })
        .then((user) => {
            if (!user) {
                res.render('pages/error', {
                    error: `Artist ID (${req.params.id}) was not found`,
                });
                return;
            }
            // search for artworks that belong to user
            Artwork.find({ _id: { $in: user.artworks } })
                .then(async (artworks) => {
                    let following = false;

                    // check if session user is following this artist
                    user.followers.forEach((follower) => {
                        if (follower.id == req.session.user.accountID) {
                            following = true;
                        }
                    });

                    // get liked artworks
                    let likedArtworkIds = [];
                    let likedArtwork = [];

                    user.likes.forEach((like) => {
                        likedArtworkIds.push(like.toString());
                    });

                    await Artwork.find({ _id: { $in: likedArtworkIds } }).then(
                        (artworks) => {
                            likedArtwork = artworks;
                        }
                    );

                    // get workshops by user and enrolled workshops user is in
                    let workshops = [];
                    let enrolledWorkshops = [];

                    await Workshop.find({ owner: user._id }).then((results) => {
                        workshops = results;
                    });

                    await Workshop.find({}).then((everyWorkshop) => {
                        everyWorkshop.forEach((workshop) => {
                            if (
                                !(
                                    workshop.owner ==
                                    req.session.user.accountID.toString()
                                )
                            ) {
                                if (
                                    workshop.participants.includes(
                                        req.session.user.accountID.toString()
                                    )
                                ) {
                                    enrolledWorkshops.push(workshop);
                                }
                            }
                        });
                    });

                    if (req.session.user.loggedIn) {
                        await User.findOne({
                            _id: req.session.user.accountID.toString(),
                        }).then((user) => {
                            myArtworks = user.artworks;
                            myLikes = user.likes;
                        });
                    }

                    let response = {
                        user: user,
                        artworks: artworks,
                        loggedIn: req.session.user.loggedIn,
                        myAccountID: req.session.user.accountID,
                        myAccount: req.session.user.accountID == req.params.id,
                        myArtworks: myArtworks,
                        myLikes: myLikes,
                        role: req.session.user.role,
                        following: following,
                        likedArtwork: likedArtwork,
                        workshops: workshops,
                        enrolledWorkshops: enrolledWorkshops,
                    };
                    res.render('pages/account', response);
                })
                .catch((err) => {
                    console.log(err);
                });
        })
        .catch((err) => {
            console.log(err);
        });
});

// export the router
module.exports = router;
