document.addEventListener("DOMContentLoaded", () => {
    if (localStorage.getItem('token')) {
        fetch(routes.access(), {
            method: 'POST',
            body: `{"token": "${localStorage.getItem('token')}"}`
        }).then(data => data.json()).then(jsondata => {
            if (jsondata.access === "success") {
                document.querySelector('.loginform').classList.remove('active')
                get_all_users()
            }
        })
    }
})