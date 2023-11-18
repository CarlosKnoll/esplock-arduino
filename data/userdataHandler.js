var gateway = `ws://${window.location.hostname}/ws`;
var websocket;
var usersArray;

window.addEventListener('load', onLoad);

function onLoad(event) {
    initWebSocket();
}

function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage;
}
function onOpen(event) {
    websocket.send('populateUsers');
}

function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}

function onMessage(event) {
    console.log(event.data)
    let data = event.data;
    checkData = data.split("=")
    console.log(checkData)
    if (checkData[0] == 'users'){
        usersArray = checkData[1].split(";");
        populateUsers();
        append_extra();
    }
}

function removeUser(event){
    websocket.send('removeUser=' + event.target.parentElement.parentElement.id);
    console.log( event.target.parentElement.parentElement.id );
}

function populateUsers(){
    var table = document.getElementById('usersTable').tBodies[0];
    table.innerHTML = "";
    usersArray.forEach(element => {
        const currentUser = element.split(",");
        var tr = document.createElement('tr');
        tr.id = currentUser[0];
        tr.innerHTML = '<td>' + currentUser[1] + '</td>' +
        '<td>' + currentUser[2] + '</td>' +
        '<td><button id="remove' + currentUser[0] + '" class="button button4">X</button></a></td>';
        table.appendChild(tr);
        document.getElementById('remove' + currentUser[0]).addEventListener('click', removeUser);
    });
}

function append_extra(){
    var table = document.getElementById('usersTable').tBodies[0];

    var blank = document.createElement('tr');
    blank.classList.add('spacer');
    blank.innerHTML = '<td colspan="3"><br/> </td>'
    table.appendChild(blank);
    
    var add = document.createElement('tr');
    add.classList.add('lastRow');
    add.innerHTML = '<td colspan="2">(Adicionar usuário)</td>' + 
    '<td><a href="/novouser"><button class="button button5">+</button></td>'
    table.appendChild(add);

    var goBack = document.createElement('tr');
    goBack.innerHTML = '<td colspan="2"><button onclick="history.back()" class="button button3">Retornar</button></td>'
    table.appendChild(goBack);
}