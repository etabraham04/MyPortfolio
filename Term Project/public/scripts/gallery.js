// load buttons with like or dislike
function load(id) {
    let xhttp = new XMLHttpRequest();

    xhttp.open('GET', `/artworks/like/${id}`);

    xhttp.setRequestHeader('Content-Type', 'application/json');

    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            if (this.responseText == 'liked') {
                document.getElementById(`like_${id}`).innerHTML = 'Dislike';
            } else if (this.responseText == 'disliked') {
                document.getElementById(`like_${id}`).innerHTML = 'Like';
            }
        }
    };

    xhttp.send();
}

// like the artwork
function like(id) {
    let xhttp = new XMLHttpRequest();

    xhttp.open('POST', '/artworks/like');

    xhttp.setRequestHeader('Content-Type', 'application/json');

    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            if (this.responseText == 'not logged in') {
                console.log('not logged in');
                window.location.href = '/account/login';
            } else {
                window.location.reload();
            }
        }
    };

    xhttp.send(JSON.stringify({ id: id }));
}

function reviews() {}
