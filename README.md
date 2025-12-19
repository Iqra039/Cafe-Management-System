# ☕ Cafe Management System - SFML GUI

Modern cafe management application with graphical interface built using C++ and SFML.

## 🎯 Features

### **Admin Panel**
- Staff management (add/remove)
- Menu price & availability updates
- Inventory restocking
- View customer feedback

### **Staff Panel**
- Create and process orders
- Add menu items with quantities
- Generate receipts automatically

### **Customer Panel**
- Browse menu items
- Place orders
- Submit feedback with ratings

### **Additional Features**
- Inventory tracking with low stock alerts
- Sales reporting
- File-based data persistence
- Cafe-themed color scheme

## 🚀 Quick Start

### **Requirements**
- C++ compiler (Visual Studio)
- SFML 2.5+

### **Installation**
```bash
# Clone repository
git clone https://github.com/yourusername/cafe-management-system.git
cd cafe-management-system

# Compile (Linux/macOS)
g++ -o cafe_app main.cpp Cafe.cpp -lsfml-graphics -lsfml-window -lsfml-system

# Run
./cafe_app
```

### **Windows**
1. Install SFML from [sfml-dev.org](https://www.sfml-dev.org)
2. Link SFML in your IDE (Visual Studio, Code::Blocks, etc.)
3. Compile and run

## 🔑 Login Credentials

- **Admin Password**: `deathnote`
- **Staff**: Add via Admin panel first
- **Customer**: No login required

## 🎨 Color Theme
- **Primary**: Khaki Brown `(205, 179, 139)`
- **Secondary**: Espresso Brown `(76, 47, 39)`
- **Background**: Cream White `(250, 245, 240)`

## 📁 Data Files
- `staff.txt` - Staff usernames
- `feedback.txt` - Customer feedback
- `orders.txt` - Order history  
- `ingredient.txt` - Inventory
- `receipt_*.txt` - Generated receipts

## 🛠️ Customization
Edit `main.cpp` to:
- Add/update menu items
- Modify color scheme
- Change default settings

## 📄 License
MIT License

---

**Perfect for small cafes, coffee shops, or educational purposes!**
