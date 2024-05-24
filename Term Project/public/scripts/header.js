// send search query to server
function search() {
    let search = document.getElementById('search').value;

    window.location.href = `/search?query=${search}&page=1`;
}

//redirect user to the account page 
function accountRedir() {
    let xhttp = new XMLHttpRequest();

    xhttp.open('GET', '/account');

    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            // redir user
            let response = JSON.parse(this.responseText);
            let loggedIn = response.loggedIn;

            if (loggedIn) {
                window.location.href = `/account/${response.accountID}`;
            } else {
                window.location.href = '/account/login';
            }
        }
    };

    xhttp.send();
}

// redirect user to the home page after logging out
function logout() {
    let xhttp = new XMLHttpRequest();

    xhttp.open('get', '/account/logout');

    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            //redir user
            window.location.href = '/home';
        }
    };

    xhttp.send();
}

// show a list of notifications
function showNotifications() {
    let notification_panel = document.getElementById('notifications');
    notification_panel.innerHTML = '';

    let xhttp = new XMLHttpRequest();

    xhttp.open('GET', '/account/notifications');

    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            let notifications = JSON.parse(this.response).notifications;

            notifications.forEach((notification) => {
                notification_panel.innerHTML += `
                    <div class="notification">
                        <p>${notification.message}</p>
                    </div>
                `;
            });
            notification_panel.innerHTML += `<button onclick=clearNotifications()>Clear Notifications</button>`;
            notification_panel.innerHTML += `<button onclick=window.location.reload()>Close</button>`;
        }
    };

    xhttp.send();
}

// delete notifications
function clearNotifications() {
    let notification_panel = document.getElementById('notifications');
    notification_panel.innerHTML = '';

    let xhttp = new XMLHttpRequest();

    xhttp.open('PUT', '/account/notifications');

    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            window.location.reload();
        }
    };

    xhttp.send();
}
