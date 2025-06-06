#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <limits>
using namespace std;

class Room {
protected:
    int roomNumber;
    double price;
    bool isAvailable;

public:
    Room(int num, double p) : roomNumber(num), price(p), isAvailable(true) {}
    virtual void displayInfo() const = 0;
    virtual double calculateBill(int days) const = 0;

    bool getAvailability() const { return isAvailable; }
    void bookRoom() {
        if (isAvailable) isAvailable = false;
        else throw runtime_error("Room already booked.");
    }
    void vacateRoom() { isAvailable = true; }
    int getRoomNumber() const { return roomNumber; }
};

class DeluxeRoom : public Room {
public:
    DeluxeRoom(int num) : Room(num, 150.0) {}
    void displayInfo() const override {
        cout << "Deluxe Room " << roomNumber << ", Price: $" << price
             << ", Available: " << (isAvailable ? "Yes" : "No") << endl;
    }
    double calculateBill(int days) const override {
        return days * price;
    }
};

class SuiteRoom : public Room {
public:
    SuiteRoom(int num) : Room(num, 300.0) {}
    void displayInfo() const override {
        cout << "Suite Room " << roomNumber << ", Price: $" << price
             << ", Available: " << (isAvailable ? "Yes" : "No") << endl;
    }
    double calculateBill(int days) const override {
        return days * price + 100;
    }
};

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

class User {
private:
    string username;
    string password;
    vector<int> bookedRoomNumbers;

public:
    User(string u, string p) : username(u), password(p) {}
    string getUsername() const { return username; }
    bool checkPassword(const string& p) const { return p == password; }

    void addBooking(int roomNum) { bookedRoomNumbers.push_back(roomNum); }
    void removeBooking(int roomNum) {
        bookedRoomNumbers.erase(remove(bookedRoomNumbers.begin(), bookedRoomNumbers.end(), roomNum), bookedRoomNumbers.end());
    }

    void showBookings() const {
        if (bookedRoomNumbers.empty()) {
            cout << "No rooms currently booked.\n";
        } else {
            cout << "Booked rooms: ";
            for (int num : bookedRoomNumbers) {
                cout << num << " ";
            }
            cout << endl;
        }
    }

    bool hasBooking(int roomNum) const {
        return find(bookedRoomNumbers.begin(), bookedRoomNumbers.end(), roomNum) != bookedRoomNumbers.end();
    }
};

class HotelSystem {
private:
    unordered_map<int, shared_ptr<Room>> rooms;
    vector<User> users;
    shared_ptr<BillingStrategy> billingStrategy;
    User* currentUser = nullptr;

public:
    HotelSystem() {
        for (int i = 1; i <= 3; ++i) rooms[i] = make_shared<DeluxeRoom>(i);
        for (int i = 4; i <= 5; ++i) rooms[i] = make_shared<SuiteRoom>(i);
        billingStrategy = make_shared<StandardBilling>();
    }

    void signup(const string& u, const string& p) {
        for (const auto& user : users)
            if (user.getUsername() == u)
                throw runtime_error("Username already exists.");
        users.emplace_back(u, p);
        cout << "Signup successful!\n";
    }

    bool login(const string& u, const string& p) {
        for (auto& user : users) {
            if (user.getUsername() == u && user.checkPassword(p)) {
                cout << "Login successful!\n";
                currentUser = &user;
                return true;
            }
        }
        cout << "Login failed!\n";
        return false;
    }

    void showAvailableRooms() const {
        for (const auto& pair : rooms) {
            if (pair.second->getAvailability())
                pair.second->displayInfo();
        }
    }

    void bookRoom(int roomNum) {
        auto it = rooms.find(roomNum);
        if (it != rooms.end()) {
            if (!currentUser) throw runtime_error("No user logged in.");
            it->second->bookRoom();
            currentUser->addBooking(roomNum);
            cout << "Room " << roomNum << " booked successfully.\n";
        } else {
            throw runtime_error("Room number not found.");
        }
    }

    void cancelBooking(int roomNum) {
        auto it = rooms.find(roomNum);
        if (it != rooms.end()) {
            if (!currentUser) throw runtime_error("No user logged in.");
            if (!currentUser->hasBooking(roomNum)) throw runtime_error("You haven't booked this room.");
            it->second->vacateRoom();
            currentUser->removeBooking(roomNum);
            cout << "Booking for Room " << roomNum << " cancelled.\n";
        } else {
            throw runtime_error("Room number not found.");
        }
    }

    void calculateBill(int roomNum, int days) {
        auto it = rooms.find(roomNum);
        if (it != rooms.end()) {
            cout << "Total Bill: $" << billingStrategy->getBill(it->second.get(), days) << endl;
        } else {
            throw runtime_error("Room not found.");
        }
    }

    void showUserBookings() const {
        if (currentUser)
            currentUser->showBookings();
        else
            cout << "No user logged in.\n";
    }
};

void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int main() {
    HotelSystem system;
    string username, password;

    cout << "===== Hotel Reservation System =====\n";
    cout << "1. Signup\n2. Login\nChoose option: ";
    int choice;
    cin >> choice;
    clearInput();
    cout << "Username: "; getline(cin, username);
    cout << "Password: "; getline(cin, password);

    try {
        if (choice == 1) {
            system.signup(username, password);
            cout << "Now please log in with the same credentials.\n";
            cout << "Username: "; getline(cin, username);
            cout << "Password: "; getline(cin, password);
        }

        if (!system.login(username, password)) return 0;

        while (true) {
            cout << "\n1. Show Available Rooms\n2. Book Room\n3. Calculate Bill\n4. View My Bookings\n5. Cancel Booking\n6. Exit\nChoose option: ";
            if (!(cin >> choice)) {
                clearInput();
                cout << "Invalid input. Please enter a number.\n";
                continue;
            }

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
                    system.showUserBookings();
                } else if (choice == 5) {
                    int roomNum;
                    cout << "Enter Room Number to Cancel: ";
                    cin >> roomNum;
                    system.cancelBooking(roomNum);
                } else if (choice == 6) {
                    break;
                } else {
                    cout << "Invalid choice.\n";
                }
            } catch (const exception& e) {
                cerr << "Error: " << e.what() << endl;
            }
        }

    } catch (const exception& e) {
        cerr << "Fatal Error: " << e.what() << endl;
    }

    return 0;
}
