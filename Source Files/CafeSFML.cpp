#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <numeric>

// Include your backend header
#include "Cafe.h"

using namespace sf;
using namespace std;

// Colors
const Color PRIMARY_COLOR(205, 179, 139);      // Khaki Brown
const Color SECONDARY_COLOR(76, 47, 39);       // Dark Espresso
const Color ACCENT_COLOR(220, 20, 60);        // Crimson
const Color BACKGROUND_COLOR(245, 245, 245);  // Light Gray
const Color TEXT_COLOR(50, 50, 50);           // Dark Gray
const Color BUTTON_HOVER_COLOR(175, 149, 109);

// Global variables for backend
InventoryManager* inventory = nullptr;
FeedbackManager* feedback = nullptr;
MenuItem* menu = nullptr;
int menuCount = 0;
Admin* admin = nullptr;
Staff* staffUser = nullptr;
Customer* customerUser = nullptr;
Order* currentOrder = nullptr;
Order* allOrders = nullptr;
int orderCount = 0;
const int MAX_ORDERS = 200;

// Button class
class Button {
public:
    RectangleShape shape;
    Text text;
    bool isHovered;

    Button(Font& font, const string& label, Vector2f pos, Vector2f size)
        : isHovered(false) {
        shape.setSize(size);
        shape.setPosition(pos);
        shape.setFillColor(PRIMARY_COLOR);
        shape.setOutlineThickness(2);
        shape.setOutlineColor(Color::White);

        text.setFont(font);
        text.setString(label);
        text.setCharacterSize(18);
        text.setFillColor(Color::White);

        FloatRect textRect = text.getLocalBounds();
        text.setOrigin(textRect.left + textRect.width / 2.0f,
            textRect.top + textRect.height / 2.0f);
        text.setPosition(pos.x + size.x / 2.0f, pos.y + size.y / 2.0f);
    }

    void update(Vector2f mousePos) {
        isHovered = shape.getGlobalBounds().contains(mousePos);
        shape.setFillColor(isHovered ? BUTTON_HOVER_COLOR : PRIMARY_COLOR);
    }

    void draw(RenderWindow& window) {
        window.draw(shape);
        window.draw(text);
    }

    bool contains(Vector2f point) {
        return shape.getGlobalBounds().contains(point);
    }
};

// InputBox class
class InputBox {
public:
    RectangleShape box;
    Text label;
    Text inputText;
    string input;
    bool active;
    int maxLength;

    InputBox(Font& font, const string& lbl, Vector2f pos, Vector2f size, int maxLen = 50)
        : input(""), active(false), maxLength(maxLen) {
        box.setSize(size);
        box.setPosition(pos);
        box.setFillColor(Color::White);
        box.setOutlineThickness(2);
        box.setOutlineColor(active ? SECONDARY_COLOR : Color(200, 200, 200));

        label.setFont(font);
        label.setString(lbl);
        label.setCharacterSize(16);
        label.setFillColor(TEXT_COLOR);
        label.setPosition(pos.x, pos.y - 25);

        inputText.setFont(font);
        inputText.setString(input);
        inputText.setCharacterSize(18);
        inputText.setFillColor(Color::Black);
        inputText.setPosition(pos.x + 10, pos.y + 10);
    }

    void update(bool isClicked, Vector2f mousePos) {
        if (isClicked) {
            active = box.getGlobalBounds().contains(mousePos);
            box.setOutlineColor(active ? SECONDARY_COLOR : Color(200, 200, 200));
        }
    }

    void handleText(char c) {
        if (!active) return;

        if (c == 8) { // Backspace
            if (!input.empty()) {
                input.pop_back();
            }
        }
        else if (c == 13) { // Enter
            active = false;
            box.setOutlineColor(Color(200, 200, 200));
        }
        else if (input.length() < maxLength && isprint(c)) {
            input.push_back(c);
        }

        inputText.setString(input);
    }

    void draw(RenderWindow& window) {
        window.draw(box);
        window.draw(label);
        window.draw(inputText);
    }

    void clear() {
        input.clear();
        inputText.setString(input);
    }
};

// MessageDisplay class for showing messages
class MessageDisplay {
public:
    Text message;
    Clock displayClock;
    float displayTime;
    bool isShowing;

    MessageDisplay(Font& font) : displayTime(3.0f), isShowing(false) {
        message.setFont(font);
        message.setCharacterSize(18);
        message.setPosition(400, 550);
    }

    void show(const string& msg, Color color = ACCENT_COLOR) {
        message.setString(msg);
        message.setFillColor(color);
        FloatRect bounds = message.getLocalBounds();
        message.setOrigin(bounds.width / 2, 0);
        message.setPosition(512, 550);
        displayClock.restart();
        isShowing = true;
    }

    void update() {
        if (isShowing && displayClock.getElapsedTime().asSeconds() > displayTime) {
            isShowing = false;
        }
    }

    void draw(RenderWindow& window) {
        if (isShowing) {
            window.draw(message);
        }
    }
};

// Table class for displaying data
class Table {
public:
    vector<Text> headers;
    vector<vector<Text>> rows;
    Font* font;
    Vector2f position;
    float rowHeight;
    vector<float> columnWidths;

    Table(Font& f, Vector2f pos) : font(&f), position(pos), rowHeight(30.0f) {}

    void setHeaders(const vector<string>& headerStrings, const vector<float>& widths) {
        headers.clear();
        columnWidths = widths;

        float x = position.x;
        for (size_t i = 0; i < headerStrings.size(); i++) {
            Text header;
            header.setFont(*font);
            header.setString(headerStrings[i]);
            header.setCharacterSize(18);
            header.setFillColor(Color::White);
            header.setPosition(x + 10, position.y);
            headers.push_back(header);
            x += widths[i];
        }
    }

    void addRow(const vector<string>& rowData) {
        vector<Text> row;
        float x = position.x;
        float y = position.y + headers.size() * rowHeight + (rows.size() * rowHeight);

        for (size_t i = 0; i < rowData.size(); i++) {
            Text cell;
            cell.setFont(*font);
            cell.setString(rowData[i]);
            cell.setCharacterSize(16);
            cell.setFillColor(TEXT_COLOR);
            cell.setPosition(x + 10, y);
            row.push_back(cell);
            x += columnWidths[i];
        }
        rows.push_back(row);
    }

    void clearRows() {
        rows.clear();
    }

    void draw(RenderWindow& window) {
        // Calculate total width
        float totalWidth = 0;
        for (float width : columnWidths) {
            totalWidth += width;
        }

        // Draw header background
        RectangleShape headerBg(Vector2f(totalWidth, rowHeight));
        headerBg.setPosition(position);
        headerBg.setFillColor(SECONDARY_COLOR);
        window.draw(headerBg);

        for (auto& header : headers) {
            window.draw(header);
        }

        // Draw rows with alternating colors
        float maxVisibleHeight = 400.0f;
        for (size_t i = 0; i < rows.size(); i++) {
            float rowY = position.y + rowHeight + (i * rowHeight);

            if (rowY > position.y + maxVisibleHeight) {
                continue;
            }

            RectangleShape rowBg(Vector2f(totalWidth, rowHeight));
            rowBg.setPosition(position.x, rowY);
            rowBg.setFillColor(i % 2 == 0 ? Color(240, 240, 240) : Color(255, 255, 255));
            window.draw(rowBg);

            for (auto& cell : rows[i]) {
                Vector2f cellPos = cell.getPosition();
                cell.setPosition(cellPos.x, rowY + 5);
                window.draw(cell);
                cell.setPosition(cellPos);
            }
        }

        if (rows.size() * rowHeight > maxVisibleHeight) {
            Text moreText;
            moreText.setFont(*font);
            moreText.setString("... " + to_string(rows.size() - static_cast<int>(maxVisibleHeight / rowHeight)) + " more entries");
            moreText.setCharacterSize(14);
            moreText.setFillColor(Color::Red);
            moreText.setPosition(position.x + 10, position.y + maxVisibleHeight + 10);
            window.draw(moreText);
        }
    }
};

// Application states
enum class AppState {
    RoleSelection,
    AdminLogin,
    AdminMenu,
    AdminAddStaff,
    AdminRemoveStaff,
    AdminUpdatePrice,
    AdminChangeAvailability,
    AdminRestock,
    AdminViewFeedback,
    AdminViewMenu,
    StaffLogin,
    StaffMenu,
    StaffCreateOrder,
    CustomerMenu,
    CustomerPlaceOrder,
    CustomerViewMenu,
    CustomerGiveFeedback,
    ViewInventory,
    Reports
};

// Initialize backend systems
void initializeBackend() {
    inventory = new InventoryManager();
    feedback = new FeedbackManager();
    menu = new MenuItem[200];
    admin = new Admin();
    staffUser = new Staff();
    customerUser = new Customer("Guest", "");
    allOrders = new Order[MAX_ORDERS];
    orderCount = 0;

    // Load menu items
    menuCount = 0;

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

    // Load inventory
    ifstream fin("ingredient.txt");
    if (fin.is_open()) {
        string ingName;
        int qty;
        while (fin >> ingName >> qty) {
            inventory->addIngredient(ingName, qty);
        }
        fin.close();
    }

    // Attach references to admin
    admin->attachMenu(menu, &menuCount);
    admin->attachInventory(inventory);
    admin->attachFeedback(feedback);

    // Attach inventory to staff
    staffUser->attachInventory(inventory);

    // Load existing feedback
    feedback->loadFromFile();
}

// Cleanup backend systems
void cleanupBackend() {
    delete[] menu;
    delete[] allOrders;
    delete inventory;
    delete feedback;
    delete admin;
    delete staffUser;
    delete customerUser;
}

int main() {
    RenderWindow window(VideoMode(1024, 768), "Cafe Management System");
    window.setFramerateLimit(60);

    // Load font
    Font font;
    if (!font.loadFromFile("arial.ttf")) {
        if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
            cerr << "Failed to load font" << endl;
            return -1;
        }
    }

    // Initialize backend
    initializeBackend();

    // Create message display
    MessageDisplay messageDisplay(font);

    // Application state
    AppState state = AppState::RoleSelection;

    // UI Elements
    vector<Button> roleButtons;
    vector<Button> adminMenuButtons;
    vector<Button> staffMenuButtons;
    vector<Button> customerMenuButtons;
    vector<InputBox> inputBoxes;
    Table* menuTable = nullptr;
    Table* inventoryTable = nullptr;
    Table* feedbackTable = nullptr;

    // Variables for specific states
    bool orderCreated = false;

    // Rebuild UI function
    auto rebuildUI = [&]() {
        roleButtons.clear();
        adminMenuButtons.clear();
        staffMenuButtons.clear();
        customerMenuButtons.clear();
        inputBoxes.clear();
        delete menuTable;
        delete inventoryTable;
        delete feedbackTable;
        menuTable = nullptr;
        inventoryTable = nullptr;
        feedbackTable = nullptr;

        switch (state) {
        case AppState::RoleSelection:
            roleButtons.emplace_back(font, "Admin", Vector2f(412, 200), Vector2f(200, 50));
            roleButtons.emplace_back(font, "Staff", Vector2f(412, 270), Vector2f(200, 50));
            roleButtons.emplace_back(font, "Customer", Vector2f(412, 340), Vector2f(200, 50));
            roleButtons.emplace_back(font, "View Inventory", Vector2f(412, 410), Vector2f(200, 50));
            roleButtons.emplace_back(font, "Reports", Vector2f(412, 480), Vector2f(200, 50));
            roleButtons.emplace_back(font, "Exit", Vector2f(412, 550), Vector2f(200, 50));
            break;

        case AppState::AdminLogin:
            inputBoxes.emplace_back(font, "Admin Password:", Vector2f(362, 300), Vector2f(300, 40));
            break;

        case AppState::AdminMenu:
            adminMenuButtons.emplace_back(font, "Add Staff", Vector2f(412, 150), Vector2f(200, 40));
            adminMenuButtons.emplace_back(font, "Remove Staff", Vector2f(412, 200), Vector2f(200, 40));
            adminMenuButtons.emplace_back(font, "Update Menu Price", Vector2f(412, 250), Vector2f(200, 40));
            adminMenuButtons.emplace_back(font, "Change Availability", Vector2f(412, 300), Vector2f(200, 40));
            adminMenuButtons.emplace_back(font, "Restock Inventory", Vector2f(412, 350), Vector2f(200, 40));
            adminMenuButtons.emplace_back(font, "View Feedback", Vector2f(412, 400), Vector2f(200, 40));
            adminMenuButtons.emplace_back(font, "View Menu", Vector2f(412, 450), Vector2f(200, 40));
            adminMenuButtons.emplace_back(font, "Back", Vector2f(412, 550), Vector2f(200, 40));
            break;

        case AppState::AdminAddStaff:
            inputBoxes.emplace_back(font, "Staff Username:", Vector2f(362, 300), Vector2f(300, 40));
            adminMenuButtons.emplace_back(font, "Add", Vector2f(362, 370), Vector2f(100, 40));
            adminMenuButtons.emplace_back(font, "Back", Vector2f(562, 370), Vector2f(100, 40));
            break;

        case AppState::AdminRemoveStaff:
            inputBoxes.emplace_back(font, "Staff Username:", Vector2f(362, 300), Vector2f(300, 40));
            adminMenuButtons.emplace_back(font, "Remove", Vector2f(362, 370), Vector2f(100, 40));
            adminMenuButtons.emplace_back(font, "Back", Vector2f(562, 370), Vector2f(100, 40));
            break;

        case AppState::AdminUpdatePrice:
            inputBoxes.emplace_back(font, "Menu Item ID:", Vector2f(362, 250), Vector2f(300, 40));
            inputBoxes.emplace_back(font, "New Price:", Vector2f(362, 320), Vector2f(300, 40));
            adminMenuButtons.emplace_back(font, "Update", Vector2f(362, 390), Vector2f(100, 40));
            adminMenuButtons.emplace_back(font, "Back", Vector2f(562, 390), Vector2f(100, 40));
            break;

        case AppState::AdminChangeAvailability:
            inputBoxes.emplace_back(font, "Menu Item ID:", Vector2f(362, 300), Vector2f(300, 40));
            adminMenuButtons.emplace_back(font, "Set Available", Vector2f(362, 370), Vector2f(130, 40));
            adminMenuButtons.emplace_back(font, "Set Unavailable", Vector2f(512, 370), Vector2f(150, 40));
            adminMenuButtons.emplace_back(font, "Back", Vector2f(412, 450), Vector2f(200, 40));
            break;

        case AppState::AdminRestock:
            inputBoxes.emplace_back(font, "Ingredient Name:", Vector2f(362, 250), Vector2f(300, 40));
            inputBoxes.emplace_back(font, "Quantity:", Vector2f(362, 320), Vector2f(300, 40));
            adminMenuButtons.emplace_back(font, "Restock", Vector2f(362, 390), Vector2f(100, 40));
            adminMenuButtons.emplace_back(font, "Back", Vector2f(562, 390), Vector2f(100, 40));
            break;

        case AppState::AdminViewFeedback:
            feedbackTable = new Table(font, Vector2f(50, 150));
            feedbackTable->rowHeight = 40.0f;
            feedbackTable->setHeaders({ "Customer", "Rating", "Comment" }, { 200, 100, 600 });

            {
                ifstream ifs("feedback.txt");
                if (ifs.is_open()) {
                    string line;
                    int feedbackCount = 0;
                    while (getline(ifs, line)) {
                        feedbackCount++;
                        if (!line.empty()) {
                            stringstream ss(line);
                            string user, comment, ratingStr;

                            if (getline(ss, user, '|') &&
                                getline(ss, comment, '|') &&
                                getline(ss, ratingStr)) {

                                user = user.substr(0, min(30, (int)user.length()));
                                comment = comment.substr(0, min(50, (int)comment.length()));

                                feedbackTable->addRow({ user, ratingStr, comment });
                            }
                        }
                    }
                    ifs.close();
                }
                else {
                    feedbackTable->addRow({ "No feedback", "file found", "" });
                }
            }

            adminMenuButtons.emplace_back(font, "Back", Vector2f(412, 650), Vector2f(200, 40));
            break;

        case AppState::AdminViewMenu:
            menuTable = new Table(font, Vector2f(50, 100));
            menuTable->setHeaders({ "ID", "Name", "Price", "Available" }, { 100, 200, 100, 100 });
            for (int i = 0; i < menuCount; i++) {
                menuTable->addRow({
                    menu[i].getId(),
                    menu[i].getName(),
                    "$" + to_string(menu[i].getPrice()),
                    menu[i].isAvailable() ? "Yes" : "No"
                    });
            }
            adminMenuButtons.emplace_back(font, "Back", Vector2f(412, 650), Vector2f(200, 40));
            break;

        case AppState::StaffLogin:
            inputBoxes.emplace_back(font, "Staff Username:", Vector2f(362, 300), Vector2f(300, 40));
            break;

        case AppState::StaffMenu:
            staffMenuButtons.emplace_back(font, "Create Order", Vector2f(412, 150), Vector2f(200, 40));
            staffMenuButtons.emplace_back(font, "View Menu", Vector2f(412, 250), Vector2f(200, 40));
            staffMenuButtons.emplace_back(font, "Back", Vector2f(412, 350), Vector2f(200, 40));
            break;

        case AppState::StaffCreateOrder:
            if (!currentOrder) {
                currentOrder = new Order();
                currentOrder->setOrderId("ORD" + to_string(time(nullptr)));
                orderCreated = true;
            }
            menuTable = new Table(font, Vector2f(50, 100));
            menuTable->setHeaders({ "ID", "Name", "Price", "Add" }, { 100, 200, 100, 100 });
            for (int i = 0; i < menuCount; i++) {
                menuTable->addRow({
                    menu[i].getId(),
                    menu[i].getName(),
                    "$" + to_string(menu[i].getPrice()),
                    "Add"
                    });
            }
            staffMenuButtons.emplace_back(font, "Checkout", Vector2f(412, 600), Vector2f(200, 40));
            staffMenuButtons.emplace_back(font, "Cancel", Vector2f(412, 650), Vector2f(200, 40));
            break;

        case AppState::CustomerMenu:
            customerMenuButtons.emplace_back(font, "View Menu", Vector2f(412, 200), Vector2f(200, 40));
            customerMenuButtons.emplace_back(font, "Place Order", Vector2f(412, 250), Vector2f(200, 40));
            customerMenuButtons.emplace_back(font, "Give Feedback", Vector2f(412, 300), Vector2f(200, 40));
            customerMenuButtons.emplace_back(font, "Back", Vector2f(412, 400), Vector2f(200, 40));
            break;

        case AppState::CustomerViewMenu:
            menuTable = new Table(font, Vector2f(50, 100));
            menuTable->setHeaders({ "ID", "Name", "Price", "Available" }, { 100, 200, 100, 100 });
            for (int i = 0; i < menuCount; i++) {
                menuTable->addRow({
                    menu[i].getId(),
                    menu[i].getName(),
                    "$" + to_string(menu[i].getPrice()),
                    menu[i].isAvailable() ? "Yes" : "No"
                    });
            }
            customerMenuButtons.emplace_back(font, "Back", Vector2f(412, 650), Vector2f(200, 40));
            break;

        case AppState::CustomerPlaceOrder:
            menuTable = new Table(font, Vector2f(50, 100));
            menuTable->setHeaders({ "ID", "Name", "Price", "Add" }, { 100, 200, 100, 100 });
            for (int i = 0; i < menuCount; i++) {
                menuTable->addRow({
                    menu[i].getId(),
                    menu[i].getName(),
                    "$" + to_string(menu[i].getPrice()),
                    "Add"
                    });
            }
            inputBoxes.emplace_back(font, "Item ID:", Vector2f(50, 600), Vector2f(150, 40));
            inputBoxes.emplace_back(font, "Quantity:", Vector2f(250, 600), Vector2f(150, 40));
            customerMenuButtons.emplace_back(font, "Add Item", Vector2f(450, 600), Vector2f(150, 40));
            customerMenuButtons.emplace_back(font, "Place Order", Vector2f(650, 600), Vector2f(150, 40));
            customerMenuButtons.emplace_back(font, "Back", Vector2f(850, 600), Vector2f(150, 40));
            break;

        case AppState::CustomerGiveFeedback:
            inputBoxes.emplace_back(font, "Your Name:", Vector2f(362, 200), Vector2f(300, 40));
            inputBoxes.emplace_back(font, "Comment:", Vector2f(362, 270), Vector2f(300, 40));
            inputBoxes.emplace_back(font, "Rating (1-5):", Vector2f(362, 340), Vector2f(300, 40));
            customerMenuButtons.emplace_back(font, "Submit", Vector2f(362, 420), Vector2f(100, 40));
            customerMenuButtons.emplace_back(font, "Back", Vector2f(562, 420), Vector2f(100, 40));
            break;

        case AppState::ViewInventory:
            inventoryTable = new Table(font, Vector2f(50, 100));
            inventoryTable->setHeaders({ "Ingredient", "Quantity" }, { 400, 200 });
            inventoryTable->addRow({ "CoffeeBeans", "100" });
            inventoryTable->addRow({ "Sugar", "50" });
            inventoryTable->addRow({ "Milk", "200" });
            roleButtons.emplace_back(font, "Back", Vector2f(412, 650), Vector2f(200, 40));
            break;

        case AppState::Reports:
            roleButtons.emplace_back(font, "Daily Sales", Vector2f(412, 200), Vector2f(200, 40));
            roleButtons.emplace_back(font, "Best Sellers", Vector2f(412, 250), Vector2f(200, 40));
            roleButtons.emplace_back(font, "Low Stock", Vector2f(412, 300), Vector2f(200, 40));
            roleButtons.emplace_back(font, "Back", Vector2f(412, 400), Vector2f(200, 40));
            break;
        }
        };

    // Initial UI build
    rebuildUI();

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }

            Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

            // Update button hover states
            for (auto& btn : roleButtons) btn.update(mousePos);
            for (auto& btn : adminMenuButtons) btn.update(mousePos);
            for (auto& btn : staffMenuButtons) btn.update(mousePos);
            for (auto& btn : customerMenuButtons) btn.update(mousePos);

            // Update input boxes
            bool isMouseClicked = event.type == Event::MouseButtonPressed &&
                event.mouseButton.button == Mouse::Left;
            for (auto& inputBox : inputBoxes) {
                inputBox.update(isMouseClicked, mousePos);
            }

            if (event.type == Event::TextEntered) {
                // Handle text input for active input boxes
                for (auto& inputBox : inputBoxes) {
                    inputBox.handleText(static_cast<char>(event.text.unicode));
                }
            }

            if (isMouseClicked) {
                // Handle button clicks based on current state
                switch (state) {
                case AppState::RoleSelection:
                    for (auto& btn : roleButtons) {
                        if (btn.contains(mousePos)) {
                            string label = btn.text.getString();
                            if (label == "Admin") {
                                state = AppState::AdminLogin;
                                rebuildUI();
                            }
                            else if (label == "Staff") {
                                state = AppState::StaffLogin;
                                rebuildUI();
                            }
                            else if (label == "Customer") {
                                state = AppState::CustomerMenu;
                                rebuildUI();
                            }
                            else if (label == "View Inventory") {
                                state = AppState::ViewInventory;
                                rebuildUI();
                            }
                            else if (label == "Reports") {
                                state = AppState::Reports;
                                rebuildUI();
                            }
                            else if (label == "Exit") {
                                window.close();
                            }
                        }
                    }
                    break;

                case AppState::AdminLogin:
                    if (inputBoxes.size() > 0 && inputBoxes[0].input == "deathnote") {
                        state = AppState::AdminMenu;
                        rebuildUI();
                        messageDisplay.show("Admin login successful!", Color::Green);
                    }
                    else if (inputBoxes.size() > 0 && !inputBoxes[0].input.empty()) {
                        messageDisplay.show("Incorrect password!", ACCENT_COLOR);
                    }
                    break;

                case AppState::AdminMenu:
                    for (auto& btn : adminMenuButtons) {
                        if (btn.contains(mousePos)) {
                            string label = btn.text.getString();
                            if (label == "Add Staff") {
                                state = AppState::AdminAddStaff;
                                rebuildUI();
                            }
                            else if (label == "Remove Staff") {
                                state = AppState::AdminRemoveStaff;
                                rebuildUI();
                            }
                            else if (label == "Update Menu Price") {
                                state = AppState::AdminUpdatePrice;
                                rebuildUI();
                            }
                            else if (label == "Change Availability") {
                                state = AppState::AdminChangeAvailability;
                                rebuildUI();
                            }
                            else if (label == "Restock Inventory") {
                                state = AppState::AdminRestock;
                                rebuildUI();
                            }
                            else if (label == "View Feedback") {
                                state = AppState::AdminViewFeedback;
                                rebuildUI();
                            }
                            else if (label == "View Menu") {
                                state = AppState::AdminViewMenu;
                                rebuildUI();
                            }
                            else if (label == "Back") {
                                state = AppState::RoleSelection;
                                rebuildUI();
                            }
                        }
                    }
                    break;

                case AppState::AdminAddStaff:
                    for (auto& btn : adminMenuButtons) {
                        if (btn.contains(mousePos)) {
                            string label = btn.text.getString();
                            if (label == "Add" && inputBoxes.size() > 0) {
                                string username = inputBoxes[0].input;
                                if (!username.empty()) {
                                    if (admin->addStaff(username)) {
                                        messageDisplay.show("Staff added successfully!", Color::Green);
                                        inputBoxes[0].clear();
                                    }
                                    else {
                                        messageDisplay.show("Failed to add staff.", ACCENT_COLOR);
                                    }
                                }
                            }
                            else if (label == "Back") {
                                state = AppState::AdminMenu;
                                rebuildUI();
                            }
                        }
                    }
                    break;

                case AppState::AdminRemoveStaff:
                    for (auto& btn : adminMenuButtons) {
                        if (btn.contains(mousePos)) {
                            string label = btn.text.getString();
                            if (label == "Remove" && inputBoxes.size() > 0) {
                                string username = inputBoxes[0].input;
                                if (!username.empty()) {
                                    if (admin->removeStaff(username)) {
                                        messageDisplay.show("Staff removed successfully!", Color::Green);
                                        inputBoxes[0].clear();
                                    }
                                    else {
                                        messageDisplay.show("Staff not found.", ACCENT_COLOR);
                                    }
                                }
                            }
                            else if (label == "Back") {
                                state = AppState::AdminMenu;
                                rebuildUI();
                            }
                        }
                    }
                    break;

                case AppState::AdminUpdatePrice:
                    for (auto& btn : adminMenuButtons) {
                        if (btn.contains(mousePos)) {
                            string label = btn.text.getString();
                            if (label == "Update" && inputBoxes.size() > 1) {
                                string menuId = inputBoxes[0].input;
                                string priceStr = inputBoxes[1].input;
                                if (!menuId.empty() && !priceStr.empty()) {
                                    try {
                                        double newPrice = stod(priceStr);
                                        if (admin->updateMenuItemPrice(menuId, newPrice)) {
                                            messageDisplay.show("Price updated successfully!", Color::Green);
                                            inputBoxes[0].clear();
                                            inputBoxes[1].clear();
                                        }
                                        else {
                                            messageDisplay.show("Menu item not found.", ACCENT_COLOR);
                                        }
                                    }
                                    catch (...) {
                                        messageDisplay.show("Invalid price format.", ACCENT_COLOR);
                                    }
                                }
                            }
                            else if (label == "Back") {
                                state = AppState::AdminMenu;
                                rebuildUI();
                            }
                        }
                    }
                    break;

                case AppState::AdminChangeAvailability:
                    for (auto& btn : adminMenuButtons) {
                        if (btn.contains(mousePos)) {
                            string label = btn.text.getString();
                            if (label == "Set Available" && inputBoxes.size() > 0) {
                                string menuId = inputBoxes[0].input;
                                if (!menuId.empty()) {
                                    if (admin->changeMenuAvailability(menuId, true)) {
                                        messageDisplay.show("Item set to available!", Color::Green);
                                    }
                                    else {
                                        messageDisplay.show("Menu item not found.", ACCENT_COLOR);
                                    }
                                }
                            }
                            else if (label == "Set Unavailable" && inputBoxes.size() > 0) {
                                string menuId = inputBoxes[0].input;
                                if (!menuId.empty()) {
                                    if (admin->changeMenuAvailability(menuId, false)) {
                                        messageDisplay.show("Item set to unavailable!", Color::Green);
                                    }
                                    else {
                                        messageDisplay.show("Menu item not found.", ACCENT_COLOR);
                                    }
                                }
                            }
                            else if (label == "Back") {
                                state = AppState::AdminMenu;
                                rebuildUI();
                            }
                        }
                    }
                    break;

                case AppState::AdminRestock:
                    for (auto& btn : adminMenuButtons) {
                        if (btn.contains(mousePos)) {
                            string label = btn.text.getString();
                            if (label == "Restock" && inputBoxes.size() > 1) {
                                string ingName = inputBoxes[0].input;
                                string qtyStr = inputBoxes[1].input;
                                if (!ingName.empty() && !qtyStr.empty()) {
                                    try {
                                        int qty = stoi(qtyStr);
                                        if (admin->restockInventory(ingName, qty)) {
                                            messageDisplay.show("Inventory restocked!", Color::Green);
                                            inputBoxes[0].clear();
                                            inputBoxes[1].clear();
                                        }
                                        else {
                                            messageDisplay.show("Failed to restock.", ACCENT_COLOR);
                                        }
                                    }
                                    catch (...) {
                                        messageDisplay.show("Invalid quantity.", ACCENT_COLOR);
                                    }
                                }
                            }
                            else if (label == "Back") {
                                state = AppState::AdminMenu;
                                rebuildUI();
                            }
                        }
                    }
                    break;

                case AppState::AdminViewFeedback:
                case AppState::AdminViewMenu:
                    for (auto& btn : adminMenuButtons) {
                        if (btn.contains(mousePos)) {
                            if (btn.text.getString() == "Back") {
                                state = AppState::AdminMenu;
                                rebuildUI();
                            }
                        }
                    }
                    break;

                case AppState::StaffLogin:
                    if (inputBoxes.size() > 0) {
                        string username = inputBoxes[0].input;
                        bool found = false;
                        ifstream ifs("staff.txt");
                        string name;
                        while (ifs >> name) {
                            if (name == username) {
                                found = true;
                                break;
                            }
                        }
                        ifs.close();

                        if (found) {
                            state = AppState::StaffMenu;
                            rebuildUI();
                            messageDisplay.show("Staff login successful!", Color::Green);
                        }
                        else if (!username.empty()) {
                            messageDisplay.show("Unauthorized staff.", ACCENT_COLOR);
                        }
                    }
                    break;

                case AppState::StaffMenu:
                    for (auto& btn : staffMenuButtons) {
                        if (btn.contains(mousePos)) {
                            string label = btn.text.getString();
                            if (label == "Create Order") {
                                state = AppState::StaffCreateOrder;
                                rebuildUI();
                            }
                            else if (label == "View Menu") {
                                menuTable = new Table(font, Vector2f(50, 100));
                                menuTable->setHeaders({ "ID", "Name", "Price", "Available" }, { 100, 200, 100, 100 });
                                for (int i = 0; i < menuCount; i++) {
                                    menuTable->addRow({
                                        menu[i].getId(),
                                        menu[i].getName(),
                                        "$" + to_string(menu[i].getPrice()),
                                        menu[i].isAvailable() ? "Yes" : "No"
                                        });
                                }
                                staffMenuButtons.clear();
                                staffMenuButtons.emplace_back(font, "Back", Vector2f(412, 650), Vector2f(200, 40));
                            }
                            else if (label == "Back") {
                                state = AppState::RoleSelection;
                                rebuildUI();
                            }
                        }
                    }
                    break;

                case AppState::StaffCreateOrder:
                    if (menuTable && menuTable->rows.size() > 0) {
                        float tableY = menuTable->position.y + menuTable->rowHeight;
                        for (size_t i = 0; i < menuTable->rows.size(); i++) {
                            float cellY = tableY + (i * menuTable->rowHeight);
                            RectangleShape addCell(Vector2f(menuTable->columnWidths[3], menuTable->rowHeight));
                            addCell.setPosition(menuTable->position.x +
                                menuTable->columnWidths[0] +
                                menuTable->columnWidths[1] +
                                menuTable->columnWidths[2],
                                cellY);

                            if (addCell.getGlobalBounds().contains(mousePos)) {
                                string itemId = menuTable->rows[i][0].getString();
                                int qty = 1;

                                for (int j = 0; j < menuCount; j++) {
                                    if (menu[j].getId() == itemId) {
                                        if (currentOrder->addItem(menu[j], qty)) {
                                            messageDisplay.show("Item added to order: " + menu[j].getName(), Color::Green);
                                        }
                                        else {
                                            messageDisplay.show("Failed to add item", ACCENT_COLOR);
                                        }
                                        break;
                                    }
                                }
                            }
                        }
                    }

                    for (auto& btn : staffMenuButtons) {
                        if (btn.contains(mousePos)) {
                            string label = btn.text.getString();
                            if (label == "Checkout" && currentOrder) {
                                if (orderCount < MAX_ORDERS) {
                                    currentOrder->checkout();
                                    allOrders[orderCount++] = *currentOrder;

                                    ofstream ofs("orders.txt", ios::app);
                                    if (ofs.is_open()) {
                                        ofs << currentOrder->getOrderId() << "|"
                                            << "Staff" << "|"
                                            << currentOrder->computeTotal() << "\n";
                                        ofs.close();
                                    }

                                    double total = Billing::applyDiscounts(*currentOrder, 0.0);
                                    Billing::generateReceipt(*currentOrder, total, "Staff");

                                    messageDisplay.show("Order completed! Total: $" + to_string(total), Color::Green);
                                    delete currentOrder;
                                    currentOrder = nullptr;
                                    orderCreated = false;
                                    state = AppState::StaffMenu;
                                    rebuildUI();
                                }
                            }
                            else if (label == "Cancel") {
                                delete currentOrder;
                                currentOrder = nullptr;
                                orderCreated = false;
                                state = AppState::StaffMenu;
                                rebuildUI();
                            }
                        }
                    }
                    break;

                case AppState::CustomerMenu:
                    for (auto& btn : customerMenuButtons) {
                        if (btn.contains(mousePos)) {
                            string label = btn.text.getString();
                            if (label == "View Menu") {
                                state = AppState::CustomerViewMenu;
                                rebuildUI();
                            }
                            else if (label == "Place Order") {
                                state = AppState::CustomerPlaceOrder;
                                rebuildUI();
                            }
                            else if (label == "Give Feedback") {
                                state = AppState::CustomerGiveFeedback;
                                rebuildUI();
                            }
                            else if (label == "Back") {
                                state = AppState::RoleSelection;
                                rebuildUI();
                            }
                        }
                    }
                    break;

                case AppState::CustomerViewMenu:
                    for (auto& btn : customerMenuButtons) {
                        if (btn.contains(mousePos)) {
                            if (btn.text.getString() == "Back") {
                                state = AppState::CustomerMenu;
                                rebuildUI();
                            }
                        }
                    }
                    break;

                case AppState::CustomerPlaceOrder:
                    if (menuTable && menuTable->rows.size() > 0) {
                        float tableY = menuTable->position.y + menuTable->rowHeight;
                        for (size_t i = 0; i < menuTable->rows.size(); i++) {
                            float cellY = tableY + (i * menuTable->rowHeight);
                            RectangleShape addCell(Vector2f(menuTable->columnWidths[3], menuTable->rowHeight));
                            addCell.setPosition(menuTable->position.x +
                                menuTable->columnWidths[0] +
                                menuTable->columnWidths[1] +
                                menuTable->columnWidths[2],
                                cellY);

                            if (addCell.getGlobalBounds().contains(mousePos)) {
                                string itemId = menuTable->rows[i][0].getString();
                                if (inputBoxes.size() > 0) {
                                    inputBoxes[0].input = itemId;
                                    inputBoxes[0].inputText.setString(itemId);
                                }
                            }
                        }
                    }

                    for (auto& btn : customerMenuButtons) {
                        if (btn.contains(mousePos)) {
                            string label = btn.text.getString();
                            if (label == "Add Item" && inputBoxes.size() >= 2) {
                                string itemId = inputBoxes[0].input;
                                string qtyStr = inputBoxes[1].input;

                                if (!itemId.empty() && !qtyStr.empty()) {
                                    try {
                                        int qty = stoi(qtyStr);
                                        for (int i = 0; i < menuCount; i++) {
                                            if (menu[i].getId() == itemId) {
                                                if (!currentOrder) {
                                                    currentOrder = new Order();
                                                    currentOrder->setOrderId("ORD" + to_string(time(nullptr)));
                                                }

                                                if (currentOrder->addItem(menu[i], qty)) {
                                                    messageDisplay.show("Added: " + menu[i].getName() + " x" + qtyStr, Color::Green);
                                                    inputBoxes[0].clear();
                                                    inputBoxes[1].clear();
                                                }
                                                else {
                                                    messageDisplay.show("Failed to add item", ACCENT_COLOR);
                                                }
                                                break;
                                            }
                                        }
                                    }
                                    catch (...) {
                                        messageDisplay.show("Invalid quantity", ACCENT_COLOR);
                                    }
                                }
                            }
                            else if (label == "Place Order" && currentOrder) {
                                if (customerUser->placeOrder(*currentOrder)) {
                                    double total = Billing::applyDiscounts(*currentOrder, 0.0);
                                    Billing::generateReceipt(*currentOrder, total, customerUser->getUsername());

                                    if (orderCount < MAX_ORDERS) {
                                        allOrders[orderCount++] = *currentOrder;
                                    }

                                    messageDisplay.show("Order placed! Total: $" + to_string(total), Color::Green);
                                    delete currentOrder;
                                    currentOrder = nullptr;
                                    state = AppState::CustomerMenu;
                                    rebuildUI();
                                }
                                else {
                                    messageDisplay.show("Order placement failed", ACCENT_COLOR);
                                }
                            }
                            else if (label == "Back") {
                                if (currentOrder) {
                                    delete currentOrder;
                                    currentOrder = nullptr;
                                }
                                state = AppState::CustomerMenu;
                                rebuildUI();
                            }
                        }
                    }
                    break;

                case AppState::CustomerGiveFeedback:
                    for (auto& btn : customerMenuButtons) {
                        if (btn.contains(mousePos)) {
                            string label = btn.text.getString();
                            if (label == "Submit" && inputBoxes.size() >= 3) {
                                string name = inputBoxes[0].input;
                                string comment = inputBoxes[1].input;
                                string ratingStr = inputBoxes[2].input;

                                if (!name.empty() && !comment.empty() && !ratingStr.empty()) {
                                    try {
                                        int rating = stoi(ratingStr);
                                        if (rating >= 1 && rating <= 5) {
                                            Customer tempCustomer(name, "");
                                            if (tempCustomer.giveFeedback(*feedback, comment, rating)) {
                                                messageDisplay.show("Feedback submitted! Thank you!", Color::Green);
                                                inputBoxes[0].clear();
                                                inputBoxes[1].clear();
                                                inputBoxes[2].clear();
                                            }
                                            else {
                                                messageDisplay.show("Failed to submit feedback", ACCENT_COLOR);
                                            }
                                        }
                                        else {
                                            messageDisplay.show("Rating must be 1-5", ACCENT_COLOR);
                                        }
                                    }
                                    catch (...) {
                                        messageDisplay.show("Invalid rating", ACCENT_COLOR);
                                    }
                                }
                            }
                            else if (label == "Back") {
                                state = AppState::CustomerMenu;
                                rebuildUI();
                            }
                        }
                    }
                    break;

                case AppState::ViewInventory:
                    for (auto& btn : roleButtons) {
                        if (btn.contains(mousePos)) {
                            if (btn.text.getString() == "Back") {
                                state = AppState::RoleSelection;
                                rebuildUI();
                            }
                        }
                    }
                    break;

                case AppState::Reports:
                    for (auto& btn : roleButtons) {
                        if (btn.contains(mousePos)) {
                            string label = btn.text.getString();
                            if (label == "Daily Sales") {
                                double totalSales = 0.0;
                                ifstream ifs("orders.txt");
                                string line;
                                while (getline(ifs, line)) {
                                    stringstream ss(line);
                                    string orderId, customer, totalStr;
                                    getline(ss, orderId, '|');
                                    getline(ss, customer, '|');
                                    getline(ss, totalStr);
                                    totalSales += stod(totalStr);
                                }
                                ifs.close();

                                messageDisplay.show("Daily Sales Total: $" + to_string(totalSales), Color::Green);
                            }
                            else if (label == "Best Sellers") {
                                messageDisplay.show("Feature coming soon!", Color::Green);
                            }
                            else if (label == "Low Stock") {
                                inventory->lowStockAlerts(10);
                                messageDisplay.show("Low stock items shown in console", Color::Green);
                            }
                            else if (label == "Back") {
                                state = AppState::RoleSelection;
                                rebuildUI();
                            }
                        }
                    }
                    break;
                }
            }
        }

        // Update message display
        messageDisplay.update();

        // Clear window
        window.clear(BACKGROUND_COLOR);

        // Draw title based on state
        Text title("", font, 32);
        title.setFillColor(SECONDARY_COLOR);

        switch (state) {
        case AppState::RoleSelection:
            title.setString("Cafe Management System");
            break;
        case AppState::AdminLogin:
            title.setString("Admin Login");
            break;
        case AppState::AdminMenu:
            title.setString("Admin Dashboard");
            break;
        case AppState::StaffLogin:
            title.setString("Staff Login");
            break;
        case AppState::StaffMenu:
            title.setString("Staff Panel");
            break;
        case AppState::CustomerMenu:
            title.setString("Customer Menu");
            break;
        case AppState::ViewInventory:
            title.setString("Inventory");
            break;
        case AppState::Reports:
            title.setString("Reports");
            break;
        default:
            title.setString("Cafe Management System");
        }

        FloatRect titleBounds = title.getLocalBounds();
        title.setOrigin(titleBounds.width / 2, 0);
        title.setPosition(512, 30);
        window.draw(title);

        // Draw UI elements based on state
        for (auto& btn : roleButtons) btn.draw(window);
        for (auto& btn : adminMenuButtons) btn.draw(window);
        for (auto& btn : staffMenuButtons) btn.draw(window);
        for (auto& btn : customerMenuButtons) btn.draw(window);
        for (auto& inputBox : inputBoxes) inputBox.draw(window);

        if (menuTable) menuTable->draw(window);
        if (inventoryTable) inventoryTable->draw(window);
        if (feedbackTable) feedbackTable->draw(window);

        // Draw message display
        messageDisplay.draw(window);

        // Display window
        window.display();
    }

    // Cleanup
    cleanupBackend();
    if (currentOrder) delete currentOrder;

    return 0;
}