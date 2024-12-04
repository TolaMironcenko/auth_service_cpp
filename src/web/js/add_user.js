const adduserbutton = document.querySelector('.adduserbutton')
const adduserform = document.querySelector('.adduserform')
const canceladduserbutton = document.querySelector('.canceladduser')
const adduserformbody = document.querySelector('.adduserformbody')
const usernameinput = document.querySelector('#adduserformusername')
const passwordinput = document.querySelector('#adduserformpassword')
const groupinput = document.querySelector('#adduserformgroup')
const is_superuserinput = document.querySelector('#adduserformissuperuser')

adduserbutton.addEventListener('click', () => {
    adduserform.classList.add('active')
})

canceladduserbutton.addEventListener('click', (e) => {
    e.preventDefault()
    adduserform.classList.remove('active')
})

adduserformbody.addEventListener('submit', async (e) => {
    e.preventDefault()
    const hashedpassword = await hashPassword(passwordinput.value)
    const new_user = {
        username: usernameinput.value,
        password: hashedpassword,
        group: groupinput.value,
        is_superuser: is_superuserinput.checked?"1":"0"
    }
    const status = await fetch(routes.adduser(), {
        method: 'POST',
        body: `{"token":"${localStorage.getItem('token')}","username":"${new_user.username}","password":"${new_user.password}","group":"${new_user.group}","is_superuser":"${new_user.is_superuser}"}`
    }).then(data => data.json()).then(jsondata => jsondata).catch((error) => {
        notification(`Ошибка на сервере: ${error}`, "error")
    })
    if (status.status === "ok") {
        adduserform.classList.remove('active')
        await get_all_users()
        notification("Пользователь успешно добавлен", "success")
        usernameinput.value = ""
        passwordinput.value = ""
        groupinput.value = ""
    }
    if (status.status === "user already exists") {
        notification("Пользователь с таким именем уже существует", "error")
    }
})
