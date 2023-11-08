#define RFID_SDA 5
#define RFID_SCK 18
#define RFID_MOSI 23
#define RFID_MISO 19
#define RFID_RST 22

unsigned long UID[3];
String id = "";

MFRC522 mfrc522(RFID_SDA, RFID_RST);  // Create MFRC522 instance

void setupRFID(){
    SPI.begin(RFID_SCK, RFID_MISO, RFID_MOSI);
    mfrc522.PCD_Init();		// Init MFRC522
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

void checkCard(){
    id = "";
    // Look for new cards
	if ( ! mfrc522.PICC_IsNewCardPresent()) {
        digitalWrite(led, LOW);
		return;
	}

	// Select one of the cards
	if ( ! mfrc522.PICC_ReadCardSerial()) {
		return;
	}

    for (byte i = 0; i < 4; i++) {
        UID[i] = mfrc522.uid.uidByte[i];
    }
    printHex(mfrc522.uid.uidByte, mfrc522.uid.size);

    Serial.println("TAG lida:" + id);
    digitalWrite(led, HIGH);
    dbCheck(id);

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
}

String newCard(){
    id = "";
    if ( ! mfrc522.PICC_IsNewCardPresent()) {
        digitalWrite(led, LOW);
        return "";
	}
    else{

	// Select one of the cards
	if ( ! mfrc522.PICC_ReadCardSerial()) {
		return "";
	}

    for (byte i = 0; i < 4; i++) {
        UID[i] = mfrc522.uid.uidByte[i];
    }
    printHex(mfrc522.uid.uidByte, mfrc522.uid.size);

    Serial.println("TAG lida:" + id);
    digitalWrite(led, HIGH);
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();

    return id;
    }
}