const loginform = document.querySelector('.loginform')
const loginformbody = document.querySelector('.loginformbody')
const loginforminputusername = document.querySelector('#loginforminputusername')
const loginforminputpassword = document.querySelector('#loginforminputpassword')
const loginformbutton = document.querySelector('.loginformbutton')

// loginformbody.unbind('submit')

loginformbody.addEventListener('submit', (e) => {
    e.preventDefault()
    console.log('login clicked')
    console.log(loginforminputusername.value, loginforminputpassword.value)
    login(loginforminputusername.value, loginforminputpassword.value)
    // console.log(loginform, loginforminputpassword, loginforminputusername, loginformbutton)
})

const login = async (username, password) => {
    const response = await fetch(routes.login(), {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        mode: 'no-cors',
        body: `{"username":"${username}","password":"${password}"}`
    })
    console.log(response.json())
}
