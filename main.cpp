#include <iostream>
#include <iomanip>
#include <string>
#include <ctime>
#include "Cafe.h" 
#include<fstream>

using namespace std;

const int MAX_MENU = 200;
const int MAX_ORDERS = 200;

int main() {
    InventoryManager inventory;
    FeedbackManager feedback;

    MenuItem* menu = new MenuItem[MAX_MENU];
    int menuCount = 0;

    // ======== Menu Items ========
    MenuItem coffee;
    coffee.setId("M001");
    coffee.setName("Black Coffee");
    coffee.setPrice(450.0);
    coffee.clearIngredients();
    coffee.addIngredientRequirement("CoffeeBeans", 10);
    coffee.addIngredientRequirement("Sugar", 2);
    menu[menuCount++] = coffee;

    MenuItem greenTea;
    greenTea.setId("M002");
    greenTea.setName("Green Tea");
    greenTea.setPrice(350.0);
    greenTea.clearIngredients();
    greenTea.addIngredientRequirement("BlackTeaLeaves", 5);
    greenTea.addIngredientRequirement("Lavender", 3);
    menu[menuCount++] = greenTea;

    MenuItem honeyLemonTea;
    honeyLemonTea.setId("M003");
    honeyLemonTea.setName("Honey Lemon Tea");
    honeyLemonTea.setPrice(400.0);
    honeyLemonTea.clearIngredients();
    honeyLemonTea.addIngredientRequirement("BlackTeaLeaves", 5);
    honeyLemonTea.addIngredientRequirement("Honey", 10);
    honeyLemonTea.addIngredientRequirement("Lemon", 5);
    menu[menuCount++] = honeyLemonTea;

    MenuItem icedLatte;
    icedLatte.setId("M004");
    icedLatte.setName("Iced Latte");
    icedLatte.setPrice(600.0);
    icedLatte.clearIngredients();
    icedLatte.addIngredientRequirement("CoffeeBeans", 8);
    icedLatte.addIngredientRequirement("Milk", 50);
    icedLatte.addIngredientRequirement("Ice", 30);
    menu[menuCount++] = icedLatte;

    MenuItem avocadoToast;
    avocadoToast.setId("M005");
    avocadoToast.setName("Avocado Toast");
    avocadoToast.setPrice(550.0);
    avocadoToast.clearIngredients();
    avocadoToast.addIngredientRequirement("Bread", 2);
    avocadoToast.addIngredientRequirement("Avocado", 1);
    avocadoToast.addIngredientRequirement("Microgreens", 5);
    menu[menuCount++] = avocadoToast;

    MenuItem spicyChiliCoffee;
    spicyChiliCoffee.setId("M006");
    spicyChiliCoffee.setName("Spicy Chili Coffee");
    spicyChiliCoffee.setPrice(500.0);
    spicyChiliCoffee.clearIngredients();
    spicyChiliCoffee.addIngredientRequirement("CoffeeBeans", 10);
    spicyChiliCoffee.addIngredientRequirement("ChiliFlakes", 1);
    spicyChiliCoffee.addIngredientRequirement("Sugar", 2);
    menu[menuCount++] = spicyChiliCoffee;

    MenuItem roseMilk;
    roseMilk.setId("M007");
    roseMilk.setName("Rose Milk");
    roseMilk.setPrice(450.0);
    roseMilk.clearIngredients();
    roseMilk.addIngredientRequirement("Milk", 100);
    roseMilk.addIngredientRequirement("RoseSyrup", 15);
    menu[menuCount++] = roseMilk;

    // ======== Inventory ========
    ifstream fin("ingredient.txt");
    if (!fin) {
        cout << "Failed to open ingredient.txt\n";
    }

    string ingName;
    int qty;

    while (fin >> ingName >> qty) {
        inventory.addIngredient(ingName, qty);
    }

    fin.close();


    // ======== Admin/Staff/Feedback ========
    Admin admin;
    admin.attachMenu(menu, &menuCount);
    admin.attachInventory(&inventory);
    admin.attachFeedback(&feedback);

    Staff staff;
    staff.attachInventory(&inventory);

    Customer customer("Neesa", "password123");

    Order* orders = new Order[MAX_ORDERS];
    int orderCount = 0;

    int roleChoice = 0;

    while (true) {
        cout << "\n" << setw(40) << "WELCOME TO CAFE MANAGEMENT SYSTEM" << "\n\n";
        cout << setw(10) << left << "1." << "Admin\n";
        cout << setw(10) << left << "2." << "Staff\n";
        cout << setw(10) << left << "3." << "Customer\n";
        cout << setw(10) << left << "4." << "Exit\n";
        cout << "\nEnter your choice: ";
        cin >> roleChoice;

        if (roleChoice == 4) break;

       // ===================== ADMIN =====================
        if (roleChoice == 1) {

            string adminPass = "deathnote";
            string enteredPass;

            cout << "\nEnter Admin Password: ";
            cin >> enteredPass;

            if (enteredPass != adminPass) {
                cout << "\nIncorrect password! Access denied.\n";
                continue;
            }

            int adminChoice = 0;

            do {
                cout << "\n" << setw(40) << "ADMIN DASHBOARD" << "\n\n";
                cout << setw(10) << left << "1." << "Add Staff\n";
                cout << setw(10) << left << "2." << "Remove Staff\n";
                cout << setw(10) << left << "3." << "Update Menu Item Price\n";
                cout << setw(10) << left << "4." << "Change Menu Item Availability\n";
                cout << setw(10) << left << "5." << "Restock Inventory\n";
                cout << setw(10) << left << "6." << "View All Feedback\n";
                cout << setw(10) << left << "7." << "View Menu\n";
                cout << setw(10) << left << "8." << "Back\n";
                cout << "\nEnter choice: ";
                cin >> adminChoice;

                if (adminChoice == 1) {
                    string uname;
                    cout << "Enter username to add as staff: ";
                    cin >> uname;
                    if (admin.addStaff(uname))
                        cout << "\nStaff added successfully!\n";
                    else
                        cout << "\nFailed to add staff.\n";
                }
                else if (adminChoice == 2) {
                    string uname;
                    cout << "Enter staff username to remove: ";
                    cin >> uname;
                    if (admin.removeStaff(uname))
                        cout << "\nStaff removed.\n";
                    else
                        cout << "\nStaff not found.\n";
                }
                else if (adminChoice == 3) {
                    string menuId;
                    double newPrice;
                    cout << "Enter Menu Item ID: ";
                    cin >> menuId;
                    cout << "Enter new price: ";
                    cin >> newPrice;
                    if (admin.updateMenuItemPrice(menuId, newPrice))
                        cout << "\nPrice updated.\n";
                    else
                        cout << "\nMenu item not found.\n";
                }
                else if (adminChoice == 4) {
                    string menuId;
                    int availInt;
                    cout << "Enter Menu Item ID: ";
                    cin >> menuId;
                    cout << "Enter availability (1 = available, 0 = not available): ";
                    cin >> availInt;

                    if (admin.changeMenuAvailability(menuId, availInt == 1))
                        cout << "\nAvailability updated.\n";
                    else
                        cout << "\nMenu item not found.\n";
                }
                else if (adminChoice == 5) {
                    string ingName;
                    int qty;
                    cout << "Enter ingredient name to restock: ";
                    cin >> ingName;
                    cout << "Enter quantity to add: ";
                    cin >> qty;
                    if (admin.restockInventory(ingName, qty))
                        cout << "\nInventory restocked.\n";
                    else
                        cout << "\nFailed to restock.\n";
                }
                else if (adminChoice == 6) {
                    admin.viewAllFeedback();
                }
                else if (adminChoice == 7) {
                    cout << "\n";
                    cout << setw(10) << left << "ID"
                        << setw(25) << left << "Item Name"
                        << setw(10) << left << "Price"
                        << "Availability\n";

                    cout << setw(10) << setfill('-') << ""
                        << setw(25) << ""
                        << setw(10) << ""
                        << setw(12) << "" << setfill(' ') << endl;

                    for (int i = 0; i < menuCount; ++i) {
                        cout << setw(10) << left << menu[i].getId()
                            << setw(25) << left << menu[i].getName()
                            << "Rs " << setw(7) << right << fixed << setprecision(2)
                            << menu[i].getPrice()
                            << "  " << (menu[i].isAvailable() ? "Yes" : "No") << endl;
                    }
                }

            } while (adminChoice != 8);
        }


        // ===================== STAFF =====================
        else if (roleChoice == 2) {
            string staffName;
            cout << "Enter staff username: ";
            cin >> staffName;

            bool found = false;
            ifstream ifs("staff.txt");
            string name;
            while (ifs >> name) {
                if (name == staffName) {
                    found = true;
                    break;
                }
            }
            ifs.close();

            if (!found) {
                cout << "\nUnauthorized staff.\n";
                continue;
            }
            int staffChoice = 0;
            static Order currentOrder;
            static bool orderCreated = false;

            do {
                cout << "\n" << setw(40) << "STAFF PANEL" << "\n\n";
                cout << setw(10) << left << "1." << "Create Order\n";
                cout << setw(10) << left << "2." << "Add Item to Order\n";
                cout << setw(10) << left << "3." << "Update Order Status\n";
                cout << setw(10) << left << "4." << "Back\n";
                cout << "\nEnter choice: ";
                cin >> staffChoice;

                if (staffChoice == 1) {
                    currentOrder = staff.createOrder();
                    orderCreated = true;
                    cout << "\nNew order created. Order ID: " << currentOrder.getOrderId() << endl;
                }
                else if (staffChoice == 2) {
                    if (!orderCreated) {
                        cout << "\nPlease create an order first.\n";
                        continue;
                    }
                    cout << "\n";
                    cout << setw(10) << left << "ID" 
                         << setw(25) << left << "Item Name" 
                         << setw(10) << left << "Price" 
                         << "Availability" << endl;

                    cout << setw(10) << setfill('-') << "" 
                         << setw(25) << "" 
                         << setw(10) << "" 
                         << setw(12) << "" << setfill(' ') << endl;

                    for (int i = 0; i < menuCount; ++i) {
                        cout << setw(10) << left << menu[i].getId()
                             << setw(25) << left << menu[i].getName()
                             << "Rs " << setw(7) << right << fixed << setprecision(2) << menu[i].getPrice()
                             << "  " << (menu[i].isAvailable() ? "Yes" : "No") << endl;
                    }

                    string menuId;
                    int qty;
                    cout << "\nEnter Menu Item ID: ";
                    cin >> menuId;
                    cout << "Enter quantity: ";
                    cin >> qty;

                    MenuItem* selectedItem = nullptr;
                    for (int i = 0; i < menuCount; ++i) {
                        if (menu[i].getId() == menuId) {
                            selectedItem = &menu[i];
                            break;
                        }
                    }

                    if (!selectedItem) {
                        cout << "\nMenu item not found.\n";
                        continue;
                    }

                    if (staff.addItemToOrder(currentOrder, *selectedItem, qty))
                        cout << "\nItem added to order.\n";
                    else
                        cout << "\nFailed to add item (out of stock or unavailable).\n";
                }
                else if (staffChoice == 3) {
                    if (!orderCreated) {
                        cout << "\nPlease create an order first.\n";
                        continue;
                    }
                    string status;
                    cout << "Enter new status for order " << currentOrder.getOrderId() << ": ";
                    cin.ignore();
                    getline(cin, status);

                    staff.updateOrderStatus(currentOrder, status);

                    if (!currentOrder.isCheckedOut()) {
                        currentOrder.checkout();
                        orders[orderCount++] = currentOrder;
                        cout << "\nOrder checked out and saved.\n";
                        orderCreated = false;
                    }
                }

            } while (staffChoice != 4);
        }

        // ===================== CUSTOMER =====================
        else if (roleChoice == 3) {
            cout << "\n";
            cout << setw(10) << left << "ID" 
                 << setw(25) << left << "Item Name" 
                 << setw(10) << left << "Price" 
                 << "Availability" << endl;

            cout << setw(10) << setfill('-') << "" << setw(25) << "" << setw(10) << "" << setw(12) << "" << setfill(' ') << endl;

            for (int i = 0; i < menuCount; ++i) {
                cout << setw(10) << left << menu[i].getId()<< setw(25) << left << menu[i].getName()<< "Rs " << setw(7) << right << fixed << setprecision(2) << menu[i].getPrice()<< "  " << (menu[i].isAvailable() ? "Yes" : "No") << endl;
            }

            int customerChoice = 0;
            do {
                cout << "\n" << setw(40) << "CUSTOMER MENU" << "\n\n";
                cout << setw(10) << left << "1." << "Place an Order\n";
                cout << setw(10) << left << "2." << "Give Feedback\n";
                cout << setw(10) << left << "3." << "Back\n";
                cout << "\nEnter choice: ";
                cin >> customerChoice;

                if (customerChoice == 1) {
                    Order newOrder;
                    newOrder.setOrderId("ORD" + to_string(time(nullptr)));

                    int orderItemCount;
                    cout << "How many items to order? ";
                    cin >> orderItemCount;

                    for (int i = 0; i < orderItemCount; ++i) {
                        string menuId;
                        int qty;
                        cout << "Enter Menu Item ID: ";
                        cin >> menuId;
                        cout << "Enter quantity: ";
                        cin >> qty;

                        MenuItem* selectedItem = nullptr;
                        for (int j = 0; j < menuCount; ++j) {
                            if (menu[j].getId() == menuId) {
                                selectedItem = &menu[j];
                                break;
                            }
                        }

                        if (!selectedItem) {
                            cout << "Menu item not found. Skipping.\n";
                            continue;
                        }

                        if (!newOrder.addItem(*selectedItem, qty)) {
                            cout << "Failed to add item " << selectedItem->getName() << " to order.\n";
                        }
                    }

                    if (customer.placeOrder(newOrder)) {
                        double total = Billing::applyDiscounts(newOrder, 0.0);
                        Billing::generateReceipt(newOrder, total, customer.getUsername());
                        orders[orderCount++] = newOrder;
                        cout << fixed << setprecision(2);
                        cout << "\nOrder " << newOrder.getOrderId() << " placed. Total: " << total << "\n";
                    } else {
                        cout << "Order placement failed.\n";
                    }
                }
                else if (customerChoice == 2) {
                    cin.ignore();
                    string custName, comment;
                    int rating;

                    cout << "Enter your name: ";
                    getline(cin, custName);

                    cout << "Enter your feedback comment: ";
                    getline(cin, comment);

                    cout << "Enter rating (1-5): ";
                    cin >> rating;

                    Customer tempCustomer(custName, "");
                    if (tempCustomer.giveFeedback(feedback, comment, rating))
                        cout << "\nFeedback submitted. Thank you!\n";
                    else
                        cout << "\nFailed to submit feedback.\n";
                }

            } while (customerChoice != 3);
        }
        else {
            cout << "\nInvalid role choice.\n";
        }
    }


    delete[] menu;
    delete[] orders;

    cout << "\nExiting program.\n";
    return 0;
}
