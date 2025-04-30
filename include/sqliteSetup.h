#ifndef SQLITESETUP_H
#define SQLITESETUP_H

#include <Arduino.h>
#include <sqlite3.h>
#include <SPIFFS.h>
#include "heltecSetup.h"
#include "rfidSetup.h"


// ----------------------------------------------------------------------------
// Global variables
extern const char* data;
extern char *zErrMsg;
extern String message;
extern sqlite3 *db1;
extern int rc;


// ----------------------------------------------------------------------------
// Function prototypes
void removeLastChar();
static int callback(void *data, int argc, char **argv, char **azColName);
int db_open(const char *filename, sqlite3 **db);
int db_exec(sqlite3 *db, const char *sql);
String dbAccessCheck(String tag);
String getData(String numPage, String type);
void removeUser(int idDelete);
void dbCheck(String id);
int checkTag(String id);
void addUser(String usuario, String tag);
void clearDB();
String getDB();
void beginDB();


#endif