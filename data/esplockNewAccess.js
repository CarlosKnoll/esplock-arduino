var gateway = `ws://${window.location.hostname}/ws`;
var websocket;
var rfidStatus = false;

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
    var client = ''
    console.log(event.data)
    checkData = event.data.split("=")
    if (checkData[0] == 'NewAccess'){
        if(checkData[1] == 'FALSE'){
            client = 'Usuário não cadastrado'
        }
        else{
            const user = checkData[1].split(";");
            client = 'Bem vindo ' + user[0] + '! <br> (TAG: ' + user[1] + ')'
            websocket.send('populateAccess');
        }  
        document.getElementById('feedback').innerHTML = 'Tentativa de acesso detectada. <br>' + client 
        rfidStatus = false
        document.getElementById('RFID').innerHTML = 'Ler TAG RFID'      
    }
    else if(checkData[0] == 'cancel'){
        rfidStatus = false
        document.getElementById('RFID').innerHTML = 'Ler TAG RFID'   
    }
}

function getRFID(){
    rfidStatus = !rfidStatus    
    if(rfidStatus == true){
        document.getElementById('RFID').innerHTML = 'Aguardando TAG...'
        websocket.send('accessRFID')
    }
    if(rfidStatus == false){
        websocket.send('cancelRFID')
    }
}