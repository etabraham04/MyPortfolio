const mongoose = require('mongoose');
const { ObjectId } = require('mongodb');
const Schema = mongoose.Schema;

let artworkSchema = Schema({
    Title: {
        type: String,
        required: true,
        minlength: 1,
        unique: true,
    },
    Artist: {
        type: String,
        required: true,
        minlength: 1,
    },
    Year: {
        type: Number,
        required: true,
        min: [0, 'Year must be greater than 0'],
        max: [2023, "Year can't be in the future"],
    },
    Category: {
        type: String,
        required: true,
        minlength: 1,
    },
    Medium: {
        type: String,
        required: true,
        minlength: 1,
    },
    Description: {
        type: String,
    },
    Poster: {
        type: String,
        required: true,
        minlength: 1,
    },
    Reviews: {
        type: [Object],
        required: false,
    },
    Likes: {
        type: [ObjectId],
        required: false,
    },
});

module.exports = mongoose.model('artworks', artworkSchema);
