const changeuserform = document.querySelector('.changeuserform')
const changeuserformbody = document.querySelector('.changeuserformbody')
const changeuserformusername = document.querySelector('#changeuserformusername')
const changeuserformpassword = document.querySelector('#changeuserformpassword')
const changeuserformgroup = document.querySelector('#changeuserformgroup')
const changeuserformissuperuser = document.querySelector('#changeuserformissuperuser')
const cancelchangeuser = document.querySelector(".cancelchangeuser")

let changedatauserid = "";
let userdata = "";

const change_user_data = async (userid) => {
    changeuserform.classList.add('active')
    changedatauserid = userid
    const all_users = await fetch(routes.all_users(), {
        method: 'POST',
        body: `{"token":"${localStorage.getItem('token')}"}`
    }).then(data => data.json()).then(jsondata => jsondata).catch((error) => {
        notification(`Ошибка на сервере: ${error}`, "error")
    })
    userdata = all_users.find(usr => usr.id === userid)
    changeuserformusername.value = userdata.username
    changeuserformgroup.value = userdata.group
    changeuserformissuperuser.checked = userdata.is_superuser === "1"
}

cancelchangeuser.addEventListener('click', (e) => {
    e.preventDefault()
    changeuserform.classList.remove('active')
})

changeuserformbody.addEventListener('submit', async (e) => {
    e.preventDefault()
    let request = {
        token: localStorage.getItem('token'),
        userid: changedatauserid,
        username: changeuserformusername.value,
        is_superuser: changeuserformissuperuser.checked ? "1" : "0",
        group: changeuserformgroup.value
    };
    if (changeuserformpassword.value !== "") {request = {...request, "password": changeuserformpassword.value}}
    const status = await fetch(routes.change_user(), {
        method: 'POST',
        body: JSON.stringify(request)
    }).then(data => data.json()).then(jsondata => jsondata).catch((error) => {
        notification(`Ошибка на сервере: ${error}`, "error")
    })
    if (status.status === "ok") {
        notification("Данные пользователя успешно изменены", "success")
        changeuserform.classList.remove('active')
        await get_all_users()
    }
})
