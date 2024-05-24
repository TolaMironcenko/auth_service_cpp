const host = 'http://localhost:43243'
const prefix = 'api'

const routes = {
    login: () => [host, prefix, 'token'].join('/'),
    user: () => [host, prefix, 'users', 'user'].join('/'),
    all_users: () => [host, prefix, 'users', 'all'].join('/'),
    access: () => [host, prefix, 'access'].join('/'),
    adduser: () => [host, prefix, 'users', 'add'].join('/'),
    deluser: () => [host, prefix, 'users', 'del'].join('/'),
    change_password_url: () => [host, prefix, 'users', 'user', 'password', 'change'].join('/')
}