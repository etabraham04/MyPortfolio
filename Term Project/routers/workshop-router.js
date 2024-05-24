const mongoose = require("mongoose");

const express = require("express");
const Artwork = require("../schemas/Art-Model");
const User = require("../schemas/User-Model");
const Workshop = require("../schemas/Workshop-Model");

let router = express.Router();

// create workshop in database
router.post("/create", async function (req, res, next) {
	let name = req.body.name;

	if (!name) {
		res.status(400).send("Workshop name is required");
		return;
	}

	let workshop = new Workshop({
		title: name,
		owner: req.session.user.accountID,
		participants: [],
	});

	await User.findOne({ _id: req.session.user.accountID }).then((user) => {
		let notification = {
			type: "new-workshop",
			message: `${user.fullname} has uploaded a new workshop!`,
			accountID: req.session.user.accountID.toString(),
			accountName: req.session.user.username,
		};

		// add notifications to followers
		user.followers.forEach((followerid) => {
			User.findOne({ _id: followerid.id }).then((follower) => {
				follower.notifications.push(notification);
				follower.save();
			});
		});
	});

	await workshop.save();

	res.status(200).send("Workshop created");
});

// remove workshop from database
router.put("/remove", async function (req, res, next) {
	let id = req.body.id;

	Workshop.deleteOne({
		_id: id,
	})
		.then(() => {
			res.status(200).send("Workshop removed");
		})
		.catch((err) => {
			console.log(err);
		});
});

// add session user to workshop
router.put("/join", async function (req, res, next) {
	let id = req.body.id;

	Workshop.findOne({
		_id: id,
	}).then((workshop) => {
		workshop.participants.push(req.session.user.accountID);
		workshop.save();
	});

	res.status(200).send("Joined workshop");
});

// remove session user from workshop
router.put("/leave", async function (req, res, next) {
	let id = req.body.id;

	Workshop.findOne({
		_id: id,
	}).then((workshop) => {
		workshop.participants.pull(req.session.user.accountID);
		workshop.save();
	});

	res.status(200).send("Left workshop");
});

// export the router
module.exports = router;
