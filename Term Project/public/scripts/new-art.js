// submit new artwork
function submit() {
    let title = document.getElementById('title').value;
    let year = document.getElementById('year').value;
    let category = document.getElementById('category').value;
    let medium = document.getElementById('medium').value;
    let description = document.getElementById('description').value;
    let poster = document.getElementById('poster').value;

    let art = {
        title: title,
        year: year,
        category: category,
        medium: medium,
        description: description,
        poster: poster,
    };

    let xhttp = new XMLHttpRequest();

    xhttp.open('POST', '/artworks/new');

    xhttp.setRequestHeader('Content-Type', 'application/json');

    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            let response = JSON.parse(this.response);

            if (response.message == 'success') {
                window.location.href = `/artworks/${response.id}`;
            } else {
                let error = document.getElementById('formErrors');
                error.innerHTML =
                    '<p>Error creating artwork.<br>Please check values and try again.</p>';
            }
        }
    };

    xhttp.send(JSON.stringify(art));
}
