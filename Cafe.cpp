#include<iostream>
#include<cstdlib>
#include<cstdio>
#include<iomanip>
#include<string>
#include<fstream>
#include<sstream>
#include"Cafe.h"
using namespace std;

/* --------------- User --------------- */
User::User() : username(""), password("") {}

User::User(const string& u, const string& p) : username(u), password(p) {}

bool User::login(const string& u, const string& p) {
    return (username == u && password == p);
}

void User::logout() {
    // placeholder
}

void User::changePassword(const string& newPass) {
    password = newPass;
}

string User::getUsername() const {
    return username;
}

User::~User() {
}

/* --------------- Admin --------------- */
Admin::Admin() : User(), staffCount(0),menuRef(nullptr),menuCountRef(nullptr),inventoryRef(nullptr),feedbackRef(nullptr) {
}

void Admin::attachMenu(MenuItem* mRef, int* mCountRef) {
    menuRef = mRef;
    menuCountRef = mCountRef;
}

void Admin::attachInventory(InventoryManager* inv) {
    inventoryRef = inv;
}

void Admin::attachFeedback(FeedbackManager* fdb) {
    feedbackRef = fdb;
}

bool Admin::addStaff(const string& uname) {
    if (staffCount >= MAX_STAFF) return false;

    staffUsernames[staffCount++] = uname;

    ofstream ofs("staff.txt", ios::app);
    if (ofs.is_open()) {
        ofs << uname << "\n";
        ofs.close();
    }
    return true;
}

bool Admin::removeStaff(const string& uname) {
    int found = -1;

    for (int i = 0; i < staffCount; ++i) {
        if (staffUsernames[i] == uname) {
            found = i;
            break;
        }
    }

    if (found == -1) return false;

    for (int j = found; j < staffCount - 1; ++j)
        staffUsernames[j] = staffUsernames[j + 1];

    staffCount--;

    ofstream ofs("staff.txt");
    for (int i = 0; i < staffCount; ++i)
        ofs << staffUsernames[i] << "\n";
    ofs.close();

    return true;
}


bool Admin::updateMenuItemPrice(const string& menuId, double newPrice) {
    if (!menuRef || !menuCountRef) return false;
    for (int i = 0; i < *menuCountRef; ++i) {
        if (menuRef[i].getId() == menuId) {
            menuRef[i].setPrice(newPrice);
            return true;
        }
    }
    return false;
}

bool Admin::changeMenuAvailability(const string& menuId, bool avail) {
    if (!menuRef || !menuCountRef) return false;
    for (int i = 0; i < *menuCountRef; ++i) {
        if (menuRef[i].getId() == menuId) {
            menuRef[i].setAvailable(avail);
            return true;
        }
    }
    return false;
}

bool Admin::restockInventory(const string& ingName, int qty) {
    if (!inventoryRef) return false;
    inventoryRef->restock(ingName, qty);
    return true;
}

void Admin::viewAllFeedback() {
    if (feedbackRef) feedbackRef->viewAllFeedback();
}


/* --------------- Billing --------------- */
double Billing::applyDiscounts(const Order& order, double discountPercent) {
    double total = order.computeTotal();
    if (discountPercent <= 0) return total;
    if (discountPercent >= 100) return 0.0;
    return total * (1.0 - discountPercent / 100.0);
}

void Billing::generateReceipt(const Order& order, double finalAmount, const string& customerName) {
    string filename = "receipt_" + order.getOrderId() + ".txt";
    ofstream ofs(filename.c_str());
    if (!ofs.is_open()) return;

    ofs << "------ Cafe Receipt ------\n";
    ofs << "Order ID: " << order.getOrderId() << "\n";


    ofs << "Customer: " << customerName << "\n\n";
    ofs << "Items:\n";

    for (int i = 0; i < order.getItemCount(); ++i) {
        ofs << order.getItemAt(i).getName() << " x " << order.getQtyAt(i)
            << " = " << (order.getItemAt(i).getPrice() * order.getQtyAt(i)) << "\n";
    }

    ofs << "\nTotal Paid: " << finalAmount << "\n";
    ofs << "--------------------------\n";

    ofs.close();
}


/* --------------- Customer --------------- */
Customer::Customer() : User() {}

Customer::Customer(const string& u, const string& p) : User(u, p) {}

bool Customer::placeOrder(Order& order) {
    if (order.getItemCount() == 0) return false;

    ofstream ofs("orders.txt", ios::app);
    if (ofs.is_open()) {
        ofs << order.getOrderId() << "|" << username << "|"<< order.computeTotal() << "\n";
        ofs.close();
    }

    order.checkout();
    return true;
}

bool Customer::giveFeedback(FeedbackManager& fm, const string& comment, int rating) {
    ofstream ofs("feedback.txt", ios::app);

    if (!ofs.is_open())
        return false;

    ofs << username << "|" << comment << "|" << rating << endl;
    ofs.close();

    return true;
}


/* --------------- Feedback Manager --------------- */
FeedbackManager::FeedbackManager() : count(0) {}

string FeedbackManager::makeId() const {
    stringstream ss;
    ss << "FB" << (count + 1);
    return ss.str();
}

bool FeedbackManager::submitFeedback(const string& customerName, const string& comment, int rating) {
    if (count >= MAX_FEEDBACK) return false;

    FeedbackEntry& e = entries[count++];
    e.id = makeId();
    e.customerName = customerName;
    e.comment = comment;
    e.rating = rating;

    ofstream ofs("feedbacks.txt", ios::app);
    if (ofs.is_open()) {
        ofs << e.id << "|" << e.customerName << "|"
            << e.rating << "|" << e.comment << "\n";
        ofs.close();
    }
    return true;
}

void FeedbackManager::loadFromFile() {
    count = 0;
    ifstream ifs("feedbacks.txt");
    if (!ifs.is_open()) return;

    string line;
    while (getline(ifs, line) && count < MAX_FEEDBACK) {
        if (line.empty()) continue;

        string id, name, ratingStr, comment;
        stringstream ss(line);

        getline(ss, id, '|');
        getline(ss, name, '|');
        getline(ss, ratingStr, '|');
        getline(ss, comment);

        FeedbackEntry& e = entries[count++];
        e.id = id;
        e.customerName = name;
        e.rating = stoi(ratingStr);
        e.comment = comment;
    }

    ifs.close();
}



    void FeedbackManager::viewAllFeedback() {
        ifstream ifs("feedback.txt");

        if (!ifs.is_open()) {
            cout << "\nNo feedback file found.\n";
            return;
        }

        cout << "\n========== CUSTOMER FEEDBACK ==========\n";

        string user, comment, ratingStr;
        bool hasFeedback = false;

        while (getline(ifs, user, '|')) {  // read up to first '|'
            if (!getline(ifs, comment, '|')) break; // read up to second '|'
            if (!getline(ifs, ratingStr)) break;    // read rest of line (rating)

            int rating = stoi(ratingStr);

            cout << "\nUser    : " << user;
            cout << "\nRating  : " << rating << "/5";
            cout << "\nComment : " << comment << endl;
            cout << "------------------------------------\n";

            hasFeedback = true;
        }

        if (!hasFeedback) {
            cout << "\nNo feedback entries found.\n";
        }

        ifs.close();
    }



void FeedbackManager::filterByRating(int minRating) const {
    cout << "---- Feedbacks Rating >= " << minRating << " ----\n\n";
    for (int i = 0; i < count; i++) {
        const FeedbackEntry& e = entries[i];

        if (e.rating >= minRating) {
            cout << "ID: " << e.id << "\n";
            cout << "Customer: " << e.customerName << "\n";
            cout << "Rating: " << e.rating << "\n";
            cout << "Comment: " << e.comment << "\n\n";
        }
    }
}


/* --------------- INGREDIENT --------------- */
Ingredient::Ingredient() : name(""), quantity(0) {}

Ingredient::Ingredient(const string& n, int q) : name(n), quantity(q) {}

void Ingredient::setName(const string& n) {
    name = n;
}

void Ingredient::setQuantity(int q) {
    quantity = q;
}

string Ingredient::getName() const {
    return name;
}

int Ingredient::getQuantity() const {
    return quantity;
}

void Ingredient::consume(int q) {
    if (q <= 0) return;
    if (q > quantity) quantity = 0;
    else quantity -= q;
}

void Ingredient::restock(int q) {
    if (q <= 0) return;
    quantity += q;

}
InventoryManager::InventoryManager() : ingredientCount(0) {}


int InventoryManager::findIngredientIndex(const string& name) const {
    for (int i = 0; i < ingredientCount; ++i) {
        if (ingredients[i].getName() == name)
            return i;
    }
    return -1;
}
bool InventoryManager::addIngredient(const string& name, int qty) {
    if (qty < 0) return false;

    int idx = findIngredientIndex(name);

    if (idx == -1) {
        if (ingredientCount >= MAX_INGREDIENTS)
            return false;

        ingredients[ingredientCount].setName(name);
        ingredients[ingredientCount].setQuantity(qty);
        ingredientCount++;
    }
    else {
        ingredients[idx].restock(qty);
    }
    return true;
}

bool InventoryManager::restock(const string& name, int qty) {
    return addIngredient(name, qty);
}

bool InventoryManager::checkAvailabilityForItem(const MenuItem& item, int multiplier) const {
    for (int j = 0; j < item.getIngCount(); ++j) {
        string ingName = item.getIngNameAt(j);
        int requiredQty = item.getIngQtyAt(j) * multiplier;

        int idx = findIngredientIndex(ingName);
        if (idx == -1 || ingredients[idx].getQuantity() < requiredQty)
            return false;
    }
    return true;
}

bool InventoryManager::deductForItem(const MenuItem& item, int multiplier) {
    if (!checkAvailabilityForItem(item, multiplier))
        return false;

    for (int j = 0; j < item.getIngCount(); ++j) {
        int idx = findIngredientIndex(item.getIngNameAt(j));
        if (idx != -1) {
            ingredients[idx].consume(item.getIngQtyAt(j) * multiplier);
        }
    }
    return true;
}

bool InventoryManager::deductForMultiple(const MenuItem menuItems[], const int qty[], int count) {
    for (int i = 0; i < count; ++i) {
        if (!checkAvailabilityForItem(menuItems[i], qty[i]))
            return false;
    }

    for (int i = 0; i < count; ++i) {
        deductForItem(menuItems[i], qty[i]);
    }
    return true;
}

void InventoryManager::listAllStock() const {
    cout << "\n------------ Inventory ------------\n" << endl;

    // Print header
    cout << left << setw(25) << "Ingredient" << "Quantity" << endl;

    cout << string(35, '-') << endl;

    for (int i = 0; i < ingredientCount; ++i) {
        cout << left << setw(25) << ingredients[i].getName() << ingredients[i].getQuantity() << endl;
    }
}
void InventoryManager::lowStockAlerts(int threshold) const {
    cout << "---- Low Stock Alerts (<= " << threshold << ") ----" << endl;
    for (int i = 0; i < ingredientCount; ++i) {
        if (ingredients[i].getQuantity() <= threshold) {
            cout << ingredients[i].getName()
                << " : "
                << ingredients[i].getQuantity()
                << endl;
        }
    }
}
/* --------------- Menu Item --------------- */
MenuItem::MenuItem() : id(""), name(""), price(0.0), available(true), ingCount(0) {}

void MenuItem::setId(const string& i) { id = i; }
void MenuItem::setName(const string& n) { name = n; }
void MenuItem::setPrice(double p) { price = p; }
void MenuItem::setAvailable(bool a) { available = a; }

string MenuItem::getId() const { return id; }
string MenuItem::getName() const { return name; }
double MenuItem::getPrice() const { return price; }
bool MenuItem::isAvailable() const { return available; }

void MenuItem::clearIngredients() { ingCount = 0; }

bool MenuItem::addIngredientRequirement(const string& ingName, int qty) {
    if (ingCount >= MAX_ING_PER_ITEM) return false;
    ingNames[ingCount] = ingName;
    ingQty[ingCount] = qty;
    ingCount++;
    return true;
}

int MenuItem::getIngCount() const { return ingCount; }

string MenuItem::getIngNameAt(int idx) const {
    if (idx < 0 || idx >= ingCount) return "";
    return ingNames[idx];
}

int MenuItem::getIngQtyAt(int idx) const {
    if (idx < 0 || idx >= ingCount)
        return 0;
    return ingQty[idx];
}


/* --------------- Order --------------- */
Order::Order() : orderId(""), itemCount(0), checkedOut(false) {
}

void Order::setOrderId(const string& id) {
    orderId = id;
}

string Order::getOrderId() const {
    return orderId;
}

bool Order::addItem(const MenuItem& m, int q) {
    if (!m.isAvailable() || q <= 0) return false;
    for (int i = 0; i < itemCount; ++i) {
        if (items[i].getId() == m.getId()) {
            qty[i] += q;
            return true;
        }
    }
    if (itemCount >= MAX_ORDER_ITEMS) return false;
    items[itemCount] = m;
    qty[itemCount] = q;
    itemCount++;
    return true;
}

bool Order::removeItem(const string& menuItemId, int q) {
    for (int i = 0; i < itemCount; ++i) {
        if (items[i].getId() == menuItemId) {
            if (q >= qty[i]) {
                for (int j = i; j < itemCount - 1; ++j) {
                    items[j] = items[j + 1];
                    qty[j] = qty[j + 1];
                }
                itemCount--;
            }
            else {
                qty[i] -= q;
            }
            return true;
        }
    }
    return false;
}

double Order::computeTotal() const {
    double total = 0.0;
    for (int i = 0; i < itemCount; ++i) {
        total += items[i].getPrice() * qty[i];
    }
    return total;
}

void Order::checkout() {
    checkedOut = true;
}

bool Order::isCheckedOut() const {
    return checkedOut;
}

int Order::getItemCount() const {
    return itemCount;
}

const MenuItem& Order::getItemAt(int idx) const {
    return items[idx];
}

int Order::getQtyAt(int idx) const {
    return qty[idx];
}


/* --------------- Report--------------- */
double Report::dailySalesReport(const Order orders[], int count) {
    double total = 0.0;
    for (int i = 0; i < count; ++i) {
        total += orders[i].computeTotal();
    }
    return total;
}

void Report::bestSellingItems(const Order orders[], int orderCount) {
    const int MAX_TRACK = 500;
    string names[MAX_TRACK];
    int counts[MAX_TRACK];
    int trackCount = 0;

    for (int i = 0; i < orderCount; ++i) {
        for (int j = 0; j < orders[i].getItemCount(); ++j) {
            string n = orders[i].getItemAt(j).getName();
            int q = orders[i].getQtyAt(j);
            int idx = -1;
            for (int k = 0; k < trackCount; ++k) {
                if (names[k] == n) {
                    idx = k;
                    break;
                }
            }
            if (idx == -1) {
                if (trackCount < MAX_TRACK) {
                    names[trackCount] = n;
                    counts[trackCount] = q;
                    trackCount++;
                }
            }
            else {
                counts[idx] += q;
            }
        }
    }

    for (int t = 0; t < 5 && t < trackCount; ++t) {
        int maxIdx = t;
        for (int u = t + 1; u < trackCount; ++u) {
            if (counts[u] > counts[maxIdx]) {
                maxIdx = u;
            }
        }
        string tmpn = names[t];
        names[t] = names[maxIdx];
        names[maxIdx] = tmpn;

        int tmpc = counts[t];
        counts[t] = counts[maxIdx];
        counts[maxIdx] = tmpc;

        cout << (t + 1) << ") " << names[t] << " - " << counts[t] << " sold" << endl;
    }
}

void Report::inventoryAlerts(const InventoryManager& inv, int threshold) {
    inv.lowStockAlerts(threshold);
}


/* --------------- Staff --------------- */
Staff::Staff() : User(), inventoryRef(nullptr) {}

void Staff::attachInventory(InventoryManager* inv) {
    inventoryRef = inv;
}

Order Staff::createOrder() {
    Order o;
    o.setOrderId("ORD" + to_string(time(nullptr)));
    return o;
}

bool Staff::addItemToOrder(Order& order, MenuItem& item, int qty) {
    if (!item.isAvailable() || qty <= 0) return false;

    if (!inventoryRef->checkAvailabilityForItem(item, qty))
        return false;

    if (!inventoryRef->deductForItem(item, qty))
        return false;

    return order.addItem(item, qty);
}



void Staff::updateOrderStatus(Order& order, const string& status) {
    cout << "Order " << order.getOrderId() << " status updated to: " << status << endl;
}