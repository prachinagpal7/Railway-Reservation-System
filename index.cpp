#include <iostream>
#include "db.h"
#include<string>
#include<memory>
#include <limits>
#include <stdexcept>
#include <cstdlib>
#include "config.h"
using namespace std;

struct Train{
    string trainName, source, destination, time;
    int trainNo, fare, seats;
};

bool readInt(int& value) {
    if (cin >> value) return true;
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return false;
}

class userSystem{
    private:
        string name;
        int age;
        char gender;
        
    public:
        void userRegister(){
            string username, gender, password;
            int userAge;
            cout << endl<<endl<<" ====== Railway Reservation System! ====== "<<endl;
            cout <<endl<< " Please enter your details to create an account."<<endl<<endl;
            cout << "Enter Username: ";
            cin >> username;
            cout << "Enter Age: ";
            if (!readInt(userAge)) {
                cout << "Registration failed: age must be a number." << endl;
                return;
            }
            cout << "Enter Gender (Male/Female/Other): ";
            cin >> gender;
            if(gender != "Male" && gender != "Female" &&
                gender != "Other" && gender != "other") {
                cout << "Please enter Male, Female, or Other for gender." << endl;
                return;
            }
            cout << "Enter Password: ";
            cin >> password;
            if(userAge <= 13) {
                cout << "Registration failed: age must be above 13." << endl;
                return;
            }
            try {
                unique_ptr<sql::Connection> con(connectToDatabase());
                unique_ptr<sql::PreparedStatement> pstmt(
                    con->prepareStatement("INSERT INTO users (username, age, gender, password) VALUES (?, ?, ?, ?)")
                );
                pstmt->setString(1, username);
                pstmt->setInt(2, userAge);
                pstmt->setString(3, gender);
                pstmt->setString(4, password);
                pstmt->executeUpdate();
                cout << endl << " ------ Registration Successful! You can now login. ------"<<endl<<endl;
            }
            catch (sql::SQLException& e) {
                cerr << "MySQL error during registration: " << e.what() << endl;
            }
        }
        
        bool userLogin(){
            string userInput, passInput;
            cout << endl << " ------ User Login ------" << endl;
            cout << "Enter Username: ";
            cin >> userInput;
            cout << "Enter Password: ";
            cin >> passInput;
            try {
                unique_ptr<sql::Connection> con(connectToDatabase());
                unique_ptr<sql::PreparedStatement> pstmt(
                    con->prepareStatement("SELECT username, age, gender FROM users WHERE username = ? AND password = ?")
                );
                pstmt->setString(1, userInput);
                pstmt->setString(2, passInput);
                unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
                if(res->next()) {
                    name = res->getString("username");
                    age = res->getInt("age");
                    gender = res->getString("gender")[0];
                    cout << endl << " ------ Login Successful! Welcome back, " << name << "! ------" << endl << endl;
                    return true;
                }
                cout << " Login Failed! Username or Password is incorrect." << endl << endl;
                return false;
            } catch (sql::SQLException& e) {
                cerr << "MySQL error during login: " << e.what() << endl;
                return false;
            }
        }  
        
        string getUsername() {
            return name;
        }
};

class adminPanel{
    public:
        void adminLogin(){
            string username, password;
            cout << endl << "Enter Admin Username: ";
            cin >> username;
            cout << "Enter Admin Password: ";
            cin >> password;
            const char* adminUser = getenv("RAILWAY_ADMIN_USER");
            const char* adminPassword = getenv("RAILWAY_ADMIN_PASSWORD");
            if (adminUser == nullptr || adminPassword == nullptr) {
                cout << "Admin credentials are not configured." << endl;
                return;
            }
            if (username == adminUser && password == adminPassword) {
                cout << "Admin login successful!" << endl;
                adminMenu();
            } else {
                cout << "Invalid admin credentials." << endl;
            }
        }

        void adminMenu(){
            int adminChoice = 0;
            do {
                cout<<endl<<" ====== Admin Menu ====== "<<endl;
                cout<<"1. Add Train "<<endl;
                cout<<"2. View Trains "<<endl;
                cout<<"3. Search Train "<<endl;
                cout<<"4. Update Train "<<endl;
                cout<<"5. Delete Train "<<endl;
                cout<<"6. Exit Admin Menu "<<endl;
                cout<<"7. Exit "<<endl;
                cout<<"Enter Choice: ";
                if (!readInt(adminChoice)) {
                    cout << "Invalid input. Please enter a number." << endl;
                    continue;
                }
                switch(adminChoice){
                    case 1: addTrain(); break;
                    case 2: viewTrain(); break;
                    case 3: searchTrain(); break;
                    case 4: updateTrain(); break;
                    case 5: deleteTrain(); break;
                    case 6: cout <<endl<< " --- Exiting Admin Menu... ---" << endl; break;
                    case 7: cout<<endl<<" ---Thankyou. Visit Again. --- "<<endl; return;
                    default: cout << " --- Invalid Choice. --- " << endl;
                }
            } while (adminChoice != 6 && adminChoice != 7);
        }
        
        void addTrain(){
            Train t;
            cout << endl << " --- Add Train --- " << endl;
            cout << "Enter Train Name (without spaces): ";
            cin >> t.trainName;
            cout << "Enter Train Number: ";
            if(!readInt(t.trainNo) || t.trainNo <= 0) {
                cout << "Train number must be a positive integer." << endl;
                return;
            }
            cout << "Enter Train Source: ";
            cin >> t.source;
            cout << "Enter Train Destination: ";
            cin >> t.destination;
            cout << "Enter Fare (per Ticket): ";
            if(!readInt(t.fare) || t.fare < 0) {
                cout << "Fare must be zero or greater." << endl;
                return;
            }
            cout << "Enter Available Seats: ";
            if (!readInt(t.seats) || t.seats <= 0) {
                cout << "Available seats must be greater than zero." << endl;
                return;
            }
            cout << "Enter Train Time (e.g., 09:45PM): ";
            cin >> t.time;
            try {
                unique_ptr<sql::Connection> con(connectToDatabase());
                unique_ptr<sql::PreparedStatement> pstmt(
                    con->prepareStatement("INSERT INTO trains (train_no, train_name, source, destination, departure_time, fare, available_seats) VALUES (?, ?, ?, ?, ?, ?, ?)")
                );
                pstmt->setInt(1, t.trainNo);
                pstmt->setString(2, t.trainName);
                pstmt->setString(3, t.source);
                pstmt->setString(4, t.destination);
                pstmt->setString(5, t.time);
                pstmt->setInt(6, t.fare);
                pstmt->setInt(7, t.seats);
                pstmt->executeUpdate();
                cout << endl << "Train added to MySQL successfully!" << endl;
            }
            catch (sql::SQLException& e) {
                cerr << "MySQL error while adding train: " << e.what() << endl;
            }
        }

        void viewTrain(){
            try {
                unique_ptr<sql::Connection> con(connectToDatabase());
                unique_ptr<sql::Statement> stmt(con->createStatement());
                unique_ptr<sql::ResultSet> res(
                    stmt->executeQuery("SELECT train_name, train_no, source, destination, fare, available_seats, departure_time FROM trains")
                );
                cout<<endl<<" --- Train List --- "<<endl;
                bool found = false;
                while(res->next()){
                    found = true;
                    cout << "Train Name: " << res->getString("train_name") << endl;
                    cout << "Train Number: " << res->getInt("train_no") << endl;
                    cout << "Train Source: " << res->getString("source") << endl;
                    cout << "Train Destination: " << res->getString("destination") << endl;
                    cout << "Train Fare (per Ticket): " << res->getInt("fare") << endl;
                    cout << "Available Seats: " << res->getInt("available_seats") << endl;
                    cout << "Train Timing: " << res->getString("departure_time") << endl;
                    cout << "-----------------------------" << endl;
                }
                if(!found) {
                    cout << "No trains available in the database." << endl;
                }
            }
            catch(sql::SQLException& e) {
                cerr << "MySQL error while viewing trains: " << e.what() << endl;
            }
        }

        void searchTrain(){
            int searchTrainNo;
            cout << endl << "Enter Train Number to Search: ";
            if (!readInt(searchTrainNo) || searchTrainNo <= 0) {
                cout << "Please enter a valid positive train number." << endl;
                return;
            }
            try {
                unique_ptr<sql::Connection> con(connectToDatabase());
                unique_ptr<sql::PreparedStatement> pstmt(
                    con->prepareStatement("SELECT train_name, train_no, source, destination, fare, available_seats, departure_time FROM trains WHERE train_no = ?")
                );
                pstmt->setInt(1, searchTrainNo);
                unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
                if(res->next()) {
                    cout << endl << " --- Train Found --- " << endl;
                    cout << "Train Name: " << res->getString("train_name") << endl;
                    cout << "Train Number: " << res->getInt("train_no") << endl;
                    cout << "Train Source: " << res->getString("source") << endl;
                    cout << "Train Destination: " << res->getString("destination") << endl;
                    cout << "Train Fare (per Ticket): " << res->getInt("fare") << endl;
                    cout << "Available Seats: " << res->getInt("available_seats") << endl;
                    cout << "Train Timing: " << res->getString("departure_time") << endl;
                } else {
                    cout << "No train found with number " << searchTrainNo << "." << endl;
                }
            }
            catch (sql::SQLException& e) {
                cerr << "MySQL error while searching train: " << e.what() << endl;
            }
        }

        void updateTrain(){
            int trainNo, fare, seats;
            string source, destination, time;
            cout << endl << "Enter Train Number to Update: ";
            if (!readInt(trainNo) || trainNo <= 0) {
                cout << "Please enter a valid positive train number." << endl;
                return;
            }
            cout << "Enter New Train Source: ";
            cin >> source;
            cout << "Enter New Train Destination: ";
            cin >> destination;
            cout << "Enter New Train Fare: ";
            if (!readInt(fare) || fare < 0) {
                cout << "Fare must be zero or greater." << endl;
                return;
            }
            cout << "Enter New Available Seats: ";
            if (!readInt(seats) || seats < 0) {
                cout << "Available seats cannot be negative." << endl;
                return;
            }
            cout << "Enter New Train Time: ";
            cin >> time;
            try {
                unique_ptr<sql::Connection> con(connectToDatabase());
                unique_ptr<sql::PreparedStatement> pstmt(
                    con->prepareStatement("UPDATE trains SET source = ?, destination = ?, fare = ?, available_seats = ?, departure_time = ? WHERE train_no = ?")
                );
                pstmt->setString(1, source);
                pstmt->setString(2, destination);
                pstmt->setInt(3, fare);
                pstmt->setInt(4, seats);
                pstmt->setString(5, time);
                pstmt->setInt(6, trainNo);
                int rowsUpdated = pstmt->executeUpdate();
                if (rowsUpdated > 0) {
                    cout << "Train updated in MySQL successfully!" << endl;
                } else {
                    cout << "No train found with number " << trainNo << "." << endl;
                }
            }
            catch (sql::SQLException& e) {
                cerr << "MySQL error while updating train: " << e.what() << endl;
            }
        }
        
        void deleteTrain(){
            int trainNo;
            cout << endl << "Enter Train Number to Delete: ";
            if (!readInt(trainNo) || trainNo <= 0) {
                cout << "Please enter a valid positive train number." << endl;
                return;
            }
            try {
                unique_ptr<sql::Connection> con(connectToDatabase());
                unique_ptr<sql::PreparedStatement> pstmt(
                    con->prepareStatement("DELETE FROM trains WHERE train_no = ?")
                );
                pstmt->setInt(1, trainNo);
                int rowsDeleted = pstmt->executeUpdate();
                if (rowsDeleted > 0) {
                    cout << "Train deleted from MySQL successfully!" << endl;
                } else {
                    cout << "No train found with number " << trainNo << "." << endl;
                }
            }
            catch (sql::SQLException& e) {
                cerr << "MySQL error while deleting train: " << e.what() << endl;
                cerr << "The train may be referenced by existing bookings." << endl;
            }
        }
};

class userPanel{
    public:
        string username;

    public:   
        void viewAvailableTrains() {
            try {
                unique_ptr<sql::Connection> con(connectToDatabase());
                unique_ptr<sql::Statement> stmt(con->createStatement());
                unique_ptr<sql::ResultSet> res(
                    stmt->executeQuery("SELECT train_name, train_no, source, destination, fare, available_seats, departure_time FROM trains")
                );
                cout << endl << " --- Available Trains --- " << endl;
                bool found = false;
                while(res->next()) {
                    found = true;
                    cout << "Train Name: " << res->getString("train_name") << " (" << res->getInt("train_no") << ") | " << res->getString("source") << " to " << res->getString("destination") << " | Ticket: Rs " << res->getInt("fare") << " | Seats: " << res->getInt("available_seats") << " | Time: " << res->getString("departure_time")<< endl;
                }
                if(!found) {
                    cout << "No trains available." << endl;
                }
            }
            catch (sql::SQLException& e) {
                cerr << "MySQL error while viewing available trains: " << e.what() << endl;
            }
        }

        void bookTicket(){
            int trainNo, tickets;
            cout << endl << "Enter Train Number to book: ";
            if (!readInt(trainNo) || trainNo <= 0) {
                cout << "Please enter a valid positive train number." << endl;
                return;
            }
            cout << "Enter number of tickets: ";
            if (!readInt(tickets) || tickets <= 0) {
                cout << "Ticket count must be a positive integer." << endl;
                return;
            }
            try{
                unique_ptr<sql::Connection> con(connectToDatabase());
                con->setAutoCommit(false);
                try {
                    unique_ptr<sql::PreparedStatement> findTrain(
                        con->prepareStatement("SELECT available_seats, fare FROM trains WHERE train_no = ? FOR UPDATE")
                    );
                    findTrain->setInt(1, trainNo);
                    unique_ptr<sql::ResultSet> res(findTrain->executeQuery());
                    if(!res->next()) {
                        con->rollback();
                        cout <<endl<<"No train found."<<endl<<endl;
                        return;
                    }
                    int seats = res->getInt("available_seats");
                    int fare = res->getInt("fare");
                    if(tickets > seats) {
                        con->rollback();
                        cout << "Not enough seats available." << endl;
                        return;
                    }
                    unique_ptr<sql::PreparedStatement> findUser(
                        con->prepareStatement("SELECT user_id FROM users WHERE username = ?")
                    );
                    findUser->setString(1, username);
                    unique_ptr<sql::ResultSet> userRes(findUser->executeQuery());
                    if(!userRes->next()) {
                        con->rollback();
                        cout << "User account not found." << endl;
                        return;
                    }
                    int userId = userRes->getInt("user_id");
                    unique_ptr<sql::PreparedStatement> updateSeats(
                        con->prepareStatement("UPDATE trains SET available_seats = available_seats - ? WHERE train_no = ?")
                    );
                    updateSeats->setInt(1, tickets);
                    updateSeats->setInt(2, trainNo);
                    if (updateSeats->executeUpdate() != 1) {
                        throw runtime_error("Could not update available seats.");
                    }
                    unique_ptr<sql::PreparedStatement> insertBooking(
                        con->prepareStatement(
                            "INSERT INTO bookings (user_id, train_no, tickets, status) VALUES (?, ?, ?, 'ACTIVE')"
                        )
                    );
                    insertBooking->setInt(1, userId);
                    insertBooking->setInt(2, trainNo);
                    insertBooking->setInt(3, tickets);
                    if (insertBooking->executeUpdate() != 1) {
                        throw runtime_error("Could not create the booking.");
                    }
                    con->commit();
                    cout << "Train booked successfully!" << endl;
                    cout << "Tickets booked: " << tickets << endl;
                    cout << "Total fare: Rs " << tickets * fare << endl;
                }
                catch(...) {
                    con->rollback();
                    throw;
                }
            } catch (sql::SQLException& e) {
                cerr << "MySQL error while booking: " << e.what() << endl;
            }
        }

        void cancelTicket(){
            int bookingId;
            cout << "Enter Booking ID to cancel: ";
            if (!readInt(bookingId) || bookingId <= 0) {
                cout << "Please enter a valid positive booking ID." << endl;
                return;
            }
            try {
                unique_ptr<sql::Connection> con(connectToDatabase());
                con->setAutoCommit(false);
                try {
                    unique_ptr<sql::PreparedStatement> findBooking(
                        con->prepareStatement("SELECT b.train_no, b.tickets FROM bookings b JOIN users u ON b.user_id = u.user_id WHERE b.booking_id = ? AND u.username = ? AND b.status = 'ACTIVE' FOR UPDATE")
                    );
                    findBooking->setInt(1, bookingId);
                    findBooking->setString(2, username);
                    unique_ptr<sql::ResultSet> res(findBooking->executeQuery());
                    if (!res->next()) {
                        con->rollback();
                        cout << "No active booking found with that ID." << endl;
                        return;
                    }
                    int trainNo = res->getInt("train_no");
                    int tickets = res->getInt("tickets");
                    unique_ptr<sql::PreparedStatement> restoreSeats(
                        con->prepareStatement("UPDATE trains SET available_seats = available_seats + ? WHERE train_no = ?")
                    );
                    restoreSeats->setInt(1, tickets);
                    restoreSeats->setInt(2, trainNo);
                    if(restoreSeats->executeUpdate() != 1) {
                        throw runtime_error("Associated train was not found.");
                    }
                    unique_ptr<sql::PreparedStatement> cancelBooking(
                        con->prepareStatement("UPDATE bookings SET status = 'CANCELLED' WHERE booking_id = ? AND status = 'ACTIVE'")
                    );
                    cancelBooking->setInt(1, bookingId);
                    if(cancelBooking->executeUpdate() != 1) {
                        throw runtime_error("Booking could not be cancelled.");
                    }
                    con->commit();
                    cout << "Booking cancelled successfully!" << endl;
                    cout << tickets << " seat(s) restored to train " << trainNo << "." << endl;
                }
                catch (...) {
                    con->rollback();
                    throw;
                }
            }
            catch (sql::SQLException& e) {
                cerr << "MySQL error while cancelling booking: " << e.what() << endl;
            }
            catch (const exception& e) {
                cerr << "Error while cancelling booking: " << e.what() << endl;
            }
        }

        void viewBookings(){
            try {
                unique_ptr<sql::Connection> con(connectToDatabase());
                unique_ptr<sql::PreparedStatement> pstmt(
                    con->prepareStatement("SELECT b.booking_id, b.train_no, t.train_name, b.tickets, b.status FROM bookings b JOIN users u ON b.user_id = u.user_id JOIN trains t ON b.train_no = t.train_no WHERE u.username = ? ORDER BY b.booking_id")
                );
                pstmt->setString(1, username);
                unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
                cout << endl << " --- My Bookings --- " << endl;
                bool found = false;
                while(res->next()) {
                    found = true;
                    cout << "Booking ID: " << res->getInt("booking_id") << endl;
                    cout << "Train: " << res->getString("train_name") << endl;
                    cout << "Train Number: " << res->getInt("train_no") << endl;
                    cout << "Tickets: " << res->getInt("tickets") << endl;
                    cout << "Status: " << res->getString("status") << endl;
                    cout << "------------------------" << endl;
                }
                if(!found) {
                    cout << "No bookings found." << endl;
                }
            }
            catch (sql::SQLException& e) {
                cerr << "MySQL error while viewing bookings: " << e.what() << endl;
            }
        }   
        
        void setUsername(string user) {
            username = user;
        }
};

int main(){
    loadEnvFile();
    userSystem user;
    adminPanel admin;
    userPanel passenger;
    int role;
    cout << "Welcome! Please select your role: "<<endl;
    cout << "1. ADMIN"<<endl;
    cout << "2. USER"<<endl;
    cout << "Enter choice: ";
    if(!readInt(role) || (role != 1 && role != 2)) {
        cout << "Invalid role. Please select 1 or 2." << endl;
        return 0;
    }
    
    if(role == 1){
        admin.adminLogin();
    }
    else if(role == 2){
        int userChoice;
        cout << endl << "Hello, User! What would you like to do?"<<endl;
        cout << "1. REGISTER"<<endl;
        cout << "2. LOGIN"<<endl;
        cout << "Enter choice: ";
        if (!readInt(userChoice) || (userChoice != 1 && userChoice != 2)) {
            cout << "Invalid choice. Please select 1 or 2." << endl;
            return 0;
        }
        bool loggedIn = false;
        if(userChoice == 1){
            user.userRegister();
            loggedIn = user.userLogin();
        }
        else if(userChoice == 2){
            loggedIn = user.userLogin();
        }
        if(!loggedIn) return 0;
        passenger.setUsername(user.getUsername());
        int choice = 0;
        do {
            cout<<endl<<" ====== User Menu ====== "<<endl;
            cout<<"1. View Available Trains "<<endl;
            cout<<"2. Book Ticket "<<endl;
            cout<<"3. Cancel Ticket "<<endl;
            cout<<"4. View My Bookings "<<endl;
            cout<<"5. Exit User Menu "<<endl;
            cout<<"6. Logout"<<endl;
            cout<<"Enter Choice: ";
            if (!readInt(choice)) {
                cout << "Invalid input. Please enter a number." << endl;
                continue;
            }
            switch(choice){
                case 1: passenger.viewAvailableTrains(); break;
                case 2: passenger.bookTicket(); break;
                case 3: passenger.cancelTicket(); break;
                case 4: passenger.viewBookings(); break;
                case 5: cout <<endl<< " --- Exiting User Menu... ---" << endl <<endl; break;
                case 6: cout<<endl<<" --- Successfully Logged Out --- "<<endl<<" --- Thankyou. Visit Again. --- "<<endl; break;
                default: cout << endl<< " --- Invalid Choice. --- " << endl;
            }
        } while(choice != 5 && choice != 6);
    }
    else{
        cout << " --- Invalid role selected. --- "<<endl;
    }  
    return 0;
}