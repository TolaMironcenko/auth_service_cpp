const loginform = document.querySelector('.loginform')
const loginformbody = document.querySelector('.loginformbody')
const loginforminputusername = document.querySelector('#loginforminputusername')
const loginforminputpassword = document.querySelector('#loginforminputpassword')
const loginformbutton = document.querySelector('.loginformbutton')
const exitbutton = document.querySelector('.exitbutton')

// loginformbody.unbind('submit')

loginformbody.addEventListener('submit', (e) => {
    e.preventDefault()
    login(loginforminputusername.value, loginforminputpassword.value)
    // console.log(loginform, loginforminputpassword, loginforminputusername, loginformbutton)
})

const login = async (username, password) => {
    const response = await fetch(routes.login(), {
        method: 'POST',
        headers: {
            Accept: 'application/json',
            // 'Access-Control-Allow-Origin': '*',
            'Content-Type': 'application/json',
        },
        // mode: 'no-cors',
        body: `{"username":"${username}","password":"${password}"}`
    }).then(data => data.json()).then(jsondata => jsondata)
    if (response.status === "403") {
        notification("Неправильный логин или пароль", "error")
    }
    if (response.token) {
        notification("Успешно", "success")
        loginform.classList.remove("active")
        localStorage.setItem("token", response.token)
        get_user_data(response.token)
    }
}

const get_user_data = async (token) => {
    const response = await fetch(routes.user(), {
        method: "POST",
        body: `{"token":"${localStorage.getItem("token")}"}`
    }).then(data => data.json()).then(jsondata => jsondata)
    if (response.is_superuser === "1") {
        get_all_users()
    } else {
        document.querySelector('.alluserstablebody').innerHTML = ""
        loginform.classList.add("active")
        notification("Вы не можете войти потому что не являетесь администратором", "error")
        localStorage.clear()
    }
    if (response.id) {
        localStorage.setItem("userdata", JSON.stringify(response))
    }
}

message.addEventListener('click', () => {
    message.classList.remove("active")
})

exitbutton.addEventListener('click', () => {
    localStorage.clear()
    loginform.classList.add('active')
    notification('Вы вышли', "success")
})
