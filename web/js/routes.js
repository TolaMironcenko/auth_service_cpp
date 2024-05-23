const host = 'http://localhost:43243'
const prefix = 'api'

const routes = {
    login: () => [host, prefix, 'token'].join('/')
}