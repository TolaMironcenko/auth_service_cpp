const message = document.querySelector('.message')
const messagetext = document.querySelector('.messagetext')

const notification = (text, type) => {
    if (type === "error") {
        message.classList.remove('success')
        message.classList.add('error')
    } else {
        message.classList.remove('error')
        message.classList.add('success')
    }
    messagetext.innerHTML = text
    message.classList.add('active')
    setTimeout(() => {
        message.classList.remove('active')
    }, 2000)
}

