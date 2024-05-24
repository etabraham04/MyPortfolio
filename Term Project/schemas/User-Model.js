const { ObjectId } = require('mongodb');
const mongoose = require('mongoose');
const Schema = mongoose.Schema;

let userSchema = Schema({
    fullname:{
        type: String,
        required: true,
        minlength: 1,
        trim: true,
    },
    username: {
        type: String,
        required: true,
        minlength: 1,
        trim: true,
        unique: true,
    },
    password: {
        type: String,
        required: true,
        minlength: [8, 'Password must be at least 8 characters long'],
        default: 'password',
    },
    role: {
        type: String,
        required: true,
        enum: ['artist', 'patron'],
        default: 'patron',
    },
    artworks: {
        type: [ObjectId],
        required: false,
    },
    workshops: {
        type: [Object],
        required: false,
    },
    followers: {
        type: [Object],
        required: false,
    },
    following: {
        type: [Object],
        required: false,
    },
    notifications: {
        type: [Object],
        required: false,
    },
    reviews: {
        type: [Object],
        required: false,
    },
    likes: {
        type: [ObjectId],
        required: false,
    },
});

module.exports = mongoose.model('users', userSchema);
