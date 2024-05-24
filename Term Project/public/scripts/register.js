// send register information to the server to create new user
function register() {
    let xhttp = new XMLHttpRequest();

    xhttp.open('POST', '/account/register');

    xhttp.setRequestHeader('Content-Type', 'application/json');

    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            // redir user
            let response = JSON.parse(this.responseText);
            let loggedIn = response.loggedIn;

            if (loggedIn) {
                window.location.href = `/account/${response.accountID}`;
            } else {
                let formErrors = document.getElementById('formErrors');
                formErrors.innerHTML = '';
                Object.keys(response.error.errors).forEach(function (error) {
                    formErrors.innerHTML += `<p>ERROR: ${response.error.errors[error].message}</p>`;
                });
            }
        }
    };

    let fullname = document.getElementById('fullname').value;
    let username = document.getElementById('username').value;
    let password = document.getElementById('password').value;
    let passwordConfirm = document.getElementById('passwordConfirm').value;

    if (password != passwordConfirm) {
        // error message for password mismatch
        let error = document.getElementById('formErrors');
        error.innerHTML = 'ERROR: Passwords do not match.';
        return;
    }

    let user_data = {
        fullname: fullname,
        username: username,
        password: password,
    };

    xhttp.send(JSON.stringify(user_data));
}
