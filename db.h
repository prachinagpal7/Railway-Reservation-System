#ifndef DB_H
#define DB_H
#include <mysql/jdbc.h>
#include <memory>
#include <iostream>
#include <string>
#include <cstdlib>
#include <stdexcept>

inline sql::Connection* connectToDatabase() {
    const char* dbUser = std::getenv("RAILWAY_DB_USER");
    const char* dbPassword = std::getenv("RAILWAY_DB_PASSWORD");
    if (dbUser == nullptr || dbPassword == nullptr) {
        throw std::runtime_error(
            "Database credentials are not configured."
        );
    }
    sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
    sql::Connection* con = driver->connect("tcp://127.0.0.1:3306", dbUser, dbPassword);
    try {
        con->setSchema("railway_reservation");
    } catch (...) {
        delete con;
        throw;
    }
    return con;
}

#endif