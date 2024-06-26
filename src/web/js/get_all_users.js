const get_all_users = async () => {
    const response = await fetch(routes.all_users(), {
        method: "POST",
        body: `{"token":"${localStorage.getItem("token")}"}`
    }).then(data => data.json()).then(jsondata => jsondata).catch((error) => {
        notification(`Ошибка на сервере: ${error}`, "error")
    })
    const alluserstablebody = document.querySelector('.alluserstablebody')
    alluserstablebody.innerHTML = ""
    for (const user of response) {
        alluserstablebody.innerHTML += `
            <tr>
                <th><p>${user.id}</p></th>
                <th>${user.username}</th>
                <th>${user.group}</th>
                <th>${user.is_superuser}</th>
                <th><button class="deleteuser thbutton" id="${user.id}">Delete</button></th>
                <th><button class="changepassword thbutton" id="${user.id}">Change password</button></th>
                <th><button class="changeuser thbutton" id="${user.id}">Change user data</button></th>
            </tr>    
        `
    }
}
