#include <Keypad.h>
#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 4);

#define SS_PIN 10  // RFID Reader SDA pin
#define RST_PIN 9   // RFID Reader RST pin
#define valvePin 8  // Pin for the water valve

// Initialize RFID reader
MFRC522 rfid(SS_PIN, RST_PIN);

// Define Keypad
const byte ROWS = 4;  // 4 rows for the numeric keypad
const byte COLS = 3;  // 3 columns for the numeric keypad
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[ROWS] = {32, 33, 34, 35};  // Row pins for the keypad
byte colPins[COLS] = {40, 41, 42};      // Column pins for the keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Define multiple authorized UIDs with associated names
struct AuthorizedCard {
  byte uid[4];
  const char* name;
};

AuthorizedCard authorizedCards[] = {
  {{0xB3, 0x46, 0xD8, 0xA9}, "Francis Masanja"},
  {{0x63, 0xE9, 0x7C, 0xA9}, "Emanuel Ricky"},
  {{0x03, 0xDD, 0xB3, 0xA9}, "Brac'ha Marcos"},
  {{0x23, 0x08, 0x35, 0xAA}, "Arthur Daniel"},
  {{0x33, 0xC5, 0x0E, 0xAA}, "Gracious Wilfred"},
  {{0x03, 0x85, 0x92, 0xA9}, "Lidya Samson"}
};
const int numCards = sizeof(authorizedCards) / sizeof(authorizedCards[0]);

// Variables
int balance = 1000;  // Default balance
unsigned long lastTime = 0;
const unsigned long dispenseInterval = 1000;  // 1 second = 1 unit of balance
bool isAuthorized = false;  // Variable to track authorization state

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();

  pinMode(valvePin, OUTPUT);
  digitalWrite(valvePin, LOW);  // Valve initially off

  lcd.init();
  lcd.clear();
  lcd.backlight();

  // Load balance from EEPROM
  balance = EEPROM.read(0);  // Read the balance stored in EEPROM

  Serial.println("System ready. Please scan authorized card to activate keypad...");
  lcd.setCursor(0, 0);
  lcd.print("Please scan card");
  lcd.setCursor(0, 1);
  lcd.print("to activate keypad...");
}

void loop() {
  // Card reading
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    handleCard();
    rfid.PICC_HaltA();
  }

  // Keypad input for dispensing water (only if authorized)
  if (isAuthorized) {
    char key = keypad.getKey();
    if (key) {
      Serial.print("Key Pressed: ");
      Serial.println(key);

      if (balance > 0) {
        if (key >= '0' && key <= '9') {
          int dispenseTime = (key - '0') * 1000;  // Convert keypress to dispensing time in ms (1-9 seconds)
          dispenseWater(dispenseTime);
        } else if (key == '#') {
          Serial.println("Water dispensing canceled.");
          display("Water dispensing canceled.");
          digitalWrite(valvePin, LOW);
          delay(2000);
          lcd.setCursor(0, 0);
          lcd.print("Please scan card");
          lcd.setCursor(0, 1);
          lcd.print("to activate keypad...");
        }
      } else {
        Serial.println("Insufficient balance.");
        display("insufficient balance.");
        lcd.setCursor(0, 0);
        lcd.print("Please scan card");
        lcd.setCursor(0, 1);
        lcd.print("to activate keypad...");
      }
    }
  }
}

// Handle the RFID card
void handleCard() {
  Serial.print("Card UID: ");
  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
    Serial.print(rfid.uid.uidByte[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  // Check if the card UID matches any authorized cards
  bool found = false;
  for (int i = 0; i < numCards; i++) {
    if (compareUID(rfid.uid.uidByte, rfid.uid.size, authorizedCards[i].uid)) {
      Serial.print("Access granted to: ");
      Serial.println(authorizedCards[i].name);
      key("Welcome", 0);
      lcd.setCursor(0, 1);
      lcd.print(authorizedCards[i].name);
      lcd.setCursor(0, 3);
      lcd.print("Balance: ");
      lcd.print(balance);
      isAuthorized = true;
      found = true;
      break;
    }
  }

  if (!found) {
    Serial.println("Access denied: Unauthorized card");
    key("Access denied", 1);
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Please scan card");
    lcd.setCursor(0, 1);
    lcd.print("to activate keypad...");
    isAuthorized = false;
  }
}

// Dispense water for a given duration
void dispenseWater(int duration) {
  Serial.print("Dispensing water for ");
  lcd.setCursor(0, 0);
  lcd.print("Dispensing water ...");
  Serial.print(duration / 1000);
  Serial.println(" seconds.");

  digitalWrite(valvePin, HIGH);  // Open the valve
  delay(duration);  // Dispense for the given time
  digitalWrite(valvePin, LOW);   // Close the valve

  // Update the balance
  int balanceDeduction = duration / dispenseInterval;
  balance -= balanceDeduction;
  Serial.print("Remaining Balance: ");
  Serial.println(balance);
  key("Remaining Balance: ", 2);
  lcd.setCursor(0, 3);
  lcd.print(balance);

  // Save updated balance to EEPROM
  EEPROM.write(0, balance);  // Write the updated balance to EEPROM
}

// Check if the UID matches any authorized card
bool isAuthorizedCard(byte *uid, byte uidSize) {
  for (int i = 0; i < numCards; i++) {
    if (compareUID(uid, uidSize, authorizedCards[i].uid)) {
      return true;
    }
  }
  return false;
}

// Compare the RFID card UID with an authorized UID
bool compareUID(byte *uid, byte uidSize, byte *authorizedUID) {
  for (byte i = 0; i < uidSize; i++) {
    if (uid[i] != authorizedUID[i]) {
      return false;  // UIDs don't match
    }
  }
  return true;  // UIDs match
}

void display(String value) {
  lcd.clear();
  lcd.setCursor(4, 1);
  lcd.print(value);
}

void key(String capt, int post) {
  lcd.clear();
  lcd.setCursor(2, post);
  lcd.print(capt);
}
