#include "sqliteSetup.h"
#include "actuationSetup.h"

sqlite3 *db1;
int responseCode;

// ----------------------------------------------------------------------------
// SQL queries definition
const char *getIdFromUsersQuery =           "SELECT MAX(id) FROM users;";

const char *getIdFromAccessQuery =          "SELECT MAX(id) FROM access;";
const char *getLastActFromAccessQuery =     "SELECT act FROM access WHERE name = ? ORDER BY id DESC LIMIT 1;";

const char *insertAccessQuery =             "INSERT INTO access VALUES(?, ?, ?, ?, ?);";

const char *getUserDataQuery =              "SELECT * FROM users ORDER BY id ASC LIMIT 1;";
const char *getUserDataLimQuery =           "SELECT * FROM users ORDER BY id DESC LIMIT 10 OFFSET ?;";

const char *getAccessDataQuery =            "SELECT * FROM access ORDER BY id ASC LIMIT 1;";
const char *getAccessDataLimQuery =         "SELECT * FROM access ORDER BY id DESC LIMIT 10 OFFSET ?;";

const char *deleteUserQuery =               "DELETE FROM users WHERE id = ?;";

const char *checkUserQuery =                "SELECT IFNULL((SELECT name FROM users WHERE tag = ?),'FALSE');";
const char *checkTagQuery =                 "SELECT IFNULL((SELECT tag FROM users WHERE tag = ?),'FALSE');";

const char *insertUserQuery =               "INSERT INTO users VALUES(?, ?, ?);";

const char *clearAccessQuery =              "DELETE FROM access;";

const char *getAccessDBQuery =              "SELECT name, tag, date, act FROM access;";

// ----------------------------------------------------------------------------
// Database prep
void beginDB() {
    Serial.println("Initializing DB...");
    sqlite3_initialize();
}

int db_open(const char *filename, sqlite3 **db) {
   responseCode = sqlite3_open(filename, db);
   if (responseCode) {
       Serial.printf("[SQLITE3] Can't open database: %s\n", sqlite3_errmsg(*db));
       return responseCode;
   } else {
       Serial.printf("[SQLITE3] Opened database successfully\n");
   }
   return responseCode;
}

void postAccess(){
    if (stayAwake == false){
        sleepSetup();
    }
    else{
        msgEspLock1();
    }
}

// ----------------------------------------------------------------------------
// SQL functions

String dbAccessCheck(String tag){
    // Initialize insert variables for registration if the access is granted
    int id = 1; // Initialize id to 1 in case table is empty
    String user = "FALSE";
    // Tag is already known from the initial call
    String date; // Not initialized here, will be set only if access is granted.
    String newAct = "Entrada"; // Initialize newAct to "Entrada" in case of new users.
    

    sqlite3_stmt *stmt = nullptr;
    db_open("/spiffs/users.db", &db1);
    

    // Check if the tag exists in the users table
    responseCode = sqlite3_prepare_v2(db1, checkUserQuery, -1, &stmt, nullptr);
    if (responseCode != SQLITE_OK) {
        Serial.printf("[SQLITE3] Prepare failed: %s\n", sqlite3_errmsg(db1));
        goto cleanup;
    }
    sqlite3_bind_text(stmt, 1, tag.c_str(), -1, SQLITE_TRANSIENT);
    responseCode = sqlite3_step(stmt);
    if (responseCode == SQLITE_ROW) {
        const unsigned char *text = sqlite3_column_text(stmt, 0);
        if (text) {
            user = String((const char *)text);
        }
    } else {
        Serial.printf("[SQLITE3] Step failed: %s\n", sqlite3_errmsg(db1));
    }
    sqlite3_finalize(stmt);
    stmt = nullptr;


    if (user == "FALSE") { // If tag is not in the database, return early
        Serial.println("[ACCESS DENIED] Tag not found in database.");
        goto cleanup;
    }


    date = returnTime(); // If not return early, access is granted, date is set.

    // Query to get the next row ID for access registering
    responseCode = sqlite3_prepare_v2(db1, getIdFromAccessQuery, -1, &stmt, nullptr);
    if (responseCode != SQLITE_OK) {
        Serial.printf("[SQLITE3] Prepare failed: %s\n", sqlite3_errmsg(db1));
        goto cleanup;
    }
    responseCode = sqlite3_step(stmt);
    if (responseCode == SQLITE_ROW) {
        const unsigned char *text = sqlite3_column_text(stmt, 0);
        if (text) {
            id = String((const char *)text).toInt() + 1;
        }
    } else {
        Serial.printf("[SQLITE3] Step failed: %s\n", sqlite3_errmsg(db1));
    }
    sqlite3_finalize(stmt);
    stmt = nullptr;


    // Query to get last action and flip it for new action
    responseCode = sqlite3_prepare_v2(db1, getLastActFromAccessQuery, -1, &stmt, nullptr);
    if (responseCode != SQLITE_OK) {
        Serial.printf("[SQLITE3] Prepare failed: %s\n", sqlite3_errmsg(db1));
        goto cleanup;
    }
    sqlite3_bind_text(stmt, 1, user.c_str(), -1, SQLITE_TRANSIENT);
    responseCode = sqlite3_step(stmt);
    if (responseCode == SQLITE_ROW) {
        const unsigned char *text = sqlite3_column_text(stmt, 0);
        if (text) {
            newAct = (String((const char *)text) == "Entrada") ? "Saída" : "Entrada";
        }
    } else if (responseCode != SQLITE_DONE) {
        Serial.printf("[SQLITE3] Step failed: %s\n", sqlite3_errmsg(db1));
    }
    sqlite3_finalize(stmt);
    stmt = nullptr;

    
    // Final insert query to register the access
    responseCode = sqlite3_prepare_v2(db1, insertAccessQuery, -1, &stmt, nullptr);
    if (responseCode != SQLITE_OK) {
        Serial.printf("[SQLITE3] Prepare failed: %s\n", sqlite3_errmsg(db1));
        goto cleanup;
    }
    sqlite3_bind_int(stmt, 1, id);
    sqlite3_bind_text(stmt, 2, user.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, tag.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, date.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, newAct.c_str(), -1, SQLITE_TRANSIENT);
    responseCode = sqlite3_step(stmt);
    if (responseCode != SQLITE_DONE) {
        Serial.printf("[SQLITE3] Insert failed: %s\n", sqlite3_errmsg(db1));
        goto cleanup;
    }

    // Feedback and actuation
    Serial.println("[ACCESS GRANTED] " + user + " - " + newAct);
    actuate_lock();


    cleanup:
        if (stmt) {sqlite3_finalize(stmt); stmt = nullptr;}
        if (db1)  {sqlite3_close(db1); db1 = nullptr;}
        postAccess();
        return (user == "FALSE") ? "FALSE" : (user + ";" + tag); // Return user;tag if access granted, else return FALSE string
}


String getData(String numPage, String type){
    String olderID = "";
    String data = "";
    int offset = ((numPage.toInt()) - 1) * 10;


    sqlite3_stmt *stmt = nullptr;
    db_open("/spiffs/users.db", &db1);
    
    if(type == "users"){ // Build up the response data for users table
        responseCode = sqlite3_prepare_v2(db1, getUserDataQuery, -1, &stmt, nullptr);
        if (responseCode != SQLITE_OK) {
            Serial.printf("[SQLITE3] Prepare failed: %s\n", sqlite3_errmsg(db1));
            goto cleanup;
        }
        responseCode = sqlite3_step(stmt);
        if (responseCode == SQLITE_ROW) {
            const unsigned char *text = sqlite3_column_text(stmt, 0);
            if (text) {
                olderID = String((const char *)text); 
            }
        } else if (responseCode != SQLITE_DONE) {
            Serial.printf("[SQLITE3] Step failed: %s\n", sqlite3_errmsg(db1));
            goto cleanup;
        }
        sqlite3_finalize(stmt);
        stmt = nullptr;


        responseCode = sqlite3_prepare_v2(db1, getUserDataLimQuery, -1, &stmt, nullptr);
        if (responseCode != SQLITE_OK) {
            Serial.printf("[SQLITE3] Prepare failed: %s\n", sqlite3_errmsg(db1));
            goto cleanup;
        }
        sqlite3_bind_int(stmt, 1, offset);

        while ((responseCode = sqlite3_step(stmt)) == SQLITE_ROW) {
            int cols = sqlite3_column_count(stmt);
            for (int i = 0; i < cols; i++) {
                const unsigned char *text = sqlite3_column_text(stmt, i);
                data += text ? String((const char *)text) : "";
                if (i < cols - 1) data += ",";
            }
            data += ";";
        }
        if (data.length() > 0) {
            data.remove(data.length() - 1); // drop trailing ';'
        }
        if (responseCode != SQLITE_DONE) {
            Serial.printf("[SQLITE3] Step failed: %s\n", sqlite3_errmsg(db1));
            goto cleanup;
        }

        sqlite3_finalize(stmt);
        stmt = nullptr;

    }
    else if(type == "access"){ // Build up the response data for users table
        responseCode = sqlite3_prepare_v2(db1, getAccessDataQuery, -1, &stmt, nullptr);
        if (responseCode != SQLITE_OK) {
            Serial.printf("[SQLITE3] Prepare failed: %s\n", sqlite3_errmsg(db1));
            goto cleanup;
        }
        responseCode = sqlite3_step(stmt);
        if (responseCode == SQLITE_ROW) {
            const unsigned char *text = sqlite3_column_text(stmt, 0);
            if (text) {
                olderID = String((const char *)text);
            }
        } else if (responseCode != SQLITE_DONE) {
            Serial.printf("[SQLITE3] Step failed: %s\n", sqlite3_errmsg(db1));
            goto cleanup;
        }
        sqlite3_finalize(stmt);
        stmt = nullptr;


        responseCode = sqlite3_prepare_v2(db1, getAccessDataLimQuery, -1, &stmt, nullptr);
        if (responseCode != SQLITE_OK) {
            Serial.printf("[SQLITE3] Prepare failed: %s\n", sqlite3_errmsg(db1));
            goto cleanup;
        }
        sqlite3_bind_int(stmt, 1, offset);

        while ((responseCode = sqlite3_step(stmt)) == SQLITE_ROW) {
            int cols = sqlite3_column_count(stmt);
            for (int i = 0; i < cols; i++) {
                const unsigned char *text = sqlite3_column_text(stmt, i);
                data += text ? String((const char *)text) : "";
                if (i < cols - 1) data += ",";
            }
            data += ";";
        }
        if (data.length() > 0) {
            data.remove(data.length() - 1); // drop trailing ';'
        }
        if (responseCode != SQLITE_DONE) {
            Serial.printf("[SQLITE3] Step failed: %s\n", sqlite3_errmsg(db1));
            goto cleanup;
        }
    }

    cleanup:
        if (stmt) {sqlite3_finalize(stmt); stmt = nullptr;}
        if (db1)  {sqlite3_close(db1); db1 = nullptr;}
        return (olderID == "") ? "empty" : ("oldestID=" + olderID + ";data=" + data);
}


bool addUser(String usuario, String tag){
    bool tagExists = false;
    int id = 1; // Initialize id to 1 in case table is empty

    sqlite3_stmt *stmt = nullptr;
    db_open("/spiffs/users.db", &db1);

    responseCode = sqlite3_prepare_v2(db1, checkTagQuery, -1, &stmt, nullptr);
    if (responseCode != SQLITE_OK) {
        Serial.printf("[SQLITE3] Prepare failed: %s\n", sqlite3_errmsg(db1));
        goto cleanup;
    }
    sqlite3_bind_text(stmt, 1, tag.c_str(), -1, SQLITE_TRANSIENT);
    responseCode = sqlite3_step(stmt);
    if (responseCode == SQLITE_ROW) {
        const unsigned char *text = sqlite3_column_text(stmt, 0);
        if (text) {
            tagExists = (String((const char *)text) == tag) ? true : false;
        }
    } else if (responseCode != SQLITE_DONE) {
        Serial.printf("[SQLITE3] Step failed: %s\n", sqlite3_errmsg(db1));
        goto cleanup;
    }
    sqlite3_finalize(stmt);
    stmt = nullptr;

    if (!tagExists) { // If tag does not exist, proceed to insert the new user
        responseCode = sqlite3_prepare_v2(db1, getIdFromUsersQuery, -1, &stmt, nullptr);
        if (responseCode != SQLITE_OK) {
            Serial.printf("[SQLITE3] Prepare failed: %s\n", sqlite3_errmsg(db1));
            goto cleanup;
        }
        responseCode = sqlite3_step(stmt);
        if (responseCode == SQLITE_ROW) {
            const unsigned char *text = sqlite3_column_text(stmt, 0);
            if (text) {
                id = String((const char *)text).toInt() + 1;
            }
        } else if (responseCode != SQLITE_DONE) {
            Serial.printf("[SQLITE3] Step failed: %s\n", sqlite3_errmsg(db1));
            goto cleanup;
        }
        sqlite3_finalize(stmt);
        stmt = nullptr;

        responseCode = sqlite3_prepare_v2(db1, insertUserQuery, -1, &stmt, nullptr);
        if (responseCode != SQLITE_OK) {
            Serial.printf("[SQLITE3] Prepare failed: %s\n", sqlite3_errmsg(db1));
            goto cleanup;
        }
        sqlite3_bind_int(stmt, 1, id);
        sqlite3_bind_text(stmt, 2, usuario.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, tag.c_str(), -1, SQLITE_TRANSIENT);
        responseCode = sqlite3_step(stmt);
        if (responseCode != SQLITE_DONE) {
            Serial.printf("[SQLITE3] Insert failed: %s\n", sqlite3_errmsg(db1));
            goto cleanup;
        }
    }

    cleanup:
        if (stmt) {sqlite3_finalize(stmt); stmt = nullptr;}
        if (db1)  {sqlite3_close(db1); db1 = nullptr;}
        return tagExists;
}


void removeUser(int idDelete){
    sqlite3_stmt *stmt = nullptr;
    db_open("/spiffs/users.db", &db1);
    

    responseCode = sqlite3_prepare_v2(db1, deleteUserQuery, -1, &stmt, nullptr);
    if (responseCode != SQLITE_OK) {
        Serial.printf("[SQLITE3] Prepare failed: %s\n", sqlite3_errmsg(db1));
        goto cleanup;
    }
    sqlite3_bind_int(stmt, 1, idDelete);
    responseCode = sqlite3_step(stmt);
    if (responseCode != SQLITE_DONE) {
        Serial.printf("[SQLITE3] Step failed: %s\n", sqlite3_errmsg(db1));
    }

    cleanup:
        if (stmt) {sqlite3_finalize(stmt); stmt = nullptr;}
        if (db1)  {sqlite3_close(db1); db1 = nullptr;}
}


void clearDB(){
    sqlite3_stmt *stmt = nullptr;
    db_open("/spiffs/users.db", &db1);
    

    responseCode = sqlite3_prepare_v2(db1, clearAccessQuery, -1, &stmt, nullptr);
    if (responseCode != SQLITE_OK) {
        Serial.printf("[SQLITE3] Prepare failed: %s\n", sqlite3_errmsg(db1));
        goto cleanup;
    }
    responseCode = sqlite3_step(stmt);
    if (responseCode != SQLITE_DONE) {
        Serial.printf("[SQLITE3] Step failed: %s\n", sqlite3_errmsg(db1));
    }

    cleanup:
        if (stmt) {sqlite3_finalize(stmt); stmt = nullptr;}
        if (db1)  {sqlite3_close(db1); db1 = nullptr;}
}


void getDBAsync(uint32_t client) {
    xTaskCreatePinnedToCore(
        buildCSVTask,
        "CSVBuilder",
        8192,     // Stack size
        (void*)client,
        1,
        NULL,
        1         // Core 1
    );
}

void buildCSVTask(void* param) {
    uint32_t client = (uint32_t)param;

    // Start with UTF-8 BOM
    String csv = "\xEF\xBB\xBF";  // BOM
    csv += "Usuário,TAG,Data,Ação\n";

    sqlite3_stmt *stmt = nullptr;
    db_open("/spiffs/users.db", &db1);

    responseCode = sqlite3_prepare_v2(db1, getAccessDBQuery, -1, &stmt, nullptr);
    if (responseCode != SQLITE_OK) {
        Serial.printf("[SQLITE3] Prepare failed: %s\n", sqlite3_errmsg(db1));
        goto cleanup;
    }

    while ((responseCode = sqlite3_step(stmt)) == SQLITE_ROW) {
        int cols = sqlite3_column_count(stmt);
        for (int i = 0; i < cols; i++) {
            const unsigned char *text = sqlite3_column_text(stmt, i);
            String field = text ? String((const char *)text) : "";

            // Column 2 is "date" (name, tag, date, act) — reformat MM/DD/YY -> DD/MM/YYYY
            if (i == 2 && field.length() >= 17) {
                String mm = field.substring(0, 2);
                String dd = field.substring(3, 5);
                String yy = field.substring(6, 8);
                String time = field.substring(9);
                field = dd + "/" + mm + "/20" + yy + " " + time;
            }

            csv += field;
            if (i < cols - 1) csv += ',';
        }
        csv += '\n';
        yield(); // let other tasks run between rows
    }
    if (responseCode != SQLITE_DONE) {
        Serial.printf("[SQLITE3] Step failed: %s\n", sqlite3_errmsg(db1));
    }

    cleanup:
        if (stmt) {sqlite3_finalize(stmt); stmt = nullptr;}
        if (db1)  {sqlite3_close(db1); db1 = nullptr;}
        if (client) notifyUserData("csv", csv, "individual", client);
        vTaskDelete(NULL); 
}