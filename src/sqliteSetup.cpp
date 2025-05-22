#include "sqliteSetup.h"

const char* data = "Callback function called";
char *zErrMsg = 0;
String message;
sqlite3 *db1;
int rc;

void removeLastChar(){
    int lc = message.length()-1;
    message.remove(lc);
}

static int callback(void *data, int argc, char **argv, char **azColName) {
    int i;
    for (i = 0; i<argc; i++){
        message = message + ("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL") + ",";
    }
    Serial.printf("\n");
    removeLastChar();
    message += ";";
    Serial.println(message);
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

String dbAccessCheck(String tag){
    dbCheck(tag);
    String user = message;
    if (message.equals("FALSE")){
        sqlite3_close(db1);
        postAccess();
        return message;
    }
    else{
        String date = returnTime();
        db_open("/spiffs/users.db", &db1);

        message = "";
        String sql = "SELECT name FROM users WHERE name == '" + user + "';";
        rc = db_exec(db1, sql.c_str());
        removeLastChar();
        String usuario = message;

        message = "";
        sql = "SELECT tag FROM users WHERE name == '" + user + "';";
        rc = db_exec(db1, sql.c_str());
        removeLastChar();
        String tag = message;

        message = "";
        rc = db_exec(db1, "SELECT MAX(id) FROM access;");
        removeLastChar();
        int id = message.toInt() + 1;

        message = "";
        sql = "SELECT act FROM access WHERE name == '" + user + "' ORDER BY date DESC LIMIT 1;";
        rc = db_exec(db1, sql.c_str());
        removeLastChar();
        String lastAct = message;

        //entry or exit?
        String newAct;
        if (lastAct == "Entrada"){
            newAct = "Saída";
        }
        else{
            newAct = "Entrada";
        }
        String returnMessage = usuario + ";" + tag;

        sql = "INSERT INTO access VALUES(" + String(id) + ", '" + String(usuario) + "', '" + String(tag) + "', '" + String(date) + "', '" + String(newAct) + "');";
        rc = db_exec(db1, sql.c_str());
        removeLastChar();

        sqlite3_close(db1);
        delay(500);
        postAccess();
        return returnMessage;
    }
}

    // error handler
    // if (rc != SQLITE_OK) {
    //     sqlite3_close(db1);
    //     return;
    // }
String getData(String numPage, String type){
    String returnMessage = "";
    String sql = "";
    String olderID = "";
    int offset = ((numPage.toInt()) - 1) * 10;

    message = "";
    db_open("/spiffs/users.db", &db1);

    if(type == "users"){
        sql = "SELECT * FROM users ORDER BY id ASC LIMIT 1;";
        rc = db_exec(db1, sql.c_str());
        olderID = message.substring(0, message.indexOf(","));

        message = "";
        sql = "SELECT * FROM users ORDER BY id DESC LIMIT 10 OFFSET " + String(offset) + ";";
        rc = db_exec(db1, sql.c_str());
    }
    else if(type == "access"){
        sql = "SELECT * FROM access ORDER BY id ASC LIMIT 1;";
        rc = db_exec(db1, sql.c_str());
        olderID = message.substring(0, message.indexOf(","));

        message = "";
        sql = "SELECT * FROM access ORDER BY id DESC LIMIT 10 OFFSET " + String(offset) + ";";
        rc = db_exec(db1, sql.c_str());
    }

    sqlite3_close(db1);
    removeLastChar();
    Serial.println(message);
    returnMessage = "oldestID=" + olderID + ";data=" + message;
    return returnMessage;
}

void removeUser(int idDelete){
    db_open("/spiffs/users.db", &db1);
    String sql = "DELETE FROM users WHERE id = " + String(idDelete) + ";";
    rc = db_exec(db1, sql.c_str());
    sqlite3_close(db1);
}

void dbCheck(String id){
    message = "";
    db_open("/spiffs/users.db", &db1);
    String sql = "SELECT IFNULL((SELECT name FROM users WHERE tag == '" + String(id) + "'),'FALSE');";
    rc = db_exec(db1, sql.c_str());
    removeLastChar();

    if (message.equals("FALSE")){
        printMessage("Usuário não cadastrado");
    }  
    else{
        printMessage("Bem vindo(a)\n" + message);
        digitalWrite(led, HIGH);
    }
    sqlite3_close(db1);
    
}

int checkTag(String id){
    message = "";
    db_open("/spiffs/users.db", &db1);
    String sql = "SELECT IFNULL((SELECT tag FROM users WHERE tag == '" + id + "'),'FALSE');";
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
    String sql = "INSERT INTO users VALUES(" + String(id) + ", '" + String(usuario) + "', '" + String(tag) + "');";
    rc = db_exec(db1, sql.c_str());
    sqlite3_close(db1);
}

void clearDB(){
    message = "";
    sqlite3_close(db1);
    db_open("/spiffs/users.db", &db1);
    rc = db_exec(db1, "DELETE FROM access");
    sqlite3_close(db1);
}

String getDB(){
    message = "";
    sqlite3_close(db1);
    db_open("/spiffs/users.db", &db1);
    rc = db_exec(db1, "SELECT name, tag, date, act FROM access;");
    sqlite3_close(db1);

    // Start with UTF-8 BOM
    String csv = "\xEF\xBB\xBF";  // BOM
    csv += "Usuário,TAG,Data,Ação\n";

    // Format message content into CSV
    int col = 0;
    String field = "";
    for (int i = 0; i < message.length(); ) {
        field = "";
        // Extract each field until comma or semicolon
        while (i < message.length() && message.charAt(i) != ',' && message.charAt(i) != ';') {
            field += message.charAt(i++);
        }
    
        // Reformat date if it's the third column
        if (col == 2 && field.length() >= 17) {  // MM/DD/YY HH:mm:ss
            String mm = field.substring(0, 2);
            String dd = field.substring(3, 5);
            String yy = field.substring(6, 8);
            String time = field.substring(9);
            field = dd + "/" + mm + "/20" + yy + " " + time;
        }
    
        csv += field;
    
        if (message.charAt(i) == ';') {
            csv += '\n';
            col = 0;
        } else {
            csv += ',';
            col++;
        }
    
        i++; // skip the comma or semicolon
    }

    return csv;
}

void postAccess(){
    if (stayAwake == false){
        sleepSetup();
    }
    else{
        msgEspLock1();
    }
}

void beginDB() {
    Serial.println("Initializing DB...");
    sqlite3_initialize();
}