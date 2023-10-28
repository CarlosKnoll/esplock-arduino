var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

window.addEventListener('load', onLoad);

function onLoad(event) {
    initWebSocket();
    initButtons();
}

function initWebSocket() {
    console.log('Trying to open a WebSocket connection...')
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage;
}

function initButtons(){
    document.getElementById('RFID').addEventListener('click', getRFID)
}

function onOpen(event) {
    console.log('Connection opened');
}

function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}
function onMessage(event) {
    console.log(event)
    document.getElementById('tag').value = event.data
    document.getElementById('RFID').innerHTML = 'Ler TAG RFID'
}

function getRFID(){
    document.getElementById('RFID').innerHTML = 'Aguardando TAG...'
    websocket.send('readRFID')
}

function sendInfo(event){
    var message = "addUser:User=" + event.srcElement[0].value + ";ID=" + event.srcElement[1].value
    console.log("Form submitted: User:" + event.srcElement[0].value + "; ID:" + event.srcElement[1].value)
    console.log(event)
    websocket.send(message)
    alert('Usuario adicionado')
}