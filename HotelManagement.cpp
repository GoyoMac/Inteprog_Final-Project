#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <stdexcept>
using namespace std;

class Room {
protected:
    int roomNumber;
    double price;
    bool isAvailable;

public:
    Room(int num, double p) : roomNumber(num), price(p), isAvailable(true) {}
    virtual void displayInfo() const = 0; // Pure virtual (Abstraction + Polymorphism)
    virtual double calculateBill(int days) const = 0;

    bool getAvailability() const { return isAvailable; }
    void bookRoom() { if (isAvailable) isAvailable = false; else throw runtime_error("Room already booked."); }
    void vacateRoom() { isAvailable = true; }
    int getRoomNumber() const { return roomNumber; }
};

class DeluxeRoom : public Room {
public:
    DeluxeRoom(int num) : Room(num, 150.0) {}
    void displayInfo() const override {
        cout << "Deluxe Room " << roomNumber << ", Price: $" << price << ", Available: " << (isAvailable ? "Yes" : "No") << endl;
    }
    double calculateBill(int days) const override {
        return days * price;
    }
};

class SuiteRoom : public Room {
public:
    SuiteRoom(int num) : Room(num, 300.0) {}
    void displayInfo() const override {
        cout << "Suite Room " << roomNumber << ", Price: $" << price << ", Available: " << (isAvailable ? "Yes" : "No") << endl;
    }
    double calculateBill(int days) const override {
        return days * price + 100; // extra service fee
    }
};

// Strategy Pattern for Billing
class BillingStrategy {
public:
    virtual double getBill(const Room* room, int days) const = 0;
};

class StandardBilling : public BillingStrategy {
public:
    double getBill(const Room* room, int days) const override {
        return room->calculateBill(days);
    }
};

// User class (Encapsulation)
class User {
private:
    string username;
    string password;

public:
    User(string u, string p) : username(u), password(p) {}
    string getUsername() const { return username; }
    bool checkPassword(const string& p) const { return p == password; }
};

// Hotel Management (Abstraction + Composition)
class HotelSystem {
private:
    vector<shared_ptr<Room>> rooms;
    vector<User> users;
    shared_ptr<BillingStrategy> billingStrategy;

public:
    HotelSystem() {
        // Initialize with default rooms
        for (int i = 1; i <= 3; ++i) rooms.push_back(make_shared<DeluxeRoom>(i));
        for (int i = 4; i <= 5; ++i) rooms.push_back(make_shared<SuiteRoom>(i));
        billingStrategy = make_shared<StandardBilling>();
    }

    void signup(const string& u, const string& p) {
        users.emplace_back(u, p);
        cout << "Signup successful!\n";
    }

    bool login(const string& u, const string& p) {
        for (const auto& user : users) {
            if (user.getUsername() == u && user.checkPassword(p)) {
                cout << "Login successful!\n";
                return true;
            }
        }
        cout << "Login failed!\n";
        return false;
    }

    void showAvailableRooms() const {
        for (const auto& r : rooms) {
            if (r->getAvailability()) r->displayInfo();
        }
    }

    void bookRoom(int roomNum) {
        for (const auto& r : rooms) {
            if (r->getRoomNumber() == roomNum) {
                r->bookRoom();
                cout << "Room " << roomNum << " booked successfully.\n";
                return;
            }
        }
        throw runtime_error("Room number not found.");
    }

    void calculateBill(int roomNum, int days) {
        for (const auto& r : rooms) {
            if (r->getRoomNumber() == roomNum) {
                cout << "Total Bill: $" << billingStrategy->getBill(r.get(), days) << endl;
                return;
            }
        }
        throw runtime_error("Room not found.");
    }
};

int main() {
    HotelSystem system;
    string username, password;

    cout << "===== Hotel Reservation System =====\n";
    cout << "1. Signup\n2. Login\nChoose option: ";
    int choice;
    cin >> choice;
    cout << "Username: "; cin >> username;
    cout << "Password: "; cin >> password;

    if (choice == 1) system.signup(username, password);
    if (!system.login(username, password)) return 0;

    while (true) {
        cout << "\n1. Show Available Rooms\n2. Book Room\n3. Calculate Bill\n4. Exit\nChoose option: ";
        cin >> choice;
        try {
            if (choice == 1) {
                system.showAvailableRooms();
            } else if (choice == 2) {
                int roomNum;
                cout << "Enter Room Number to Book: ";
                cin >> roomNum;
                system.bookRoom(roomNum);
            } else if (choice == 3) {
                int roomNum, days;
                cout << "Enter Room Number: "; cin >> roomNum;
                cout << "Enter Number of Days: "; cin >> days;
                system.calculateBill(roomNum, days);
            } else if (choice == 4) {
                break;
            } else {
                cout << "Invalid choice.\n";
            }
        } catch (const exception& e) {
            cerr << "Error: " << e.what() << endl;
        }
    }

    return 0;
}
