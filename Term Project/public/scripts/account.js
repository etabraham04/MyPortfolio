// switch account from patron to artist and vice versa
function switchAccount() {
    let xhttp = new XMLHttpRequest();

    xhttp.open('GET', '/account/switch');

    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            let response = JSON.parse(this.response);

            // if patron has no artworks
            if (response.new) {
                window.location.href = '/artworks/new';
            }
            // patron
            else if (response.role == 'patron') {
                window.location.reload();
            }
            // artists
            else {
                window.location.reload();
            }
        }
    };

    xhttp.send();
}

// redirect user to create artwork page
function addArtwork() {
    window.location.href = '/artwork/create';
}

// add workshop to database
function addWorkshop() {
    let workshopName = document.getElementById('workshopName').value.trim();

    if (!workshopName) {
        alert('Workshop name is required');
        return;
    }

    let xhttp = new XMLHttpRequest();

    xhttp.open('POST', '/workshop/create');

    xhttp.setRequestHeader('Content-Type', 'application/json');

    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            // reload page to show new workshop
            window.location.reload();
        }
    };

    xhttp.send(JSON.stringify({ name: workshopName }));
}

// remove workshop from database
function removeWorkshop(id) {
    let xhttp = new XMLHttpRequest();

    xhttp.open('PUT', '/workshop/remove');

    xhttp.setRequestHeader('Content-Type', 'application/json');

    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            window.location.reload();
        }
    };

    xhttp.send(JSON.stringify({ id: id }));
}

// join workshop
function joinWorkshop(id) {
    let xhttp = new XMLHttpRequest();

    xhttp.open('PUT', '/workshop/join');

    xhttp.setRequestHeader('Content-Type', 'application/json');

    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            alert('Joined workshop');
            window.location.reload();
        }
    };

    xhttp.send(JSON.stringify({ id: id }));
}

// leave workshop
function leaveWorkshop(id) {
    let xhttp = new XMLHttpRequest();

    xhttp.open('PUT', '/workshop/leave');

    xhttp.setRequestHeader('Content-Type', 'application/json');

    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            window.location.reload();
        }
    };

    xhttp.send(JSON.stringify({ id: id }));
}

// follow user
function follow(user_id) {
    let xhttp = new XMLHttpRequest();

    xhttp.open('POST', '/account/follow');

    xhttp.setRequestHeader('Content-Type', 'application/json');

    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            window.location.reload();
        }
    };

    xhttp.send(JSON.stringify({ id: user_id }));
}

// like artwork
function like(id) {
    let xhttp = new XMLHttpRequest();

    xhttp.open('POST', '/artworks/like');

    xhttp.setRequestHeader('Content-Type', 'application/json');

    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            window.location.reload();
        }
    };

    xhttp.send(JSON.stringify({ id: id }));
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
