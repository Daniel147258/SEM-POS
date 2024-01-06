#include <iostream>
#include <vector>
#include <thread>
#include <cstring>
#include <algorithm>
#include <mutex>
#include "Database.h"
#include <regex>
#include "./Table/StringColumn.h"
#include "Table/IntColumn.h"
#include "Table/DoubleColumn.h"
#include "Table/BoolColumn.h"
#include "Table/DateColumn.h"

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <unistd.h>
#endif

class Server {
private:
    SOCKET serverSocket;
    std::vector<SOCKET> clientSockets;
    std::mutex clienMutex;
    bool running;
    Database* database;

public:
    Server(int port) : running(true) {
#ifdef _WIN32
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "Error initializing Winsock\n";
            exit(EXIT_FAILURE);
        }
#endif

        sockaddr_in serverAddress;
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket == -1) {
            std::cerr << "Error creating server socket\n";
            exit(EXIT_FAILURE);
        }

        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = INADDR_ANY;
        serverAddress.sin_port = htons(port);

        if (bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) == -1) {
            std::cerr << "Error binding server socket\n";
            exit(EXIT_FAILURE);
        }

        if (listen(serverSocket, 5) == -1) {
            std::cerr << "Error listening for connections\n";
            exit(EXIT_FAILURE);
        }
        database = new Database();
        std::cout << "Server listening on port " << port << std::endl;
    }


    bool option1(int clientSocket, User* client){
        bool odpojilSa = false;
        const char *dontResponse = "Dont response";
        const char *optionsMessage = "Select one option\n"
                                     "1. ""Select table\n"
                                          "2. Create table\n"
                                          "3. Delete table\n"
                                          "4. Print all my Tables\n"
                                          "5. Print tables with rights to me\n"
                                          "9. Quit\n"
                                    "Choose your option: ";
        send(clientSocket, optionsMessage, strlen(optionsMessage), 0);
        char buffer[5000];
        while (!odpojilSa){
            memset(buffer, 0, sizeof(buffer));
            int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
            if (bytesRead <= 0) {
                std::cout << "Client disconnected\n";
                closeClient(clientSocket);
                odpojilSa = true;
                break;
            }
            std::string option = buffer;
                //moznosti pre operacie nad tabulkou
                if(option == "1") {
                    send(clientSocket, dontResponse, strlen(dontResponse), 0);
                    std::string header = "Tables to choose:\n" + database->getTables();
                    const char *messageToSend = header.c_str();
                    send(clientSocket, messageToSend, strlen(messageToSend), 0);
                    if(header.find("Not tabels yet")) {
                        while (!odpojilSa) {
                            std::string tableName;
                            messageToSend = "Select name of table: ";
                            send(clientSocket, messageToSend, strlen(messageToSend), 0);
                            int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
                            if (bytesRead <= 0) {
                                std::cout << "Client disconnected\n";
                                closeClient(clientSocket);
                                odpojilSa = true;
                                break;
                            }
                            if(std::string(buffer,bytesRead) == "@"){
                                break;
                            }
                            if(database->existTable(std::string(buffer,bytesRead))){
                                tableName = std::string(buffer,bytesRead);
                                while(!odpojilSa){
                                    messageToSend = "Select one option:\n"
                                                    "1. Select row by index of row\n"
                                                    "2. Select all rows\n"
                                                    "3. Update row by index of column and value\n"
                                                    "4. Add row\n"
                                                    "5. Delete row\n"
                                                    "Choose your option: ";
                                    send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                    bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
                                    if (bytesRead <= 0) {
                                        std::cout << "Client disconnected\n";
                                        closeClient(clientSocket);
                                        odpojilSa = true;
                                        break;
                                    }
                                    if(std::string(buffer,bytesRead) == "@"){
                                        break;
                                    }
                                    std::string a(buffer,bytesRead);
                                    if(a == "1"){
                                        while(!odpojilSa){
                                            messageToSend = "Select index of row:";
                                            send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                            bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
                                            if (bytesRead <= 0) {
                                                std::cout << "Client disconnected\n";
                                                closeClient(clientSocket);
                                                odpojilSa = true;
                                                break;
                                            }
                                            if(std::string(buffer,bytesRead) == "@"){
                                                break;
                                            }
                                            a = std::string(buffer,bytesRead);
                                            try {
                                                int index = std::stoi(a);

                                                send(clientSocket, dontResponse, strlen(dontResponse), 0);

                                                if (index != 0) {
                                                    messageToSend = database->getRowByIndex(tableName, index).c_str();
                                                    send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                                } else {
                                                    messageToSend = "Wrong index!!\n";
                                                    send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                                }
                                            } catch (const std::invalid_argument& e) {
                                                // Handle invalid argument exception (e.g., when a is not a valid integer)
                                                send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                                messageToSend = "Invalid index format. Please provide a valid integer.\n";
                                                send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                            }
                                        }
                                        continue;
                                    }
                                    if(a == "2"){
                                        send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                        std::string message = database->getAllRows(tableName);
                                        send(clientSocket, message.c_str(), message.size(), 0);
                                        continue;
                                    }
                                    if(a == "3"){
                                        messageToSend = "Select one option:\n"
                                                        "1. Show index of columns\n"
                                                        "2. Select index of column\n"
                                                        "Choose option: ";
                                        send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                        bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
                                        if (bytesRead <= 0) {
                                            std::cout << "Client disconnected\n";
                                            closeClient(clientSocket);
                                            odpojilSa = true;
                                            break;
                                        }
                                        if(std::string(buffer,bytesRead) == "@"){
                                            break;
                                        }
                                        if(std::string(buffer,bytesRead) == "1"){
                                            while (!odpojilSa){
                                                messageToSend = "Select index of column: ";
                                                send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                                bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
                                                if (bytesRead <= 0) {
                                                    std::cout << "Client disconnected\n";
                                                    closeClient(clientSocket);
                                                    odpojilSa = true;
                                                    break;
                                                }
                                                if(std::string(buffer,bytesRead) == "@"){
                                                    break;
                                                }
                                                a = std::string(buffer,bytesRead);
                                                if(std::stoi(a)){
                                                    int index = std::stoi(a);
                                                    if(database->existsColumnIndex(tableName,index)){
                                                        while (!odpojilSa){
                                                            messageToSend = "Select value which you want change: ";
                                                            send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                                            bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
                                                            if (bytesRead <= 0) {
                                                                std::cout << "Client disconnected\n";
                                                                closeClient(clientSocket);
                                                                odpojilSa = true;
                                                                break;
                                                            }
                                                            if(std::string(buffer,bytesRead) == "@"){
                                                                break;
                                                            }
                                                            while(!odpojilSa) {
                                                                std::string oldValue(buffer, bytesRead);
                                                                messageToSend = "Select new value: ";
                                                                send(clientSocket, messageToSend, strlen(messageToSend),
                                                                     0);
                                                                bytesRead = recv(clientSocket, buffer, sizeof(buffer),
                                                                                 0);
                                                                if (bytesRead <= 0) {
                                                                    std::cout << "Client disconnected\n";
                                                                    closeClient(clientSocket);
                                                                    odpojilSa = true;
                                                                    break;
                                                                }
                                                                if (std::string(buffer, bytesRead) == "@") {
                                                                    break;
                                                                }
                                                                std::string newValue(buffer, bytesRead);
                                                                send(clientSocket, dontResponse, strlen(dontResponse),
                                                                     0);
                                                                std::string l = "Number of uptades rows: " +
                                                                                std::to_string(database->getTable(
                                                                                        tableName)->uploadAllColumnValues(
                                                                                        index, oldValue, newValue));
                                                                messageToSend = l.c_str();
                                                                send(clientSocket, messageToSend, strlen(messageToSend),
                                                                     0);
                                                            }
                                                        }
                                                        continue;
                                                    }
                                                    else{
                                                        send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                                        messageToSend = "Wrong index!!";
                                                        send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                                    }
                                                }
                                            }
                                            continue;
                                        }
                                        if(std::string(buffer,bytesRead) == "2"){
                                            send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                            messageToSend = database->getColumnsIndexes(tableName).c_str();
                                            send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                            continue;
                                        }
                                        else{
                                            send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                            messageToSend = "Wrong option!!";
                                            send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                        }
                                        continue;
                                    }
                                    bool odisiel = false;
                                    if(a == "4"){
                                        while (!odisiel){
                                            send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                            messageToSend = "Please now you will be select values to add row";
                                            send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                            int numberColumns = database->getNumberOfColumns(tableName);
                                            std::vector<std::string> values;
                                            for(int i = 0 ; i < numberColumns; ++i ){
                                                while(true) {
                                                    send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                                    messageToSend = database->getColumnDescription(tableName,i).c_str();
                                                    send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                                    messageToSend = "Select Value: ";
                                                    send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                                    bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
                                                    if (bytesRead <= 0) {
                                                        std::cout << "Client disconnected\n";
                                                        closeClient(clientSocket);
                                                        odpojilSa = true;
                                                        break;
                                                    }
                                                    if(std::string(buffer,bytesRead) == "@"){
                                                        odisiel = true;
                                                        break;
                                                    }
                                                    std::string value1(buffer,bytesRead);
                                                    bool isNullable = !database->isColumnNullAble(tableName, i);
                                                    if(isNullable){
                                                        if(value1 == "NULL" || value1 == "" || value1 == " " || value1 == "null" || value1 == "Null" || value1 == "nullptr"){
                                                            value1 = "";
                                                            values.push_back(value1);
                                                            break;
                                                        } else{
                                                            std::string type = database->getTypeOfColumn(tableName,i);
                                                            if(!type.empty()){
                                                                if (type == "int") {
                                                                    if(std::stoi(value1)){
                                                                        values.push_back(value1);
                                                                        break;
                                                                    }
                                                                    else{
                                                                        send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                                                        messageToSend = "Wrong value for int!!";
                                                                        send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                                                        continue;
                                                                    }
                                                                }
                                                                else if (type == "string") {
                                                                    values.push_back(value1);
                                                                }
                                                                else if (type == "double") {
                                                                    if(std::stod(value1)){
                                                                        values.push_back(value1);
                                                                        break;
                                                                    }
                                                                    else{
                                                                        send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                                                        messageToSend = "Wrong value for double!!";
                                                                        send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                                                        continue;
                                                                    }
                                                                }
                                                                else if (type == "date"){
                                                                    std::regex dateRegex(R"(\d{4}-\d{2}-\d{2})");
                                                                    if(std::regex_match(value1, dateRegex)){
                                                                        values.push_back(value1);
                                                                    } else{
                                                                        send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                                                        messageToSend = "Wrong value for type of this column!!";
                                                                        send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                                                    }
                                                                }
                                                                else if (type == "bool"){
                                                                    if(value1 == "true" || value1 == "false"){
                                                                        values.push_back(value1);
                                                                        break;
                                                                    }
                                                                    else{
                                                                        send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                                                        messageToSend = "Wrong value for bool!!";
                                                                        send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                                                        continue;
                                                                    }
                                                                }
                                                                else{
                                                                    send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                                                    messageToSend = "Problem with getting type of column!!";
                                                                    send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                                                    break;
                                                                }
                                                            }
                                                        }
                                                    }
                                                    else{
                                                        std::string type = database->getTypeOfColumn(tableName,i);
                                                        if(value1 == "NULL" || value1 == "" || value1 == " " || value1 == "null" || value1 == "Null" || value1 == "nullptr"){
                                                            send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                                            messageToSend = "Wrong value for this column!!";
                                                            send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                                        } else{
                                                            if(!type.empty()){
                                                                if (type == "int") {
                                                                    if(std::stoi(value1)){
                                                                        values.push_back(value1);
                                                                        break;
                                                                    }
                                                                    else{
                                                                        send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                                                        messageToSend = "Wrong value for int!!";
                                                                        send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                                                        continue;
                                                                    }
                                                                }
                                                                else if (type == "string") {
                                                                    values.push_back(value1);
                                                                }
                                                                else if (type == "double") {
                                                                    if(std::stod(value1)){
                                                                        values.push_back(value1);
                                                                        break;
                                                                    }
                                                                    else{
                                                                        send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                                                        messageToSend = "Wrong value for double!!";
                                                                        send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                                                        continue;
                                                                    }
                                                                }
                                                                else if (type == "date"){
                                                                    std::regex dateRegex(R"(\d{4}-\d{2}-\d{2})");
                                                                    if(std::regex_match(value1, dateRegex)){
                                                                        values.push_back(value1);
                                                                    } else{
                                                                        send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                                                        messageToSend = "Wrong value for type of this column!!";
                                                                        send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                                                    }
                                                                }
                                                                else if (type == "bool"){
                                                                    if(value1 == "true" || value1 == "false"){
                                                                        values.push_back(value1);
                                                                        break;
                                                                    }
                                                                    else{
                                                                        send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                                                        messageToSend = "Wrong value for bool!!";
                                                                        send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                                                        continue;
                                                                    }
                                                                }
                                                                else{
                                                                    send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                                                    messageToSend = "Problem with getting type of column!!";
                                                                    send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                                                    break;
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                                if(odisiel){
                                                    break;
                                                }
                                            }
                                            break;
                                        }
                                        continue;
                                    }
                                    if(a == "5"){
                                        while (!odpojilSa){
                                            messageToSend = "Choose one option:\n"
                                                            "1. Select index of column\n"
                                                            "2. Write index of columns\n"
                                                            "Choose option: ";
                                            send(clientSocket, messageToSend, strlen(messageToSend),0);
                                            bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
                                            if (bytesRead <= 0) {
                                                std::cout << "Client disconnected\n";
                                                closeClient(clientSocket);
                                                odpojilSa = true;
                                                break;
                                            }
                                            if(std::string(buffer,bytesRead) == "@"){
                                                break;
                                            }
                                            if(std::string(buffer,bytesRead) == "1"){
                                                while(!odpojilSa){
                                                    messageToSend = "Select index: ";
                                                    send(clientSocket, messageToSend, strlen(messageToSend),0);
                                                    bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
                                                    if (bytesRead <= 0) {
                                                        std::cout << "Client disconnected\n";
                                                        closeClient(clientSocket);
                                                        odpojilSa = true;
                                                        break;
                                                    }
                                                    if(std::string(buffer,bytesRead) == "@"){
                                                        break;
                                                    }
                                                    std::string valo(buffer, bytesRead);
                                                    if(std::stoi(valo)){
                                                        int index = std::stoi(valo);
                                                        while(!odpojilSa){
                                                            messageToSend = "Select value which you want remove: ";
                                                            send(clientSocket, messageToSend, strlen(messageToSend),0);
                                                            bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
                                                            if (bytesRead <= 0) {
                                                                std::cout << "Client disconnected\n";
                                                                closeClient(clientSocket);
                                                                odpojilSa = true;
                                                                break;
                                                            }
                                                            if(std::string(buffer,bytesRead) == "@"){
                                                                break;
                                                            }
                                                            send(clientSocket, dontResponse, strlen(dontResponse),0);
                                                            std::string sd = "Deleted rows: " + std::to_string(database->DeleteRow(tableName,index,std::string(buffer,bytesRead)));
                                                            messageToSend = sd.c_str();
                                                            send(clientSocket, messageToSend, strlen(messageToSend),0);
                                                            break;
                                                        }
                                                    }
                                                    else{
                                                        send(clientSocket, dontResponse, strlen(dontResponse),0);
                                                        messageToSend = "Wrong value for index!!";
                                                        send(clientSocket, messageToSend, strlen(messageToSend),0);
                                                    }
                                                }
                                            }
                                            if(std::string(buffer,bytesRead) == "2"){
                                                send(clientSocket, dontResponse, strlen(dontResponse),0);
                                                messageToSend = database->getColumnsIndexes(tableName).c_str();
                                                send(clientSocket, messageToSend, strlen(messageToSend),0);
                                                continue;
                                            }
                                            else{
                                                send(clientSocket, dontResponse, strlen(dontResponse),0);
                                                messageToSend = "Wrong option!!";
                                                send(clientSocket, messageToSend, strlen(messageToSend),0);
                                            }


                                        }
                                        continue;
                                    }
                                }
                                continue;
                            } else{
                                send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                messageToSend = "Table with this name dont exists!!";
                                send(clientSocket, messageToSend, strlen(messageToSend), 0);
                            }
                        }
                    }
                    send(clientSocket, optionsMessage, strlen(optionsMessage), 0);
                    continue;
                }
                //vytvorenie tabulky
                if(option == "2"){
                    while (!odpojilSa) {
                        Table* table = nullptr;
                        int numberColumns = 0;
                        bool primaryKey = false;
                        std::string nameOfTable = "";
                        const char *messageToSend = "Give name to table: ";
                        send(clientSocket, messageToSend, strlen(messageToSend), 0);
                        bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
                        if (bytesRead <= 0) {
                            std::cout << "Client disconnected\n";
                            closeClient(clientSocket);
                            odpojilSa = true;
                            break;
                        }
                        if(std::string(buffer,bytesRead) == "@"){
                            break;
                        }
                        if(std::string(buffer,bytesRead) == ""){
                            send(clientSocket, dontResponse, strlen(dontResponse), 0);
                            messageToSend = "No allowed name for table !!\n";
                            send(clientSocket, messageToSend, strlen(messageToSend), 0);
                            continue;
                        }
                        if(!database->existTable(std::string(buffer,bytesRead))){
                            nameOfTable = std::string(buffer,bytesRead);
                            table = new Table(nameOfTable, client);
                            bool tableCreated = false;
                            while(!odpojilSa && !tableCreated){
                                messageToSend = "Select one option:\n"
                                                "1. Add column\n"
                                                "2. Finish creating table\n"
                                                "Choose option: ";
                                send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
                                if (bytesRead <= 0) {
                                    std::cout << "Client disconnected\n";
                                    closeClient(clientSocket);
                                    odpojilSa = true;
                                    if (table != nullptr) {
                                        delete table;
                                        table = nullptr;
                                    }
                                    break;
                                }
                                if(std::string(buffer,bytesRead) == "@"){
                                    break;
                                    delete table;
                                }
                                //upravit este
                                if(std::string(buffer,bytesRead) == "1"){
                                    while(!odpojilSa){
                                        std::string name = "";
                                        messageToSend = "Get name to column: ";
                                        send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                        bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
                                        if (bytesRead <= 0) {
                                            std::cout << "Client disconnected\n";
                                            closeClient(clientSocket);
                                            odpojilSa = true;
                                            if (table != nullptr) {
                                                delete table;
                                                table = nullptr;
                                            }
                                            break;
                                        }
                                        if(std::string(buffer,bytesRead) == "@"){
                                            break;
                                        }
                                        if(std::string(buffer,bytesRead).empty()){
                                            send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                            messageToSend = "Wrong name for column!!";
                                            send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                            continue;
                                        }else{
                                            name = std::string(buffer,bytesRead);
                                            std::string primaryKeyText;
                                            while (!odpojilSa){
                                                messageToSend = "Select one option:\n"
                                                                "1. Column is primary key\n"
                                                                "2. Column is not primary key\n"
                                                                "Choose option: ";
                                                send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                                bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
                                                if (bytesRead <= 0) {
                                                    std::cout << "Client disconnected\n";
                                                    closeClient(clientSocket);
                                                    odpojilSa = true;
                                                    if (table != nullptr) {
                                                        delete table;
                                                        table = nullptr;
                                                    }
                                                    break;
                                                }
                                                if(std::string(buffer,bytesRead) == "@"){
                                                    break;
                                                }
                                                if(std::string(buffer,bytesRead) == "1"){
                                                    primaryKeyText = "true";
                                                    primaryKey = true;
                                                }
                                                else if(std::string(buffer,bytesRead) == "2"){
                                                    primaryKeyText = "false";
                                                    primaryKey = false;
                                                }
                                                else{
                                                    send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                                    messageToSend = "Wrong option!!";
                                                    send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                                    continue;
                                                }
                                                std::string isNotNull;
                                                bool back = false;
                                                if(!primaryKey) {
                                                    while (!odpojilSa && !back) {
                                                        messageToSend = "Select one option:\n"
                                                                        "1. Column is not null able \n"
                                                                        "2. Column is null able \n"
                                                                        "Choose option: ";
                                                        send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                                        bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
                                                        if (bytesRead <= 0) {
                                                            std::cout << "Client disconnected\n";
                                                            closeClient(clientSocket);
                                                            odpojilSa = true;
                                                            if (table != nullptr) {
                                                                delete table;
                                                                table = nullptr;
                                                            }
                                                            break;
                                                        }
                                                        if(std::string(buffer,bytesRead) == "@"){
                                                            back = true;
                                                            break;
                                                        }
                                                        if(std::string(buffer,bytesRead) == "1"){
                                                            isNotNull = "true";
                                                            break;
                                                        }
                                                        else if(std::string(buffer,bytesRead) == "2"){
                                                            isNotNull = "false";
                                                            break;
                                                        }
                                                        else{
                                                            send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                                            messageToSend = "Wrong option!!";
                                                            send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                                            continue;
                                                        }
                                                    }
                                                }
                                                else{
                                                    isNotNull = "true";
                                                }
                                                //zvolit typ stlpca
                                                while (!odpojilSa) {
                                                    messageToSend = "Select type:\n"
                                                                    "1. String\n"
                                                                    "2. Int\n"
                                                                    "3. Double\n"
                                                                    "4. Bool\n"
                                                                    "5. Date\n"
                                                                    "Choose option: ";
                                                    send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                                    bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
                                                    if (bytesRead <= 0) {
                                                        std::cout << "Client disconnected\n";
                                                        closeClient(clientSocket);
                                                        odpojilSa = true;
                                                        if (table != nullptr) {
                                                            delete table;
                                                            table = nullptr;
                                                        }
                                                        break;
                                                    }
                                                    if (std::string(buffer, bytesRead) == "@") {
                                                        break;
                                                    }
                                                    if(std::string(buffer, bytesRead) == "1"){
                                                        table->addColumn(new StringColumn(name, isNotNull == "true", primaryKey));
                                                        send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                                        messageToSend = "Column added to table";
                                                        send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                                        break;
                                                    }else if (std::string(buffer, bytesRead) == "2") {
                                                        table->addColumn(new IntColumn(name, isNotNull == "true", primaryKey));
                                                        send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                                        messageToSend = "Column added to table";
                                                        send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                                        break;
                                                    } else if (std::string(buffer, bytesRead) == "3") {
                                                        table->addColumn(new DoubleColumn(name, isNotNull == "true", primaryKey));
                                                        send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                                        messageToSend = "Column added to table";
                                                        send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                                        break;
                                                    } else if (std::string(buffer, bytesRead) == "4") {
                                                        table->addColumn(new BoolColumn(name, isNotNull == "true"));
                                                        send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                                        messageToSend = "Column added to table";
                                                        send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                                        break;
                                                    } else if (std::string(buffer, bytesRead) == "5") {
                                                        table->addColumn(new DateColumn(name, isNotNull == "true"));
                                                        send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                                        messageToSend = "Column added to table";
                                                        send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                                        break;
                                                    } else {
                                                        send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                                        messageToSend = "Wrong option!!";
                                                        send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    continue;
                                }
                                if(std::string(buffer,bytesRead) == "2"){
                                    if(table == nullptr || table->getNumberOfColumns() <= 0 ){
                                        send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                        messageToSend = "Table dont have any columns!!";
                                        send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                        continue;
                                    }
                                    else{
                                       if(table->existsInTablePrimaryKey()){
                                           database->addTable(table);
                                           tableCreated = true;
                                           send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                           messageToSend = "Table was added into database";
                                           send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                           continue;
                                       }
                                       else{
                                           send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                           messageToSend = "Table dont have primary key column !!";
                                           send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                       }
                                    }
                                    continue;
                                }
                            }

                        }
                        else{
                            send(clientSocket, dontResponse, strlen(dontResponse), 0);
                            messageToSend = "Name for table already exists!!\n";
                            send(clientSocket, messageToSend, strlen(messageToSend), 0);
                        }
                    }
                    send(clientSocket, optionsMessage, strlen(optionsMessage), 0);
                    continue;
                }
                if(option == "3"){
                   send(clientSocket, optionsMessage, strlen(optionsMessage), 0);
                   continue;
               }
                if(option == "9") {
                    odpojilSa = true;
                    closeClient(clientSocket);
                    break;
                }
                else {
                    const char *invalidOptionMessage = "Invalid option\n"
                                                       "Select one option\n"
                                                       "1. ""Select table\n"
                                                            "2. Create table\n"
                                                            "3. Delete table\n"
                                                       "4. Print all my Tables\n"
                                                       "5. Print tables with rights to me\n"
                                                       "9. Quit\n"
                                                       "Choose your option: ";
                    send(clientSocket, invalidOptionMessage, strlen(invalidOptionMessage), 0);
                }
        }
        return odpojilSa;
    }

    void handleClient(int clientSocket) {
        User* client = nullptr;
        //prihlasenie klienta
        while (client == nullptr) {
            const char* message = "For stepBack insert @\n"
                                  "Choose option:\n"
                                  "1. Login\n"
                                  "2. Register\n"
                                  "9. Quit\n"
                                  "Choose your option: ";
            send(clientSocket, message, strlen(message), 0);
            char buffer[1024];
            memset(buffer, 0, sizeof(buffer));
            int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
            if (bytesRead <= 0) {
                std::cout << "Client disconnected\n";
                closeClient(clientSocket);
                break;
            }
            std::string option = buffer;
            bool existUsername = false;
            if (option == "1") {
                message = "Select user name: ";
                send(clientSocket, message, strlen(message), 0);
                while (!existUsername) {
                    memset(buffer, 0, sizeof(buffer));
                    bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
                    if (bytesRead <= 0) {
                        std::cout << "Client disconnected\n";
                        closeClient(clientSocket);
                        break;
                    }
                    if(std::string(buffer,bytesRead) == "@"){
                        break;
                    }
                    if (database->existUsername(buffer)) {
                        existUsername = true;
                        std::string name = buffer;
                        message = "Select password: ";
                        send(clientSocket, message, strlen(message), 0);
                        while (true) {
                            memset(buffer, 0, sizeof(buffer));
                            bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
                            if (bytesRead <= 0) {
                                std::cout << "Client disconnected\n";
                                closeClient(clientSocket);
                                break;
                            }
                            if(std::string(buffer,bytesRead) == "@"){
                                existUsername = false;
                                message = "Select user name: ";
                                send(clientSocket, message, strlen(message), 0);
                                break;
                            }
                            std::string heslo(buffer, bytesRead);
                            if (database->getUser(name)->getHeslo() == heslo) {
                                client = database->getUser(name);
                                break;
                            } else {
                                message = "Wrong password!!\n"
                                          "Select password: ";
                                send(clientSocket, message, strlen(message), 0);
                            }
                        }
                    } else {
                        message = "This user name does not exist!!\n"
                                  "Select user name: ";
                        send(clientSocket, message, strlen(message), 0);
                    }
                }
                continue;
            }

            if (option == "2") {
                message = "Select user name: ";
                send(clientSocket, message, strlen(message), 0);
                while (!existUsername) {
                    memset(buffer, 0, sizeof(buffer));
                    bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
                    if (bytesRead <= 0) {
                        std::cout << "Client disconnected\n";
                        closeClient(clientSocket);
                        break;
                    }
                    if(std::string(buffer,bytesRead) == "@"){
                        break;
                    }
                    if (database->existUsername(buffer)) {
                        message = "This user name already exists!!\n"
                                  "Select user name: ";
                        send(clientSocket, message, strlen(message), 0);
                    } else {
                        std::string name = buffer;
                        existUsername = true;
                        message = "(Password must have 5 length) Select password:";
                        send(clientSocket, message, strlen(message), 0);
                        while (true) {
                            bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
                            if (bytesRead <= 0) {
                                std::cout << "Client disconnected\n";
                                closeClient(clientSocket);
                                break;
                            }
                            if(std::string(buffer,bytesRead) == "@"){
                                break;
                            }
                            std::string heslo(buffer, bytesRead);
                            if (heslo.length() >= 5) {
                                client = new User(name, heslo);
                                database->addUser(name, heslo);
                                message = "Registration successful click ENTER";
                                send(clientSocket, message, strlen(message), 0);
                                break;
                            } else {
                                message = "Invalid password!!!\n"
                                          "(Password must have 5 length) Select password: ";
                                send(clientSocket, message, strlen(message), 0);
                            }
                        }
                    }
                }
                continue;
            }

            if(option == "9"){
                closeClient(clientSocket);
                break;
            }

            else{
                message = "Wrong choice\nTo continue click ENTER\n";
                send(clientSocket, message, strlen(message), 0);
            }
        }

        //Zvysok moznosti pre klienta
        while (true) {
            if (option1(clientSocket, client)) {
                break;
            }
        }
    }


    void closeClient(int clientSocket) {
        std::unique_lock<std::mutex> lock(clienMutex);
        closesocket(clientSocket);
        auto it = std::find(clientSockets.begin(), clientSockets.end(), clientSocket);
        if (it != clientSockets.end()) {
            clientSockets.erase(it);
        }
        lock.unlock();
    }

    void acceptConnections() {
        while (running) {
            sockaddr_in clientAddress;
            int clientAddressLength = sizeof(clientAddress);
            SOCKET clientSocket = accept(serverSocket, (struct sockaddr *) &clientAddress, &clientAddressLength);
            if (clientSocket == -1) {
                std::cerr << "Error accepting connection\n";
                continue;
            }
            std::cout << "Client connected" << std::endl;
            clientSockets.push_back(clientSocket);
            std::thread clientThread(&Server::handleClient, this, clientSocket);
            clientThread.detach();
        }
    }

    ~Server() {
       closesocket(serverSocket);
        for (SOCKET clientSocket : clientSockets) {
            closeClient(clientSocket);
        }


#ifdef _WIN32
        WSACleanup();
#endif
    }
};

int main() {
    int port = 3050;
    Server server(port);
    server.acceptConnections();
    return 0;
}
