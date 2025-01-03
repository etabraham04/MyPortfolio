// add review to artwork
function addReview(id) {
    let review = document.getElementById('newReview').value;

    let xhttp = new XMLHttpRequest();

    xhttp.open('POST', `/artworks/review/${id}`);

    xhttp.setRequestHeader('Content-Type', 'application/json');

    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            window.location.reload();
        }
    };

    xhttp.send(JSON.stringify({ review: review }));
}

// remove review from artwork
function removeReview(id) {
    let xhttp = new XMLHttpRequest();

    xhttp.open('POST', `/artworks/review/${id}`);

    xhttp.setRequestHeader('Content-Type', 'application/json');

    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            window.location.reload();
        }
    };

    xhttp.send(JSON.stringify({ review: '' }));
}
