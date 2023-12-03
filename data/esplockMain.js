var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

window.addEventListener('load', onLoad);

function onLoad(event) {
    initWebSocket();
}

function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
}
function onOpen(event) {
    const d = new Date()
    let message = "epoch="
    message += (d.getSeconds() + ',') 
    message += (d.getMinutes() + ',')
    message += (d.getHours() + ',')
    message += (d.getDate() + ',')
    message += (d.getMonth() + ',')
    message += (d.getFullYear())
    console.log(message)
    websocket.send(message)
}