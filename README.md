🎯 Project Title: Smart Trolley Billing System using RFID

An Arduino-based **Smart Trolley System** that automates billing in supermarkets using **RFID technology**. The system is equipped with **LCD display, buzzer, EEPROM memory, and admin controls** for product management and user authentication.

---

**🛠 Core Features**

**1. RFID-Based Product Scanning**
Each product has an RFID tag (5 total).
When scanned, the system fetches:
Product Name
Unit Price
Quantity (auto-increments on repeated scans)
Subtotal for each product

**2. User Authentication**
Two users supported: User A and User B
Each has a unique RFID card
Authentication required before scanning products
Credits stored and tracked per user

**💸 Dynamic Billing System
3. Real-Time Billing**
Product subtotal and total quantity are updated live on LCD
After scanning the total tag:
Displays all items purchased
Total quantity
Total price (before and after credit deduction)

**4. Credit System**
Each user has initial credits:
User A: 30
User B: 50
Credits = discount: 1 credit = ₹2
Max discount = 50% of bill

**5. Credit Deduction Logic**
If total bill ≥ ₹300, user credits are used to reduce up to 50% of the bill
Credits used are deducted from the user's stored balance

**6. Credit Reward System**
For every ₹100 spent (post-discount), user earns 2 credits
Encourages larger purchases (gamified)

**📟 User Interface (LCD & Buzzer)
7. LCD Display Feedback**
Prompts shown on LCD:
"Scan User ID", "Scan Products"
Product name, price, and quantity
Final bill summary and earned credits

**8. Auto Scrolling Status**
LCD updates in real-time to reflect scanned products and prices
Friendly messages: "Welcome", "Thank you", "Keep Shopping"

**9. Buzzer Feedback**
A short beep (on pin D2) confirms every successful scan

**🧠 Data Handling & Control Logic
10. Tag Index Mapping**
Identifies 8 RFID UIDs:
2 for users
1 for Admin tag
5 for products
1 for the total calculation tag
Custom logic handles each tag accordingly

**11. Robust Flow Control**
Prevents operation until a user is authenticated
Blocks unknown/unregistered tags
Automatically resets for the next user after billing

**🔐 Security & Expansion Possibilities
12. Future-Ready Design**
Easy to add:
More products
More users

**13. EEPROM storage for data persistence even after reset/power cycle**

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
