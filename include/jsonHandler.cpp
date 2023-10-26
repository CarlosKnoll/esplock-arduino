void jsonHandler(String idTag){
    int checkCadastro = 0;
    // Open file
    File dados = SPIFFS.open("/data.json");
    DynamicJsonDocument document(1300);
    deserializeJson(document, dados);
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
    dados.close();
}

int removeUser(int idDelete){
    File dados = SPIFFS.open("/data.json", "r+");
    DynamicJsonDocument document(1300);

    deserializeJson(document, dados);

    JsonArray array = document["credentials"];
    //serializeJson(array, Serial);
    array.remove(idDelete);
    dados.close();

    File dadosW = SPIFFS.open("/data.json", "w+");
    serializeJsonPretty(document, dadosW);
    dados.close();
    return idDelete; 
}