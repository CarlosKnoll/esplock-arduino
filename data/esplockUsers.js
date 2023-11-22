var gateway = `ws://${window.location.hostname}/ws`;
var websocket;
var usersArray;
var numPage;
var oldestID;
var maxPage;

window.addEventListener('load', onLoad);

function onLoad(event) {
    initWebSocket();
    numPage = 1;
}

function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage;
}
function onOpen(event) {
    websocket.send('populateUsers;numPage=' + numPage);
}

function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}

function onMessage(event) {
    console.log(event.data)
    let data = event.data;
    checkData = data.split("#")
    console.log(checkData)
    if (checkData[0] == 'users'){
        temp = checkData[1].split("=");
        usersArray = temp[2].split(";");
        temp = temp[1].split(";")
        oldestID = temp[0].split(";");
        populateUsers();
        dataControl();
    }
    if (checkData[0] == 'updateUsers' || checkData[0] == 'success'){
        alert('Lista de usuários modificada!');
        numPage = 1;
        websocket.send('populateUsers;numPage=' + numPage);
    }
}

function removeUser(event){
    websocket.send('removeUser=' + event.target.parentElement.parentElement.id + ';numPage=' + numPage);
    console.log( event.target.parentElement.parentElement.id );
}

function populateUsers(){
    var table = document.getElementById('usersTable').tBodies[0];
    table.innerHTML = "";
    usersArray.forEach(element => {
        const currentUser = element.split(",");
        var tr = document.createElement('tr');
        tr.id = currentUser[0];
        if (currentUser[0] == oldestID){
            maxPage = true;
        }
        else{
            maxPage = false;
        }
        tr.innerHTML = '<td>' + currentUser[1] + '</td>' +
        '<td>' + currentUser[2] + '</td>' +
        '<td><button id="remove' + currentUser[0] + '" class="button x-button min-square-button">X</button></td>';
        table.appendChild(tr);
        document.getElementById('remove' + currentUser[0]).addEventListener('click', removeUser);
    });
}

function dataControl(){
    var tr = document.getElementById('pageControl');
    var html = "";
    tr.innerHTML = "";
    console.log(numPage);
    if (numPage == 1){
        html = '<td class="no-border"><button class="button data-button-deactivate">&#60</button></td>'
    }
    else{
        html = '<td class="no-border"><button onclick="pageMinus()" class="button data-button">&#60</button></td>'
    }
    html += '<td class="border" id="numPage">' + numPage + '</td>'
    if (maxPage == false){
        html += '<td class="no-border"><button onclick="pagePlus()" class="button data-button">&#62</button></td>'
    }
    else{
        html += '<td class="no-border"><button class="button data-button-deactivate">&#62</button></td>'
    }
    tr.innerHTML = html
}

function pageMinus(){
    if (numPage != 1){
        numPage = numPage - 1;
        updateData();
    }
}

function pagePlus(){
    numPage = numPage + 1;
    updateData();
}

function updateData(){
    var td = document.getElementById('numPage');
    td.innerHTML = numPage;
    websocket.send('populateUsers;numPage=' + numPage);
}