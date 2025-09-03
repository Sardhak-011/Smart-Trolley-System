#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define SS_PIN 10
#define RST_PIN 9
MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

LiquidCrystal_I2C lcd(0x27, 16, 2);
#define BUZZER_PIN 2
const int BLOCK = 4;

// Known UIDs
byte knownUIDs[9][4] = {
  {0x47, 0x14, 0x09, 0x05},  // User-A
  {0x3C, 0xCC, 0x00, 0x04},  // User-B
  {0xE4, 0xE0, 0x00, 0x04},  // Product 1
  {0x34, 0xD1, 0x01, 0x04},  // Product 2
  {0xCD, 0xF2, 0x01, 0x04},  // Product 3
  {0xE5, 0xF8, 0x01, 0x04},  // Product 4
  {0x99, 0x6F, 0xFE, 0x03},  // Product 5
  {0xE4, 0x3E, 0x09, 0x02},  // Total Tag
  {0x59, 0xA7, 0x8D, 0x54}   // Admin Tag (example UID)
};

String productNames[7] = {"Flour", "Oil", "Detergent", "Salt", "Chocolate"};
int productPrices[7] = {300, 150, 120, 30, 50};
int dynamicQuantities[7] = {0};
int userCredits[2] = {30, 50};
bool userEnabled[2] = {true, true};
bool secureMode = false;

int authenticatedUser = -1;

// Button Pins
#define BTN_FUNC1 3
#define BTN_FUNC2 4
#define BTN_FUNC3 5
#define BTN_FUNC4 6
#define BTN_FUNC5 7
#define BTN_CONFIRM A3
#define BTN_CANCEL  A0
#define BTN_INC     A1
#define BTN_DEC     A2

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  lcd.init();
  lcd.backlight();

  pinMode(BUZZER_PIN, OUTPUT);

  int buttons[] = {BTN_FUNC1, BTN_FUNC2, BTN_FUNC3, BTN_FUNC4, BTN_FUNC5, BTN_CONFIRM, BTN_CANCEL, BTN_INC, BTN_DEC};
  for (int i = 0; i < 9; i++) pinMode(buttons[i], INPUT_PULLUP);

  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  lcd.setCursor(0, 0);
  lcd.print("Scan User ID:");
}

void beepBuzzer(int duration = 300) {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(duration);
  digitalWrite(BUZZER_PIN, LOW);
}

void loop() {
  if (secureMode) {
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    int tagIndex = getTagIndex(rfid.uid.uidByte);
    if (tagIndex == 8) {
      beepBuzzer();
      handleAdminMode();
    } else {
      lcd.clear();
      lcd.print("System Locked");
      lcd.setCursor(0, 1);
      lcd.print("Maintenance On");
      delay(2000);
    }
    haltTag();
  } else {
    lcd.clear();
    lcd.print("System Locked");
    lcd.setCursor(0, 1);
    lcd.print("Maintenance On");
    delay(2000);
  }
  return;
}

  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) return;

  int tagIndex = getTagIndex(rfid.uid.uidByte);
  if (tagIndex == -1) {
    showMessage("Unknown Tag");
    haltTag();
    return;
  }

  beepBuzzer();

  if (tagIndex == 8) {
    handleAdminMode();
    haltTag();
    return;
  }

  if (authenticatedUser == -1) {
    if ((tagIndex == 0 || tagIndex == 1) && userEnabled[tagIndex]) {
      authenticatedUser = tagIndex;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Authenticated ID");
      delay(1000);
      lcd.clear();
      lcd.print("Welcome User ");
      lcd.print(authenticatedUser + 1);
      lcd.setCursor(0, 1);
      lcd.print("Credits: ");
      lcd.print(userCredits[authenticatedUser]);
      delay(2000);
      lcd.clear();
      lcd.print("Scan Products...");
    } else {
      showMessage("User Disabled");
    }
    haltTag();
    return;
  }

  if (tagIndex >= 2 && tagIndex <= 6) {
    int prodIndex = tagIndex - 2;
    dynamicQuantities[prodIndex]++;
    int total = productPrices[prodIndex] * dynamicQuantities[prodIndex];

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(productNames[prodIndex] + " x" + dynamicQuantities[prodIndex]);
    lcd.setCursor(0, 1);
    lcd.print("Rs ");
    lcd.print(total);
    delay(1500);
    lcd.clear();
    lcd.print("Scan More/Total");
  } else if (tagIndex == 7) {
    displayBillAndCredits();
    resetAll();
  }

  haltTag();
}

void displayBillAndCredits() {
  int totalBeforeCredits = 0;
  for (int i = 0; i < 5; i++) {
    totalBeforeCredits += productPrices[i] * dynamicQuantities[i];
  }

  int Quantities = 0;
  lcd.clear();
  lcd.print("Bill Summary");
  delay(2000);

  for (int i = 0; i < 5; i++) {
    if (dynamicQuantities[i] > 0) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(productNames[i] + " x" + dynamicQuantities[i]);
      lcd.setCursor(0, 1);
      lcd.print("Rs ");
      lcd.print(productPrices[i] * dynamicQuantities[i]);
      delay(1500);
      Quantities += dynamicQuantities[i];
    }
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Total: Rs ");
  lcd.print(totalBeforeCredits);
  lcd.setCursor(0, 1);
  lcd.print("Qty: ");
  lcd.print(Quantities);
  delay(2000);

  int finalAmount = totalBeforeCredits;
  int deduction = 0;

  if (totalBeforeCredits >= 300) {
    int maxDeduct = totalBeforeCredits / 2;
    int creditValue = userCredits[authenticatedUser] * 2;
    deduction = min(creditValue, maxDeduct);
    finalAmount -= deduction;
    userCredits[authenticatedUser] -= deduction / 2;
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Credits Discount: ");
  lcd.setCursor(0, 1);
  lcd.print("Rs ");
  lcd.print(deduction);
  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Pay: Rs ");
  lcd.print(finalAmount);
  delay(3000);

  int creditsEarned = 0;
  if (finalAmount >= 100) {
    creditsEarned = (finalAmount / 100) * 2;
    userCredits[authenticatedUser] += creditsEarned;
    lcd.clear();
    lcd.print("Credits Earned:");
    lcd.setCursor(0, 1);
    lcd.print(creditsEarned);
    delay(2000);
  } else {
    lcd.clear();
    lcd.print("No Credits Earned");
    lcd.setCursor(0, 1);
    lcd.print("Bill < Rs 100");
    delay(2000);
  }

  lcd.clear();
  lcd.print("Remaining Credits");
  lcd.setCursor(0, 1);
  lcd.print(userCredits[authenticatedUser]);
  delay(2500);
  lcd.clear();
  lcd.print("Thank You!!!");
  lcd.setCursor(0, 1);
  lcd.print("Keep Shopping.");
  delay(2500);
}

void resetAll() {
  for (int i = 0; i < 7; i++) dynamicQuantities[i] = 0;
  authenticatedUser = -1;
  lcd.clear();
  lcd.print("Scan User Card");
}

void showMessage(String msg) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(msg);
  delay(1500);
  lcd.clear();
  lcd.print("Scan User Card");
}

int getTagIndex(byte *uid) {
  for (int i = 0; i < 9; i++) {
    if (memcmp(uid, knownUIDs[i], 4) == 0) return i;
  }
  return -1;
}

void haltTag() {
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

// ADMIN MODE FUNCTIONS

void handleAdminMode() {
  int selectedFunc = -1;
  lcd.clear();
  lcd.print("Admin Mode");
  delay(1000);
  lcd.clear();

  // Display all admin functionalities with numbers
  lcd.setCursor(0, 0);
  lcd.print("1: Modify Price");
  delay(1000);
  lcd.setCursor(0, 1);
  lcd.print("2: Clear Quantities");
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("3: Toggle User");
  delay(1000);
  lcd.setCursor(0, 1);
  lcd.print("4: Toggle Secure");
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("5: Modify Credits");
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Choose Option:");

  while (selectedFunc == -1) {
    if (digitalRead(BTN_FUNC1) == LOW) selectedFunc = 1;
    else if (digitalRead(BTN_FUNC2) == LOW) selectedFunc = 2;
    else if (digitalRead(BTN_FUNC3) == LOW) selectedFunc = 3;
    else if (digitalRead(BTN_FUNC4) == LOW) selectedFunc = 4;
    else if (digitalRead(BTN_FUNC5) == LOW) selectedFunc = 5;
  }
  delay(500);

  switch (selectedFunc) {
    case 1: adminModifyProductPrice(); break;
    case 2: adminClearQuantities(); break;
    case 3: adminToggleUser(); break;
    case 4: adminToggleSecureMode(); break;
    case 5: adminModifyUserCredits(); break;
  }
  resetAll();  // Go back to user authentication after admin mode execution or cancel
}

void adminModifyProductPrice() {
  lcd.clear();
  lcd.print("Scan Product Tag");
  while (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial());
  int tagIndex = getTagIndex(rfid.uid.uidByte);
  haltTag();

  if (tagIndex < 2 || tagIndex > 6) {
    lcd.clear();
    lcd.print("Invalid Product");
    delay(2000);
    return;
  }

  int prodIndex = tagIndex - 2;
  int newPrice = productPrices[prodIndex];

  while (true) {
    lcd.clear();
    lcd.print(productNames[prodIndex]);
    lcd.setCursor(0, 1);
    lcd.print("Price: Rs ");
    lcd.print(newPrice);
    delay(300);

    if (digitalRead(BTN_INC) == LOW) newPrice += 5;
    if (digitalRead(BTN_DEC) == LOW && newPrice > 5) newPrice -= 5;

    if (digitalRead(BTN_CONFIRM) == LOW) {
      productPrices[prodIndex] = newPrice;
      lcd.clear();
      lcd.print("Price Updated");
      delay(2000);
      return;
    }

    if (digitalRead(BTN_CANCEL) == LOW) {
      lcd.clear();
      lcd.print("Cancelled");
      delay(2000);
      return;
    }
  }
}

void adminClearQuantities() {
  lcd.clear();
  lcd.print("Confirm Clear?");
  delay(1000);
  while (true) {
    if (digitalRead(BTN_CONFIRM) == LOW) {
      for (int i = 0; i < 7; i++) dynamicQuantities[i] = 0;
      lcd.clear();
      lcd.print("All Cleared");
      delay(2000);
      return;
    }
    if (digitalRead(BTN_CANCEL) == LOW) {
      lcd.clear();
      lcd.print("Cancelled");
      delay(2000);
      return;
    }
  }
}

void adminToggleUser() {
  lcd.clear();
  lcd.print("Scan User Tag");
  while (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial());
  int tagIndex = getTagIndex(rfid.uid.uidByte);
  haltTag();

  if (tagIndex != 0 && tagIndex != 1) {
    lcd.clear();
    lcd.print("Invalid User");
    delay(2000);
    return;
  }

  int uid = tagIndex;
  lcd.clear();
  lcd.print(userEnabled[uid] ? "Disable User?" : "Enable User?");
  delay(1000);

  while (true) {
    if (digitalRead(BTN_CONFIRM) == LOW) {
      userEnabled[uid] = !userEnabled[uid];
      lcd.clear();
      lcd.print(userEnabled[uid] ? "User Enabled" : "User Disabled");
      delay(2000);
      return;
    }
    if (digitalRead(BTN_CANCEL) == LOW) {
      lcd.clear();
      lcd.print("Cancelled");
      delay(2000);
      return;
    }
  }
}

void adminToggleSecureMode() {
  lcd.clear();
  lcd.print(secureMode ? "Disable Maint?" : "Enable Maint?");
  delay(1000);
  while (true) {
    if (digitalRead(BTN_CONFIRM) == LOW) {
      secureMode = !secureMode;
      lcd.clear();
      lcd.print(secureMode ? "Maint Mode ON" : "Maint Mode OFF");
      delay(2000);
      return;
    }
    if (digitalRead(BTN_CANCEL) == LOW) {
      lcd.clear();
      lcd.print("Cancelled");
      delay(2000);
      return;
    }
  }
}

void adminModifyUserCredits() {
  lcd.clear();
  lcd.print("Scan User Tag");
  while (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial());
  int tagIndex = getTagIndex(rfid.uid.uidByte);
  haltTag();

  if (tagIndex != 0 && tagIndex != 1) {
    lcd.clear();
    lcd.print("Invalid User");
    delay(2000);
    return;
  }

  int uid = tagIndex;
  int newCredits = userCredits[uid];

  while (true) {
    lcd.clear();
    lcd.print("User ");
    lcd.print(uid + 1);
    lcd.setCursor(0, 1);
    lcd.print("Credits: ");
    lcd.print(newCredits);
    delay(300);

    if (digitalRead(BTN_INC) == LOW) newCredits += 5;
    if (digitalRead(BTN_DEC) == LOW && newCredits >= 5) newCredits -= 5;

    if (digitalRead(BTN_CONFIRM) == LOW) {
      userCredits[uid] = newCredits;
      lcd.clear();
      lcd.print("Credits Updated");
      delay(2000);
      return;
    }

    if (digitalRead(BTN_CANCEL) == LOW) {
      lcd.clear();
      lcd.print("Cancelled");
      delay(2000);
      return;
    }
  }
}