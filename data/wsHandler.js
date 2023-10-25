var gateway = `ws://${window.location.hostname}/ws`;
var websocket;
window.addEventListener('load', onLoad);

function initWebSocket() {
  console.log('Trying to open a WebSocket connection...');
  websocket = new WebSocket(gateway);
  websocket.onopen    = onOpen;
  websocket.onclose   = onClose;
  websocket.onmessage = onMessage; // <-- add this line
}
function onOpen(event) {
  console.log('Connection opened');
}
function onClose(event) {
  console.log('Connection closed');
  setTimeout(initWebSocket, 2000);
}
function onMessage(event) {
  var stateled;
  var statedisplay;

  console.log(event.data);

  switch (event.data) {		
      case '01':  stateled = "OFF";
                  statedisplay = "ON";
                  document.getElementById('ButtonLED').innerHTML = stateled;
                  document.getElementById('ButtonLED').classList.add('button2')
                  document.getElementById('ButtonDisplay').innerHTML = statedisplay;
                  document.getElementById('ButtonDisplay').classList.remove('button2');
                  break
      case '10':  stateled = "ON";
                  statedisplay = "OFF";
                  document.getElementById('ButtonLED').innerHTML = stateled;
                  document.getElementById('ButtonLED').classList.remove('button2');
                  document.getElementById('ButtonDisplay').innerHTML = statedisplay;
                  document.getElementById('ButtonDisplay').classList.add('button2')
                  break		
      case '11':  stateled = "ON";
                  statedisplay = "ON";
                  document.getElementById('ButtonLED').innerHTML = stateled;
                  document.getElementById('ButtonLED').classList.remove('button2');
                  document.getElementById('ButtonDisplay').innerHTML = statedisplay;
                  document.getElementById('ButtonDisplay').classList.remove('button2');
                  break	
      default:    stateled = "OFF";
                  statedisplay = "OFF";
                  document.getElementById('ButtonLED').innerHTML = stateled;
                  document.getElementById('ButtonLED').classList.add('button2')
                  document.getElementById('ButtonDisplay').innerHTML = statedisplay;
                  document.getElementById('ButtonDisplay').classList.add('button2')
                  break   
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