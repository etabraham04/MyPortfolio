const mongoose = require("mongoose");
const { ObjectId } = require("mongodb");

const express = require("express");
const Artwork = require("../schemas/Art-Model");
const User = require("../schemas/User-Model");
const Workshop = require("../schemas/Workshop-Model");

let router = express.Router();

// checks if artwork is liked by user
router.get("/like/:id", async function (req, res, next) {
	let artworkId = req.params.id;
	let userId = req.session.user.accountID.toString();

	Artwork.findOne({ _id: artworkId }).then((artwork) => {
		if (!artwork) {
			res.render("pages/error", { error: "Artwork ID not found" });
		}
		if (!artwork.Likes.includes(userId)) {
			res.send("disliked");
		}
		// else add like
		else {
			res.send("liked");
		}
	});
});

// like artwork
router.post("/like", async function (req, res, next) {
	let artworkId = req.body.id;
	let userId = req.session.user.accountID.toString();

	Artwork.findOne({ _id: artworkId }).then(async (artwork) => {
		User.findOne({ _id: userId }).then(async (user) => {
			if (artwork.Likes.includes(userId)) {
				artwork.Likes.pull(userId);
				await artwork.save();

				user.likes.pull(artworkId);
				await user.save();

				res.send("disliked");
			}
			// else add like
			else {
				artwork.Likes.push(userId);
				await artwork.save();

				user.likes.push(artworkId);
				await user.save();

				res.send("liked");
			}
		});
	});
});

// post a review on an artwork
router.post("/review/:id", async function (req, res, next) {
	let artworkId = req.params.id;
	let userId = req.session.user.accountID.toString();

	let review = req.body.review;

	let review_data = {
		userid: userId.toString(),
		artworkid: artworkId.toString(),
		review: review,
		username: req.session.user.username,
		id: new mongoose.Types.ObjectId(),
	};

	Artwork.findOne({ _id: artworkId }).then(async (artwork) => {
		User.findOne({ _id: userId }).then(async (user) => {
			let reviewed = false;

			review_data.Title = artwork.Title;

			artwork.Reviews.forEach(async (review) => {
				if (review.userid === userId) {
					reviewed = true;
					doc = review;
				}
			});

			if (reviewed) {
				artwork.Reviews.pull(doc);
				user.reviews.pull(doc);
			} else {
				artwork.Reviews.push(review_data);
				user.reviews.push(review_data);
			}

			artwork.save();
			user.save();

			res.status(200).send("reviewed");
		});
	});
});

// load page to create new artwork
router.get("/new", function (req, res, next) {
	res.render("pages/new-art", {
		loggedIn: req.session.user.loggedIn,
	});
});

// add new artwork to database
router.post("/new", async function (req, res, next) {
	let fullname;

	await User.findOne({ _id: req.session.user.accountID }).then((user) => {
		// get session user fullname
		fullname = user.fullname;

		// create new artwork
		let art = new Artwork({
			Title: req.body.title,
			Artist: fullname,
			Year: req.body.year,
			Category: req.body.category,
			Medium: req.body.medium,
			Description: req.body.description,
			Poster: req.body.poster,
		});

		if (
			!art.Title ||
			!art.Artist ||
			!art.Year ||
			!art.Category ||
			!art.Medium ||
			!art.Description ||
			!art.Poster
		) {
			res.status(200).json({ message: "error", id: null });
			return;
		}

		req.session.user.role = "artist";
		user.role = "artist";
		user.save();

		let notification = {
			type: "new-art",
			message: `${fullname} has uploaded a new artwork: ${req.body.title}`,
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

		// save artwork and then add to user's artworks list
		art
			.save()
			.then((result) => {
				user.artworks.push(result._id);
				user.save();
				res.status(200).json({ message: "success", id: result._id });
				return;
			})
			.catch((err) => {
				res.status(200).json({ message: "error", id: result._id });
				return;
			});
	});
});

// load artwork page from param id
router.get("/:id", async function (req, res, next) {
	if (mongoose.Types.ObjectId.isValid(req.params.id) === false) {
		res.render("pages/error", {
			error: `Artwork ID (${req.params.id}) was invalid`,
		});
		return;
	}

	let art = await Artwork.findOne({ _id: req.params.id });

	if (!art) {
		res.render("pages/error", {
			error: `Artwork ID (${req.params.id}) was not found`,
		});
		return;
	}

    let user = await User.findOne({ _id: req.session.user.accountID });

    let myArtpieces = user.artworks

    let myArtpiece = myArtpieces.includes(req.params.id);

	let userId = req.session.user.accountID.toString();
	let reviewed = false;

	art.Reviews.forEach(async (review) => {
		if (review.userid === userId) {
			reviewed = true;
		}
	});
	res.render("pages/gallery", {
		art: art,
		loggedIn: req.session.user.loggedIn,
		myArtpiece: myArtpiece,
		reviewed: reviewed,
		userId: userId,
	});
});

// export the router
module.exports = router;
