var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

window.addEventListener('load', onLoad);

function onLoad(event) {
    initWebSocket();
    initButton();
}

function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage;
}
function onOpen(event) {
    get_json_data();
}

function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}
function onMessage(event) {
    console.log(event.data);
    var id = event.data;
    document.getElementById(id).remove();
}

function removeUser(event){
    websocket.send('removeUser=' + event.target.parentElement.parentElement.id);
    console.log( event.target.parentElement.parentElement.id );
  }

//this function is in the event listener and will execute on page load
function get_json_data(){
    // Relative URL of external json file
    var json_url = '/data.json';

    //Build the XMLHttpRequest (aka AJAX Request)
    xmlhttp = new XMLHttpRequest();
    xmlhttp.onreadystatechange = function() { 
        if (this.readyState == 4 && this.status == 200) {//when a good response is given do this
            // append_titles();
            var data = JSON.parse(this.responseText); // convert the response to a json object
            console.log(data)
            append_json(data);// pass the json object to the append_json function
            append_addUser();
        }
    }
    //set the request destination and type
    xmlhttp.open("POST", json_url, true);
    //set required headers for the request
    xmlhttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    // send the request
    xmlhttp.send(); // when the request completes it will execute the code in onreadystatechange section
}

//this function appends the json data to the table 'gable'
function append_json(data){
    var table = document.getElementById('usersTable');
    var elemCount = 0;
    data.credentials.forEach(function(object) {
        console.log(object)
        var tr = document.createElement('tr');
        tr.id = elemCount;
        tr.innerHTML = '<td>' + object.name + '</td>' +
        '<td>' + object.tag + '</td>' +
        '<td><button id="remove' + elemCount + '" class="button button4">X</button></a></td>';
        table.appendChild(tr);
        document.getElementById('remove' + elemCount).addEventListener('click', removeUser);
        elemCount++;
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