// comment test

function getRFID(){
    alert("Aguardando TAG")
}

function test(event){
    alert("Form submitted: User:" + event.srcElement[0].value + " ID:" + event.srcElement[1].value)
    console.log(event)
}