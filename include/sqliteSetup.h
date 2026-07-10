#ifndef SQLITESETUP_H
#define SQLITESETUP_H

#include <Arduino.h>
#include <sqlite3.h>
#include <SPIFFS.h>
#include "heltecSetup.h"
#include "rfidSetup.h"
#include "main.h"
#include "sleepSetup.h"


// ----------------------------------------------------------------------------
// Global variables
extern sqlite3 *db1;
extern int responseCode;

// SQL queries
extern const char *getIdFromUsersQuery;

extern const char *getIdFromAccessQuery;
extern const char *getLastActFromAccessQuery;

extern const char *insertAccessQuery;

extern const char *getUserDataQuery;
extern const char *getUserDataLimQuery;
extern const char *getAccessDataQuery;
extern const char *getAccessDataLimQuery;

extern const char *deleteUserQuery;
extern const char *checkUserQuery;
extern const char *checkTagQuery;

extern const char *insertUserQuery;

extern const char *clearAccessQuery;

extern const char *getAcessDBQuery;


// ----------------------------------------------------------------------------
// Function prototypes
void beginDB();
int db_open(const char *filename, sqlite3 **db);
void postAccess();


String dbAccessCheck(String tag);
String getData(String numPage, String type);

bool addUser(String usuario, String tag);

void removeUser(int idDelete);

void clearDB();

void getDBAsync(uint32_t client);
void buildCSVTask(void* param);


#endif