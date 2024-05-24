const changepasswordform = document.querySelector('.changepasswordform')
const cancelchangepassword = document.querySelector('.cancelchangepassword')
const changepasswordformbody = document.querySelector('.changepasswordformbody')
const changepasswordnewpassword = document.querySelector('#changepasswordnewpassword')

var changepassuserid = ""

const change_password = (userid) => {
    changepasswordform.classList.add('active')
    changepassuserid = userid
}

cancelchangepassword.addEventListener('click', e => {
    e.preventDefault()
    changepasswordform.classList.remove('active')
})

changepasswordformbody.addEventListener('submit', async e => {
    e.preventDefault()
    const status = await fetch(routes.change_password_url(), {
        method: 'POST',
        body: `{"token":"${localStorage.getItem('token')}","userid":"${changepassuserid}","new_password":"${changepasswordnewpassword.value}"}`
    }).then(data => data.json()).then(jsondata => jsondata)
    console.log(status)
    if (status.status === "ok") {
        notification("Пароль успешно изменен", "success")
        changepasswordform.classList.remove('active')
    }
})
