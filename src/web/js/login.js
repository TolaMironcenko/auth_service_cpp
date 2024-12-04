const loginform = document.querySelector('.loginform')
const loginformbody = document.querySelector('.loginformbody')
const loginforminputusername = document.querySelector('#loginforminputusername')
const loginforminputpassword = document.querySelector('#loginforminputpassword')
const exitbutton = document.querySelector('.exitbutton')
const usernameheader = document.querySelector('.usernameheader')

if (localStorage.getItem("token") === null) {
    loginform.classList.add("active")
}

loginformbody.addEventListener('submit', (e) => {
    e.preventDefault()
    hashPassword(loginforminputpassword.value).then(hashedpassword => {
      login(loginforminputusername.value, hashedpassword)
    })
})

const hashPassword = async (password) => {
  const encoder = new TextEncoder()
  const data = encoder.encode(password)
  const hashBuffer = await crypto.subtle.digest('SHA-256', data)
  const hashArray = Array.from(new Uint8Array(hashBuffer))
  const hashHex = hashArray.map(b=>('00' + b.toString(16)).slice(-2)).join('')
  return hashHex
}

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
    }).then(data => data.json()).then(jsondata => jsondata).catch((error) => {
        if (error.message === "Unexpected end of JSON input") {
            notification(`Ошибка на сервере: Создайте файл users.json и запишите в него первого пользователя`, "error")
        } else {
            notification(`Ошибка на сервере: ${error}`, "error")
        }
    })
    if (response.status === "403") {
        notification("Неправильный логин или пароль", "error")
    }
    if (response.token) {
        notification("Успешно", "success")
        loginform.classList.remove("active")
        localStorage.setItem("token", response.token)
        await get_user_data()
        loginforminputusername.value = ""
        loginforminputpassword.value = ""
    }
}

const get_user_data = async () => {
    const response = await fetch(routes.user(), {
        method: "POST",
        body: `{"token":"${localStorage.getItem("token")}"}`
    }).then(data => data.json()).then(jsondata => jsondata).catch((error) => {
        notification(`Ошибка на сервере: ${error}`, "error")
    })
    if (response.is_superuser === "1") {
        await get_all_users()
        usernameheader.innerHTML = response.username
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
