void jsonHandler(String idTag){
    int checkCadastro = 0;
    // Open file
    File dados = SPIFFS.open("/data.json");

    DynamicJsonDocument document(1300);
    JsonArray array = document.to<JsonArray>();
    deserializeJson(document, dados);
    
    for (JsonObject elem : document.as<JsonArray>()) {
        String tag1 = elem["tag"];
        if (tag1 == idTag){
            String user = elem["name"];
            Serial.print("Bem vindo(a) " + user);
            checkCadastro = 1;
            break;
        }

    }  
    if (checkCadastro == 0){
        Serial.print("Usuário não cadastrado");
    }
    dados.close();
}