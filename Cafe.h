#pragma once
#ifndef CAFE_H
#define CAFE_H

#include <string>
#include <cstdio>

using namespace std;

/* --------------- INGREDIENT --------------- */

const int MAX_INGREDIENT_NAME = 50;

class Ingredient {
private:
    string name;
    int quantity;

public:
    Ingredient();
    Ingredient(const string& n, int q);

    void setName(const string& n);
    void setQuantity(int q);

    string getName() const;
    int getQuantity() const;

    void consume(int q);
    void restock(int q);
};

/*--------------- MENU ITEM --------------- */

const int MAX_ITEM_NAME = 50;
const int MAX_ING_PER_ITEM = 10;

class MenuItem {
private:
    string id;
    string name;
    double price;
    bool available;

    string ingNames[MAX_ING_PER_ITEM];
    int ingQty[MAX_ING_PER_ITEM];
    int ingCount;

public:
    MenuItem();

    void setId(const string& i);
    void setName(const string& n);
    void setPrice(double p);
    void setAvailable(bool a);

    string getId() const;
    string getName() const;
    double getPrice() const;
    bool isAvailable() const;

    void clearIngredients();
    bool addIngredientRequirement(const string& ingName, int qty);

    int getIngCount() const;
    string getIngNameAt(int idx) const;
    int getIngQtyAt(int idx) const;
};

/* --------------- ORDER --------------- */

const int MAX_ORDER_ITEMS = 50;

class Order {
private:
    string orderId;
    MenuItem items[MAX_ORDER_ITEMS];
    int qty[MAX_ORDER_ITEMS];
    int itemCount;
    bool checkedOut;

public:
    Order();

    void setOrderId(const string& id);
    string getOrderId() const;

    bool addItem(const MenuItem& m, int q);
    bool removeItem(const string& menuItemId, int q);

    double computeTotal() const;
    void checkout();
    bool isCheckedOut() const;

    int getItemCount() const;
    const MenuItem& getItemAt(int idx) const;
    int getQtyAt(int idx) const;
};

/* --------------- INVENTORY --------------- */

class InventoryManager {
private:
    static const int MAX_INGREDIENTS = 100;

    Ingredient ingredients[MAX_INGREDIENTS];
    int ingredientCount;

    int findIngredientIndex(const string& name) const;

public:
    InventoryManager();

    bool addIngredient(const string& name, int qty);
    bool restock(const string& name, int qty);

    bool checkAvailabilityForItem(const MenuItem& item, int multiplier) const;
    bool deductForItem(const MenuItem& item, int multiplier);
    bool deductForMultiple(const MenuItem menuItems[], const int qty[], int count);

    void listAllStock() const;
    void lowStockAlerts(int threshold) const;
};

/* --------------- USER --------------- */

class User {
protected:
    string username;
    string password;

public:
    User();
    User(const string& u, const string& p);
    virtual ~User();

    virtual bool login(const string& u, const string& p);
    virtual void logout();
    virtual void changePassword(const string& newPass);

    string getUsername() const;
};

/* --------------- STAFF --------------- */

class Staff : public User {
private:
    InventoryManager* inventoryRef;

public:
    Staff();

    void attachInventory(InventoryManager* inv);
    Order createOrder();
    bool addItemToOrder(Order& order, MenuItem& item, int qty);
    void updateOrderStatus(Order& order, const string& status);
};

/* --------------- REPORT --------------- */

class Report {
public:
    static double dailySalesReport(const Order orders[], int count);
    static void bestSellingItems(const Order orders[], int orderCount);
    static void inventoryAlerts(const InventoryManager& inv, int threshold);
};

/* --------------- FEEDBACK --------------- */

const int MAX_FEEDBACK = 100;

struct FeedbackEntry {
    string id;
    string customerName;
    string comment;
    int rating;
};

class FeedbackManager {
private:
    FeedbackEntry entries[MAX_FEEDBACK];
    int count;

    string makeId() const;

public:
    FeedbackManager();

    bool submitFeedback(const string& customerName, const string& comment, int rating);
    void loadFromFile();
    void viewAllFeedback();
    void filterByRating(int minRating) const;
};

/* --------------- CUSTOMER --------------- */

class Customer : public User {
public:
    Customer();
    Customer(const string& u, const string& p);

    bool placeOrder(Order& order);
    bool giveFeedback(FeedbackManager& fm, const string& comment, int rating);
    };

/* --------------- BILLING --------------- */

class Billing {
public:
    static double applyDiscounts(const Order& order, double discountPercent);
    static void generateReceipt(const Order& order, double finalAmount, const string& customerName);
};

/* --------------- ADMIN --------------- */

const int MAX_STAFF = 50;

class Admin : public User {
private:
    string staffUsernames[MAX_STAFF];
    int staffCount;

    MenuItem* menuRef;
    int* menuCountRef;
    InventoryManager* inventoryRef;
    FeedbackManager* feedbackRef;

public:
    Admin();

    void attachMenu(MenuItem* mRef, int* mCountRef);
    void attachInventory(InventoryManager* inv);
    void attachFeedback(FeedbackManager* fdb);

    bool addStaff(const string& uname);
    bool removeStaff(const string& uname);

    bool updateMenuItemPrice(const string& menuId, double newPrice);
    bool changeMenuAvailability(const string& menuId, bool avail);

    bool restockInventory(const string& ingName, int qty);
    void viewAllFeedback();
};

#endif