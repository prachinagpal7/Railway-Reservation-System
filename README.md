# Railway Reservation System

A console-based railway reservation system built with C++ and MySQL. Passengers can register, log in, book and cancel tickets, and view their bookings. An admin panel manages the train records.

## Features

### Passenger
- User registration and login
- View available trains
- Book train tickets
- Check booking history
- Cancel active bookings
- Automatic seat availability updates
- Booking validation and database transactions

### Admin
- Log in with credentials from `.env`
- Add new trains
- View train information
- Search for trains
- Update train details
- Delete trains

## Tech Stack

- **Language:** C++
- **Database:** MySQL 8.0
- **Connectivity:** MySQL Connector/C++
- **Compiler:** Microsoft Visual C++ (MSVC), Windows only
- **Concepts:** OOP, prepared statements, transactions, exception handling, input validation

## Project Structure

```text
railway_project/
├── index.cpp          # Application logic and menus
├── db.h               # MySQL connection
├── config.h           # .env file loader
├── .env.example       # Template for environment variables
├── .gitignore
└── README.md
```

## Prerequisites

1. Windows
2. Visual Studio Build Tools with the MSVC C++ compiler
3. MySQL Server 8.0 or compatible
4. MySQL Connector/C++ for Windows x64
5. MySQL Workbench (optional, for running the setup SQL)

## Database Setup

### 1. Create the database

```sql
CREATE DATABASE railway_reservation;
USE railway_reservation;
```

### 2. Create the tables

```sql
CREATE TABLE users (
    user_id INT AUTO_INCREMENT PRIMARY KEY,
    username VARCHAR(50) NOT NULL UNIQUE,
    age INT CHECK (age > 13) NOT NULL,
    gender VARCHAR(50) NOT NULL,
    password VARCHAR(255) NOT NULL
);

CREATE TABLE trains (
    train_no INT PRIMARY KEY,
    train_name VARCHAR(100) NOT NULL,
    source VARCHAR(100) NOT NULL,
    destination VARCHAR(100) NOT NULL,
    departure_time VARCHAR(30) NOT NULL,
    fare INT NOT NULL,
    available_seats INT NOT NULL
);

CREATE TABLE bookings (
    booking_id INT AUTO_INCREMENT PRIMARY KEY,
    user_id INT NOT NULL,
    train_no INT NOT NULL,
    tickets INT CHECK (tickets > 0) NOT NULL,
    status ENUM('ACTIVE', 'CANCELLED') NOT NULL DEFAULT 'ACTIVE',
    FOREIGN KEY (user_id) REFERENCES users(user_id),
    FOREIGN KEY (train_no) REFERENCES trains(train_no)
);
```

### 3. Add trains

Trains can be added from the admin menu after the first run, or inserted directly into the `trains` table with SQL. Passengers cannot book anything until at least one train exists.

## Environment Configuration

Database access and admin login are read from environment variables.

1. Copy `.env.example` to `.env`.
2. Fill in your own values:

```dotenv
RAILWAY_DB_USER=your_mysql_username
RAILWAY_DB_PASSWORD=your_mysql_password
RAILWAY_ADMIN_USER=your_admin_username
RAILWAY_ADMIN_PASSWORD=your_admin_password
```

## Build and Run

### 1. Open a compiler terminal

Open the project folder and use the **x64 Native Tools Command Prompt for VS** so the MSVC compiler is on your path.

### 2. Point to Connector/C++

In PowerShell:

```powershell
$env:C = "$env:USERPROFILE\Downloads\mysql-connector-c++-26.7.0-winx64\mysql-connector-c++-26.7.0-winx64"
$env:PATH = "$env:C\lib64;$env:C\bin;$env:PATH"
```

Adjust the path if you extracted the connector elsewhere.

### 3. Compile

```powershell
cl /EHsc /MD /I "$env:C\include" index.cpp /link /LIBPATH:"$env:C\lib64\vs14" mysqlcppconn.lib /OUT:railway.exe
```

### 4. Run

```powershell
.\railway.exe
```

MySQL Server must be running and the database must exist before you launch.

## How Booking Works

Booking and cancellation each run inside a single database transaction. When a passenger books, the train row is locked with `SELECT ... FOR UPDATE`, so two people cannot take the last seat at the same time. The seat count update and the booking record are committed together, or rolled back together if anything fails. Cancellation restores the seats and marks the booking `CANCELLED` the same way, and only the booking's owner can cancel it.

All queries use prepared statements.

## Author

Prachi, second-year B.Tech CSE student. Built to practise OOP, database integration and transaction-based booking logic.
