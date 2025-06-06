#include "rfidSetup.h"

String id = "";
unsigned long UID[3] = {0};
unsigned long lastInterruptTime = 0;
MFRC522 mfrc522(RFID_SDA, RFID_RST);  // Create MFRC522 instance

int newUserTag = LOW;
int readUser = LOW;
String newUser = "";
String user = "";

void setupRFID(){
    SPI.begin(RFID_SCK, RFID_MISO, RFID_MOSI);
    mfrc522.PCD_Init();		// Init MFRC522

    // Enable IRQ for RFID
    // mfrc522.PCD_WriteRegister(MFRC522::ComIEnReg, 0xA0);
    // mfrc522.PCD_WriteRegister(MFRC522::ComIrqReg, 0x00);
    // mfrc522.PCD_WriteRegister(MFRC522::DivIEnReg, 0x00);

    // pinMode(RFID_IRQ, INPUT_PULLUP); // Set IRQ pin as input with pull-up resistor
    // attachInterrupt(digitalPinToInterrupt(RFID_IRQ), handleIRQ, FALLING);
    // esp_sleep_enable_ext0_wakeup(GPIO_NUM_2, 1);

    mfrc522.PCD_DumpVersionToSerial();	// Show details of PCD - MFRC522 Card Reader details
    Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));

}

void printHex(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        id.concat(String(buffer[i] < 0x10 ? " 0" : ""));
        id.concat(String(buffer[i], HEX));
        id.toUpperCase();
    }
}

bool checkForCard(){
    if (!mfrc522.PICC_IsNewCardPresent()) return false;
    if (!mfrc522.PICC_ReadCardSerial()) return false;

    Serial.println("[RFID] Card detected.");
    // Process UID if needed
    return true;
}

void checkCard(){
    id = "";
    // Look for new cards
	if ( ! mfrc522.PICC_IsNewCardPresent()) {
        digitalWrite(led, LOW);
        digitalWrite(relay1, LOW);
        digitalWrite(relay2, LOW);
		return;
	}

    digitalWrite(led, HIGH);
	// Select one of the cards
	if ( ! mfrc522.PICC_ReadCardSerial()) {
		return;
	}

    for (byte i = 0; i < 4; i++) {
        UID[i] = mfrc522.uid.uidByte[i];
    }
    printHex(mfrc522.uid.uidByte, mfrc522.uid.size);


    dbCheck(id);

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
}

String newCard(){
    id = "";
    if ( ! mfrc522.PICC_IsNewCardPresent()) {
        digitalWrite(led, LOW);
        digitalWrite(relay1, LOW);
        digitalWrite(relay2, LOW);
        return "";
	}
    else{

	// Select one of the cards
	if ( ! mfrc522.PICC_ReadCardSerial()) {
		return "";
	}

    digitalWrite(led, HIGH);
    for (byte i = 0; i < 4; i++) {
        UID[i] = mfrc522.uid.uidByte[i];
    }
    printHex(mfrc522.uid.uidByte, mfrc522.uid.size);

    dbCheck(id);

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();

    Serial.println("[RFID] New card detected. ID: " + id);
    return id;
    }
}

void updateStatus( String message ){
    newUserTag = !newUserTag;
}

String access(){
    String tag = newCard();
    if (tag != ""){
        return dbAccessCheck(tag);
    }
    else {
        return "";
    }
}