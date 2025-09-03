#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

const int BLOCK = 4;

// Store UIDs of 8 tags (4 bytes each)
byte knownUIDs[8][4] = {
  {0x47, 0x14, 0x09, 0x05},  // Tag 1
  {0x3C, 0xCC, 0x00, 0x04},  // Tag 2
  {0xE4, 0xE0, 0x00, 0x04},  // Tag 3
  {0x34, 0xD1, 0x01, 0x04},  // Tag 4
  {0xCD, 0xF2, 0x01, 0x04},  // Tag 5
  {0xE5, 0xF8, 0x01, 0x04},  // Tag 6
  {0x99, 0x6F, 0xFE, 0x03},  // Tag 7
  {0xE4, 0x3E, 0x09, 0x02}   // Tag 8 - total tag
};

// Product data for first 7 tags
const char* products[7] = {
  "Chips-25", "Juice-50", "Biscuit-10",
  "Soda-40", "Noodles-30", "Candy-5", "Choco-60"
};

int prices[7] = {25, 50, 10, 40, 30, 5, 60};

int scannedFlags[7] = {0,0,0,0,0,0,0};
int totalPrice = 0;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();

  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  Serial.println("Scan a product tag or total tag...");
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) return;

  int index = getTagIndex(rfid.uid.uidByte);

  if (index == -1) {
    Serial.println("Unknown tag.");
    haltTag(); return;
  }

  if (index < 7) {
    if (!scannedFlags[index]) {
      // Write product data
      byte dataBlock[16] = {0};
      strncpy((char*)dataBlock, products[index], 16);

      if (writeToTag(dataBlock)) {
        Serial.print("Written: "); Serial.println(products[index]);
        totalPrice += prices[index];
        scannedFlags[index] = 1;
      }
    } else {
      Serial.println("Tag already scanned.");
    }
  } else if (index == 7) {
    // Write total price to tag 8
    char totalStr[16];
    sprintf(totalStr, "Total-%d", totalPrice);
    byte dataBlock[16] = {0};
    strncpy((char*)dataBlock, totalStr, 16);

    if (writeToTag(dataBlock)) {
      Serial.print("Total written: ");
      Serial.println(totalStr);
    }
  }

  haltTag();
  delay(1000);
}

int getTagIndex(byte *uid) {
  for (int i = 0; i < 8; i++) {
    if (memcmp(uid, knownUIDs[i], 4) == 0) return i;
  }
  return -1;
}

bool writeToTag(byte *data) {
  MFRC522::StatusCode status;

  status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, BLOCK, &key, &(rfid.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Auth failed: ");
    Serial.println(rfid.GetStatusCodeName(status));
    return false;
  }

  status = rfid.MIFARE_Write(BLOCK, data, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Write failed: ");
    Serial.println(rfid.GetStatusCodeName(status));
    return false;
  }

  return true;
}

void haltTag() {
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}