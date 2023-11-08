const char* data = "Callback function called";
char *zErrMsg = 0;
String message = "";
sqlite3 *db1;
int rc;

void removeLastChar(){
    int lc = message.length()-1;
    message.remove(lc);
}

static int callback(void *data, int argc, char **argv, char **azColName) {
    int i;
    for (i = 0; i<argc; i++){
        //Serial.printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        message = message + ("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL") + ",";
    }
    Serial.printf("\n");
    removeLastChar();
    message += ";";
    return 0;
}

int db_open(const char *filename, sqlite3 **db) {
   rc = sqlite3_open(filename, db);
   if (rc) {
       Serial.printf("Can't open database: %s\n", sqlite3_errmsg(*db));
       return rc;
   } else {
       Serial.printf("Opened database successfully\n");
   }
   return rc;
}

int db_exec(sqlite3 *db, const char *sql) {
   Serial.println(sql);
   int rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
   if (rc != SQLITE_OK) {
       Serial.printf("SQL error: %s\n", zErrMsg);
       sqlite3_free(zErrMsg);
   } else {
       Serial.printf("Operation done successfully\n");
   }
   return rc;
}

String getData(){
    message = "";
    db_open("/spiffs/users.db", &db1);
    rc = db_exec(db1, "SELECT * FROM users;");
    // error handler
    // if (rc != SQLITE_OK) {
    //     sqlite3_close(db1);
    //     return;
    // }
    sqlite3_close(db1);
    removeLastChar();
    Serial.println("Message sending: " + message);
    return message;
}

void removeUser(int idDelete){
    db_open("/spiffs/users.db", &db1);
    String sql = "DELETE FROM users WHERE id = " + String(idDelete) + ";";
    Serial.println("SQL command: " + sql);
    rc = db_exec(db1, sql.c_str());
    sqlite3_close(db1);
}

void dbCheck(String id){
    message = "";
    db_open("/spiffs/users.db", &db1);
    String sql = "SELECT IFNULL((SELECT name FROM users WHERE tag == '" + String(id) + "'),'FALSE');";
    Serial.println("SQL command: " + sql);
    rc = db_exec(db1, sql.c_str());
    removeLastChar();

    if (message.equals("FALSE")){
        Serial.print("Usuário não cadastrado");
        printMessage("Usuário não cadastrado");
    }  
    else{
        Serial.print("Bem vindo(a) " + message);
        printMessage("Bem vindo(a) " + message);
    }
    sqlite3_close(db1);
}

int checkTag(String id){
    message = "";
    db_open("/spiffs/users.db", &db1);
    String sql = "SELECT IFNULL((SELECT tag FROM users WHERE tag == '" + id + "'),'FALSE');";
    Serial.println("SQL command: " + sql);
    rc = db_exec(db1, sql.c_str());
    removeLastChar();
    sqlite3_close(db1);
    if (message.equals("FALSE")){
        return 1;
    }  
    else{
        return 0;
    }
}

void addUser(String usuario, String tag){
    message = "";
    db_open("/spiffs/users.db", &db1);
    rc = db_exec(db1, "SELECT MAX(id) FROM users;");
    removeLastChar();
    id = message.toInt() + 1;
    Serial.println("ID: " + id);
    String sql = "INSERT INTO users VALUES(" + String(id) + ", '" + String(usuario) + "', '" + String(tag) + "');";
    Serial.println("SQL command: " + sql);
    rc = db_exec(db1, sql.c_str());
    sqlite3_close(db1);
}

void beginDB() {
    sqlite3_initialize();
}