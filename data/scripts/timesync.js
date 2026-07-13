var tsGateway = `ws://${window.location.hostname}/ws`;
var tsWebsocket;

window.addEventListener('load', initTimeSync);

function initTimeSync() {
    tsWebsocket = new WebSocket(tsGateway);
    tsWebsocket.onmessage = onTimeSyncMessage;
}

function onTimeSyncMessage(event) {
    if (event.data === "requestEpoch") {
        sendEpoch();
        tsWebsocket.close();
    }
}

function sendEpoch() {
    const d = new Date();
    let message = "epoch=";
    message += (d.getSeconds() + ',');
    message += (d.getMinutes() + ',');
    message += (d.getHours() + ',');
    message += (d.getDate() + ',');
    message += (d.getMonth() + ',');
    message += (d.getFullYear());
    console.log(message);
    tsWebsocket.send(message);
}