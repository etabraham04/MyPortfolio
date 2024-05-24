const { ObjectId } = require('mongodb');
const mongoose = require('mongoose');
const Schema = mongoose.Schema;

let workshopSchema = Schema({
    title: {
        type: String,
        required: true,
        minlength: 1,
        trim: true,
    },
    owner: {
        type: Schema.Types.ObjectId,
        ref: 'User',
        required: true,
    },
    participants: [{
        type: Schema.Types.ObjectId,
        ref: 'User',
        required: false,
    }],
});

module.exports = mongoose.model('workshops', workshopSchema);
