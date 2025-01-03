const mongoose = require('mongoose');

const express = require('express');
const Artwork = require('../schemas/Art-Model');
const User = require('../schemas/User-Model');
const Workshop = require('../schemas/Workshop-Model');

let router = express.Router();

// load search page with query
router.get('/', async function (req, res, next) {
    // get user query
    let user_query = req.query.query;
    let page = req.query.page;

    // if page is not a less that 1, set to 1
    if(Number(page) <= 0){
        page = '1';
    }

    // get total results to calculate max pages and total results
    let results = await Artwork.find({
        $or: [
            { Title: { $regex: user_query, $options: 'i' } },
            { Artist: { $regex: user_query, $options: 'i' } },
            { Category: { $regex: user_query, $options: 'i' } },
        ],
    });

    let totalResults = results.length;
    maxPages = Math.ceil(results.length / 10);

    // if no results, set max pages to 1
    if(maxPages == 0){
        maxPages = 1;
    }

    // if page is greater than max pages, set to max pages to prevent user leaving search results
    if(Number(page) > maxPages){
        page = maxPages;
    }

    // search for all instances of user query and limit to 10 results per page
    results = await Artwork.find({
        $or: [
            { Title: { $regex: user_query, $options: 'i' } },
            { Artist: { $regex: user_query, $options: 'i' } },
            { Category: { $regex: user_query, $options: 'i' } },
        ],
    })
        .limit(10)
        .skip((page - 1) * 10);

    // render search page with results
    res.render('pages/search', {
        artworks: results,
        loggedIn: req.session.user.loggedIn,
        search: user_query,
        page: Number(page),
        maxPages: maxPages,
        totalResults: totalResults,
    });
});


// same as above but with category filter
router.get('/category', async function (req, res, next) {
    // get user query
    let user_query = req.query.query;
    let page = req.query.page;

    let results = await Artwork.find({
        $or: [{ Category: { $regex: user_query, $options: 'i' } }],
    });

    let totalResults = results.length;
    maxPages = Math.ceil(results.length / 10);

    // search for all instances of user query
    results = await Artwork.find({
        $or: [{ Category: { $regex: user_query, $options: 'i' } }],
    })
        .limit(10)
        .skip((page - 1) * 10);

    res.render('pages/search', {
        artworks: results,
        loggedIn: req.session.user.loggedIn,
        search: user_query,
        page: Number(page),
        maxPages: maxPages,
        totalResults: totalResults,
    });
});

// export the router
module.exports = router;
