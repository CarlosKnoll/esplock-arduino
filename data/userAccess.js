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
    websocket.send('populateAccess');
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
    if (checkData[0] == 'access'){
        accessArray = data.split(";");
        populateTable();
    }
}

function populateTable(){
    var table = document.getElementById('accessTable').tBodies[0];;
    table.innerHTML = "";
    accessArray.forEach(element => {
        const currentUser = element.split(",");
        var tr = document.createElement('tr');
        tr.id = currentUser[0];
        tr.innerHTML = '<td>' + currentUser[1] + '</td>' +
        '<td>' + currentUser[2] + '</td>' +
        '<td>' + currentUser[3] + '</td>'
        table.appendChild(tr);
    });
}