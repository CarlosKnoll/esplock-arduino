DynamicJsonDocument document(1300);

void readJSON(){
    File dados = SPIFFS.open("/data.json", "r+");   //Open JSON file
    deserializeJson(document, dados);               //Deserialize file into document object
    dados.close();                                  //Close file
}

void writeJSON(){
    File dados = SPIFFS.open("/data.json", "w+");   //Open JSON file -> w+ mode means truncate file
    serializeJsonPretty(document, dados);           //Serialize document object data back into JSON file
    dados.close();                                  //Close file
}

void jsonHandler(String idTag){
    int checkCadastro = 0;
    readJSON();
    JsonArray array = document["credentials"];
    //serializeJson(array, Serial);

    for (JsonObject elem : array) {
        String tag1 = elem["tag"];
        if (tag1 == idTag){
            String user = elem["name"];
            Serial.print("Bem vindo(a) " + user);
            printMessage("Bem vindo(a) " + user);
            checkCadastro = 1;
            break;
        }
    }  
    if (checkCadastro == 0){
        Serial.print("Usuário não cadastrado");
        printMessage("Usuário não cadastrado");
    }
}

int removeUser(int idDelete){
    readJSON();
    JsonArray array = document["credentials"];
    //serializeJson(array, Serial);
    array.remove(idDelete);
    writeJSON();
    return idDelete; 
}

void addUser( String usuario, String tag ){
    readJSON();
    JsonArray array = document["credentials"];
    //serializeJson(array, Serial);
    JsonObject newUser = array.createNestedObject();
    newUser["name"] = usuario;
    newUser["tag"] = tag;
    serializeJson(document, Serial);
    writeJSON();
}