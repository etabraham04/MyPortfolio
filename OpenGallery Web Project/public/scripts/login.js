// send login information to the server
function login() {
    let xhttp = new XMLHttpRequest();

    xhttp.open('POST', '/account/login');

    xhttp.setRequestHeader('Content-Type', 'application/json');

    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            // redir user
            let response = JSON.parse(this.responseText);
            let loggedIn = response.loggedIn;

            if (loggedIn) {
                window.location.href = `/account/${response.accountID}`;
            } else {
                let error = document.getElementById('formErrors');
                error.innerHTML = 'Invalid username or password.';
            }
        }
    };

    let username = document.getElementById('username').value;
    let password = document.getElementById('password').value;

    let user_data = {
        username: username,
        password: password,
    };

    xhttp.send(JSON.stringify(user_data));
}
