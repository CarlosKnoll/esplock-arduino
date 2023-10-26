//first add an event listener for page load
document.addEventListener( "DOMContentLoaded", get_json_data, false ); // get_json_data is the function name that will fire on page load

//this function is in the event listener and will execute on page load
function get_json_data(){
    console.log('get_json function fired')
    // Relative URL of external json file
    var json_url = '/data.json';

    //Build the XMLHttpRequest (aka AJAX Request)
    xmlhttp = new XMLHttpRequest();
    xmlhttp.onreadystatechange = function() { 
        if (this.readyState == 4 && this.status == 200) {//when a good response is given do this
            // append_titles();
            var data = JSON.parse(this.responseText); // convert the response to a json object
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

// function append_titles(){
//     var table = document.getElementById('usersTable');
//     var tr = document.createElement('tr');
//     tr.innerHTML = '<td colspan="2">(Adicionar usuário)</td>' + 
//     '<td><a href="/novouser"><button class="button button5">+</button></td>'
//     table.appendChild(tr);
// }

//this function appends the json data to the table 'gable'
function append_json(data){
    var table = document.getElementById('usersTable');
    data.forEach(function(object) {
        var tr = document.createElement('tr');
        tr.innerHTML = '<td>' + object.name + '</td>' +
        '<td>' + object.tag + '</td>' +
        '<td><a href="/remove"><button class="button button4">X</button></a></td>';
        table.appendChild(tr);
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