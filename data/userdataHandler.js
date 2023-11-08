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
    let data = event.data;
    usersArray = data.split(";");
    populateUsers();
    append_addUser();
}

function removeUser(event){
    websocket.send('removeUser=' + event.target.parentElement.parentElement.id);
    console.log( event.target.parentElement.parentElement.id );
}

function populateUsers(){
    var table = document.getElementById('usersTable');
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

function append_addUser(){
    var table = document.getElementById('usersTable');

    var blank = document.createElement('tr');
    blank.classList.add('spacer');
    blank.innerHTML = '<td colspan="3"><br/> </td>'
    table.appendChild(blank);
    
    var tr = document.createElement('tr');
    tr.classList.add('lastRow');
    tr.innerHTML = '<td colspan="2">(Adicionar usuário)</td>' + 
    '<td><a href="/novouser"><button class="button button5">+</button></td>'
    table.appendChild(tr);
}