const userstablebody = document.querySelector('.alluserstablebody')
const confirmform = document.querySelector('.confirmform')
const confirmformcancel = document.querySelector('.no')
const confirmformok = document.querySelector('.yes')

var useridtodelete = ""

userstablebody.addEventListener('click', e => {
    if (e.target.classList.contains('deleteuser')) {
        useridtodelete = e.target.id
        confirmform.classList.add("active")
    }
    if (e.target.classList.contains('changepassword')) {
        change_password(e.target.id)
    }
})

confirmformcancel.addEventListener('click', () => {
    useridtodelete = ""
    confirmform.classList.remove('active')
})

confirmformok.addEventListener('click', async () => {
    const status = await fetch(routes.deluser(), {
        method: 'POST',
        body: `{"token":"${localStorage.getItem('token')}","userid":"${useridtodelete}"}`
    }).then(data => data.json()).then(jsondata => jsondata).catch((error) => {
        notification(`Ошибка на сервере: ${error}`, "error")
    })
    if (status.status === "ok") {
        confirmform.classList.remove('active')
        get_all_users()
        notification("Пользователь успешно удален", "success")
    }
})
