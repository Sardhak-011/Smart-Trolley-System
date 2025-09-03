# Smart Trolley System 🛒

An Arduino-based **Smart Trolley System** that automates billing in supermarkets using **RFID technology**. The system is equipped with **LCD display, buzzer, EEPROM memory, and admin controls** for product management and user authentication.

---

## 🔹 Features
- RFID-based product detection and automatic billing  
- User authentication using RFID tags with credit points  
- LCD auto-scroll for product details, total price, and status messages  
- Quantity tracking for multiple products  
- Buzzer feedback for scan confirmation  
- Admin functionalities:
  - Modify product prices
  - Clear all product quantities
  - Enable/disable users
  - Toggle secure maintenance mode
  - Adjust user credits  
- EEPROM storage for data persistence even after reset/power cycle  

---

## 🔹 Hardware Used
- Arduino Nano  
- RFID Reader & Tags (User + Product + Admin tags)  
- 16x2 LCD Display (I2C)  
- Buzzer  
- Push Buttons (for admin controls)  
- EEPROM (internal on Arduino)  
- Resistors, Breadboard, Wires  

---

## 🔹 Software & Tools
- Arduino IDE  
- C/C++ (Embedded programming)  
- Libraries: `Wire.h`, `LiquidCrystal_I2C.h`, `EEPROM.h`, `SPI.h`, `MFRC522.h`  

---

## 🔹 How It Works
1. User scans their RFID tag to log in.  
2. Products are added/removed by scanning RFID product tags.  
3. LCD displays product name, quantity, and total bill with auto-scroll.  
4. Admin can scan their tag to modify prices, adjust credits, or enable/disable users.  
5. Final bill is calculated considering user’s credit points.  

---

## 🔹 Future Improvements
- Integration with mobile app for real-time bill tracking  
- Wireless payment options  
- Larger LCD/TFT display for better UI  

---
