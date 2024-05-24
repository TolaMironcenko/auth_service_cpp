document.addEventListener("DOMContentLoaded", () => {
    if (localStorage.getItem('token')) {
        fetch(routes.access(), {
            method: 'POST',
            body: `{"token": "${localStorage.getItem('token')}"}`
        }).then(data => data.json()).then(jsondata => {
            if (jsondata.access === "success") {
                document.querySelector('.loginform').classList.remove('active')
                get_all_users()
            } else {
                document.querySelector('.loginform').classList.add('active')
            }
        }).catch((error) => {
            notification(`Ошибка на сервере: ${error}`, "error")
        })
        usernameheader.innerHTML = JSON.parse(localStorage.getItem('userdata')).username
    } else {
        document.querySelector('.loginform').classList.add('active')
    }
})