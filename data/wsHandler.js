var gateway = `ws://${window.location.hostname}/ws`;
var websocket;
var stateled;
var statedisplay;

window.addEventListener('load', onLoad);

function initWebSocket() {
  console.log('Trying to open a WebSocket connection...');
  websocket = new WebSocket(gateway);
  websocket.onopen    = onOpen;
  websocket.onclose   = onClose;
  websocket.onmessage = onMessage;
}
function onOpen(event) {
  console.log('Connection opened');
  websocket.send('getMessage');
}
function onClose(event) {
  console.log('Connection closed');
  setTimeout(initWebSocket, 2000);
}
function onMessage(event) {
  console.log(event.data);
  switch (event.data) {		
      case '01':  stateled = "OFF";
                  statedisplay = "ON";
                  break
      case '10':  stateled = "ON";
                  statedisplay = "OFF";
                  break		
      case '11':  stateled = "ON";
                  statedisplay = "ON";
                  break	
      default:    stateled = "OFF";
                  statedisplay = "OFF";
                  break   
  }
  updateHTML();
}

function updateHTML(){
  document.getElementById('ButtonLED').innerHTML = stateled;
  document.getElementById('ButtonDisplay').innerHTML = statedisplay;
  if (stateled == "ON"){
    document.getElementById('ButtonLED').classList.remove('button2');
  }
  else if (stateled == "OFF"){
    document.getElementById('ButtonLED').classList.add('button2');
  }
  if (statedisplay == "ON"){
    document.getElementById('ButtonDisplay').classList.remove('button2');
  }
  else if (statedisplay == "OFF"){
    document.getElementById('ButtonDisplay').classList.add('button2');
  }
}

function onLoad(event) {
  initWebSocket();
  initButton();
}

function initButton() {
  document.getElementById('ButtonLED').addEventListener('click', toggleLed);
  document.getElementById('ButtonDisplay').addEventListener('click', toggleDisplay);
}

function toggleLed(){
  websocket.send('toggleLED');
}

function toggleDisplay(){
  websocket.send('toggleDisplay');
}