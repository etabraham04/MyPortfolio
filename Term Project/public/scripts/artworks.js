// like artwork
function like(id) {
    let xhttp = new XMLHttpRequest();

    xhttp.open('POST', '/artworks/like');

    xhttp.setRequestHeader('Content-Type', 'application/json');

    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            // make sure user is logged in
            if (this.responseText == 'not logged in') {
                window.location.href = '/account/login';
            } else if (this.responseText == 'liked') {
                document.getElementById(`like_${id}`).innerHTML = 'Dislike';
            } else if (this.responseText == 'disliked') {
                document.getElementById(`like_${id}`).innerHTML = 'Like';
            }
        }
    };

    xhttp.send(JSON.stringify({ id: id }));
}

function reviews() {}
