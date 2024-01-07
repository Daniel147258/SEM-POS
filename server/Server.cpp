#include <iostream>
#include <vector>
#include <thread>
#include <cstring>
#include <algorithm>
#include <mutex>
#include "Database.h"
#include <regex>
#include <atomic>
#include <conio.h> // Pro _getch()
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
    std::atomic<bool> running{true};
    Database* database;
    std::mutex clnt;

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
            //Login
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
                        std::string name(buffer,bytesRead);
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

            //Register
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
                        std::string name(buffer,bytesRead);
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
                                if(!database->existUsername(name)) {
                                    client = new User(name, heslo);
                                    database->addUser(name, heslo);
                                    message = "Registration successful click ENTER";
                                    send(clientSocket, message, strlen(message), 0);
                                    break;
                                }else{
                                    message = "dont response";
                                    send(clientSocket, message, strlen(message),0);
                                    message = "User with this name was added when you was registring try it again!!";
                                    send(clientSocket, message, strlen(message),0);
                                    break;
                                }
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

        if(client != nullptr) {
            bool odpojilSa = false;
            const char *dontResponse = "Dont response";
            const char *optionsMessage = "Select one option\n"
                                         "1. ""Select table\n"
                                         "2. Create table\n"
                                         "3. Delete table\n"
                                         "4. Print all my Tables\n"
                                         "5. Print tables with rights to me\n"
                                         "6. Add user right to table\n"
                                         "9. Quit\n"
                                         "Choose your option: ";
            send(clientSocket, optionsMessage, strlen(optionsMessage), 0);
            char buffer[5000];
            while (!odpojilSa) {
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
                if (option == "1") {
                    send(clientSocket, dontResponse, strlen(dontResponse), 0);
                    std::string header = "Tables to choose:\n" + database->getTables();
                    const char *messageToSend = header.c_str();
                    send(clientSocket, messageToSend, strlen(messageToSend), 0);
                    if (header.find("Not tabels yet")) {
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
                            if (std::string(buffer, bytesRead) == "@") {
                                break;
                            }
                            if (database->existTable(std::string(buffer, bytesRead))) {
                                tableName = std::string(buffer, bytesRead);
                                while (!odpojilSa) {
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
                                    if (std::string(buffer, bytesRead) == "@") {
                                        break;
                                    }
                                    std::string a(buffer, bytesRead);
                                    if (a == "1") {
                                        if (database->getTable(tableName)->canUserSelect(client)) {
                                            while (!odpojilSa) {
                                                messageToSend = "Select index of row:";
                                                send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                                bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
                                                if (bytesRead <= 0) {
                                                    std::cout << "Client disconnected\n";
                                                    closeClient(clientSocket);
                                                    odpojilSa = true;
                                                    break;
                                                }
                                                if (std::string(buffer, bytesRead) == "@") {
                                                    break;
                                                }
                                                a = std::string(buffer, bytesRead);
                                                try {
                                                    int index = std::stoi(a);
                                                    send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                                    if (index != 0) {
                                                        std::string message = database->getRowByIndex(tableName, index);
                                                        send(clientSocket, message.c_str(), message.size(), 0);
                                                    } else {
                                                        messageToSend = "Wrong index!!\n";
                                                        send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                                    }
                                                } catch (const std::invalid_argument &e) {
                                                    send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                                    messageToSend = "Invalid index format. Please provide a valid integer.\n";
                                                    send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                                    continue;
                                                }
                                            }
                                        } else {
                                            send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                            messageToSend = "You dont have right to select";
                                            send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                        }
                                        continue;
                                    }
                                    if (a == "2") {
                                            if(database->getTable(tableName)->canUserSelect(client)) {
                                                send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                                std::string message = database->getAllRows(tableName);
                                                send(clientSocket, message.c_str(), message.size(), 0);
                                                continue;
                                            } else{
                                                send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                                messageToSend = "You dont have right to select";
                                                send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                            }
                                        }
                                    if (a == "3") {
                                        if (database->getTable(tableName)->canUserUpdate(client)){
                                            while(!odpojilSa) {
                                                messageToSend = "Select one option:\n"
                                                                "1. Select index of column\n"
                                                                "2. Show index of columns\n"
                                                                "Choose option: ";
                                                send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                                bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
                                                if (bytesRead <= 0) {
                                                    std::cout << "Client disconnected\n";
                                                    closeClient(clientSocket);
                                                    odpojilSa = true;
                                                    break;
                                                }
                                                if (std::string(buffer, bytesRead) == "@") {
                                                    break;
                                                }
                                                if (std::string(buffer, bytesRead) == "1") {
                                                    while (!odpojilSa) {
                                                        messageToSend = "Select index of column: ";
                                                        send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                                        bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
                                                        if (bytesRead <= 0) {
                                                            std::cout << "Client disconnected\n";
                                                            closeClient(clientSocket);
                                                            odpojilSa = true;
                                                            break;
                                                        }
                                                        if (std::string(buffer, bytesRead) == "@") {
                                                            break;
                                                        }
                                                        a = std::string(buffer, bytesRead);

                                                        if (std::stoi(a)) {
                                                            int index = std::stoi(a) - 1;
                                                            if (database->existsColumnIndex(tableName, index)) {
                                                                if (!database->getTable(tableName)->isColumnPrimaryKer(
                                                                        index)) {
                                                                    while (!odpojilSa) {
                                                                        messageToSend = "Select value which you want change: ";
                                                                        send(clientSocket, messageToSend,
                                                                             strlen(messageToSend),
                                                                             0);
                                                                        bytesRead = recv(clientSocket, buffer,
                                                                                         sizeof(buffer),
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
                                                                        while (!odpojilSa) {
                                                                            std::string oldValue(buffer, bytesRead);
                                                                            messageToSend = "Select new value: ";
                                                                            send(clientSocket, messageToSend,
                                                                                 strlen(messageToSend),
                                                                                 0);
                                                                            bytesRead = recv(clientSocket, buffer,
                                                                                             sizeof(buffer),
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
                                                                            send(clientSocket, dontResponse,
                                                                                 strlen(dontResponse),
                                                                                 0);
                                                                            std::string l = "Number of uptades rows: " +
                                                                                            std::to_string(
                                                                                                    database->getTable(
                                                                                                            tableName)->uploadAllColumnValues(
                                                                                                            index,
                                                                                                            oldValue,
                                                                                                            newValue));
                                                                            send(clientSocket, l.c_str(), l.size(),
                                                                                 0);
                                                                        }
                                                                        break;
                                                                    }
                                                                    continue;
                                                                } else {
                                                                    send(clientSocket, dontResponse,
                                                                         strlen(dontResponse),
                                                                         0);
                                                                    messageToSend = "Column is primary key you cant change value of primary key!!!";
                                                                    send(clientSocket, messageToSend,
                                                                         strlen(messageToSend),
                                                                         0);
                                                                }
                                                            } else {
                                                                send(clientSocket, dontResponse, strlen(dontResponse),
                                                                     0);
                                                                messageToSend = "Wrong index!!";
                                                                send(clientSocket, messageToSend, strlen(messageToSend),
                                                                     0);
                                                            }
                                                        }
                                                    }
                                                    continue;
                                                }
                                                if (std::string(buffer, bytesRead) == "2") {
                                                    send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                                    std::string message = database->getColumnsIndexes(tableName);
                                                    send(clientSocket, message.c_str(), message.size(), 0);
                                                    continue;
                                                } else {
                                                    send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                                    messageToSend = "Wrong option!!";
                                                    send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                                }
                                            }
                                        } else{
                                            send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                            messageToSend = "You dont have right to update !!";
                                            send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                        }
                                        continue;
                                    }
                                    bool odisiel = false;
                                    if (a == "4") {
                                        if (database->getTable(tableName)->canUserAdd(client)){
                                            while (!odisiel) {
                                                int numberColumns = database->getNumberOfColumns(tableName);
                                                std::vector<std::string> values;
                                                for (int i = 0; i < numberColumns; ++i) {
                                                    while (!odisiel) {
                                                        send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                                        std::string message = database->getColumnDescription(tableName,
                                                                                                             i);
                                                        send(clientSocket, message.c_str(), message.size(), 0);
                                                        messageToSend = "Select Value: ";
                                                        send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                                        bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
                                                        if (bytesRead <= 0) {
                                                            std::cout << "Client disconnected\n";
                                                            closeClient(clientSocket);
                                                            odpojilSa = true;
                                                            odisiel = true;
                                                            break;
                                                        }
                                                        if (std::string(buffer, bytesRead) == "@") {
                                                            odisiel = true;
                                                            break;
                                                        }
                                                        std::string value1(buffer, bytesRead);
                                                        bool isNullable = !database->isColumnNullAble(tableName, i);
                                                        if (isNullable) {
                                                            if (value1 == "NULL" || value1 == "" || value1 == " " ||
                                                                value1 == "null" || value1 == "Null" ||
                                                                value1 == "nullptr") {
                                                                value1 = "";
                                                                values.push_back(value1);
                                                                break;
                                                            } else {
                                                                std::string type = database->getTypeOfColumn(tableName,
                                                                                                             i);
                                                                if (!type.empty()) {
                                                                    if (type == "int") {
                                                                        try {
                                                                            int intValue = std::stoi(value1);
                                                                            values.push_back(value1);
                                                                            break;
                                                                        } catch (const std::invalid_argument &e) {
                                                                            // Handle invalid argument exception (e.g., when value1 is not a valid integer)
                                                                            send(clientSocket, dontResponse,
                                                                                 strlen(dontResponse), 0);
                                                                            messageToSend = "Wrong value for int!!";
                                                                            send(clientSocket, messageToSend,
                                                                                 strlen(messageToSend), 0);
                                                                            continue;
                                                                        } catch (const std::out_of_range &e) {
                                                                            // Handle out-of-range exception (e.g., when the converted value is too large)
                                                                            send(clientSocket, dontResponse,
                                                                                 strlen(dontResponse), 0);
                                                                            messageToSend = "Value out of range for int!!";
                                                                            send(clientSocket, messageToSend,
                                                                                 strlen(messageToSend), 0);
                                                                            continue;
                                                                        }
                                                                    } else if (type == "string") {
                                                                        values.push_back(value1);
                                                                    } else if (type == "double") {
                                                                        try {
                                                                            double doubleValue = std::stod(value1);
                                                                            values.push_back(value1);
                                                                            break;
                                                                        } catch (const std::invalid_argument &e) {
                                                                            // Handle invalid argument exception (e.g., when value1 is not a valid double)
                                                                            send(clientSocket, dontResponse,
                                                                                 strlen(dontResponse), 0);
                                                                            messageToSend = "Wrong value for double!!";
                                                                            send(clientSocket, messageToSend,
                                                                                 strlen(messageToSend), 0);
                                                                            continue;
                                                                        } catch (const std::out_of_range &e) {
                                                                            // Handle out-of-range exception (e.g., when the converted value is too large)
                                                                            send(clientSocket, dontResponse,
                                                                                 strlen(dontResponse), 0);
                                                                            messageToSend = "Value out of range for double!!";
                                                                            send(clientSocket, messageToSend,
                                                                                 strlen(messageToSend), 0);
                                                                            continue;
                                                                        }
                                                                    } else if (type == "date") {
                                                                        std::regex dateRegex(R"(\d{4}-\d{2}-\d{2})");
                                                                        if (std::regex_match(value1, dateRegex)) {
                                                                            values.push_back(value1);
                                                                        } else {
                                                                            send(clientSocket, dontResponse,
                                                                                 strlen(dontResponse), 0);
                                                                            messageToSend = "Wrong value for type of this column!!";
                                                                            send(clientSocket, messageToSend,
                                                                                 strlen(messageToSend), 0);
                                                                        }
                                                                    } else if (type == "bool") {
                                                                        if (value1 == "true" || value1 == "false") {
                                                                            values.push_back(value1);
                                                                            break;
                                                                        } else {
                                                                            send(clientSocket, dontResponse,
                                                                                 strlen(dontResponse), 0);
                                                                            messageToSend = "Wrong value for bool!!";
                                                                            send(clientSocket, messageToSend,
                                                                                 strlen(messageToSend), 0);
                                                                            continue;
                                                                        }
                                                                    } else {
                                                                        send(clientSocket, dontResponse,
                                                                             strlen(dontResponse), 0);
                                                                        messageToSend = "Problem with getting type of column!!";
                                                                        send(clientSocket, messageToSend,
                                                                             strlen(messageToSend), 0);
                                                                        break;
                                                                    }
                                                                }
                                                            }
                                                        } else {
                                                            std::string type = database->getTypeOfColumn(tableName, i);
                                                            if (value1 == "NULL" || value1 == "" || value1 == " " ||
                                                                value1 == "null" || value1 == "Null" ||
                                                                value1 == "nullptr") {
                                                                send(clientSocket, dontResponse, strlen(dontResponse),
                                                                     0);
                                                                messageToSend = "Wrong value for this column!!";
                                                                send(clientSocket, messageToSend, strlen(messageToSend),
                                                                     0);
                                                            } else {
                                                                if (!type.empty()) {
                                                                    if (type == "int") {
                                                                        try {
                                                                            int intValue = std::stoi(value1);
                                                                            values.push_back(value1);
                                                                            break;
                                                                        } catch (const std::invalid_argument &e) {
                                                                            // Handle invalid argument exception (e.g., when value1 is not a valid integer)
                                                                            send(clientSocket, dontResponse,
                                                                                 strlen(dontResponse), 0);
                                                                            messageToSend = "Wrong value for int!!";
                                                                            send(clientSocket, messageToSend,
                                                                                 strlen(messageToSend), 0);
                                                                            continue;
                                                                        } catch (const std::out_of_range &e) {
                                                                            // Handle out-of-range exception (e.g., when the converted value is too large)
                                                                            send(clientSocket, dontResponse,
                                                                                 strlen(dontResponse), 0);
                                                                            messageToSend = "Value out of range for int!!";
                                                                            send(clientSocket, messageToSend,
                                                                                 strlen(messageToSend), 0);
                                                                            continue;
                                                                        }
                                                                    } else if (type == "string") {
                                                                        values.push_back(value1);
                                                                    } else if (type == "double") {
                                                                        try {
                                                                            double doubleValue = std::stod(value1);
                                                                            values.push_back(value1);
                                                                            break;
                                                                        } catch (const std::invalid_argument &e) {
                                                                            // Handle invalid argument exception (e.g., when value1 is not a valid double)
                                                                            send(clientSocket, dontResponse,
                                                                                 strlen(dontResponse), 0);
                                                                            messageToSend = "Wrong value for double!!";
                                                                            send(clientSocket, messageToSend,
                                                                                 strlen(messageToSend), 0);
                                                                            continue;
                                                                        } catch (const std::out_of_range &e) {
                                                                            // Handle out-of-range exception (e.g., when the converted value is too large)
                                                                            send(clientSocket, dontResponse,
                                                                                 strlen(dontResponse), 0);
                                                                            messageToSend = "Value out of range for double!!";
                                                                            send(clientSocket, messageToSend,
                                                                                 strlen(messageToSend), 0);
                                                                            continue;
                                                                        }
                                                                    } else if (type == "date") {
                                                                        std::regex dateRegex(R"(\d{4}-\d{2}-\d{2})");
                                                                        if (std::regex_match(value1, dateRegex)) {
                                                                            values.push_back(value1);
                                                                        } else {
                                                                            send(clientSocket, dontResponse,
                                                                                 strlen(dontResponse), 0);
                                                                            messageToSend = "Wrong value for type of this column!!";
                                                                            send(clientSocket, messageToSend,
                                                                                 strlen(messageToSend), 0);
                                                                        }
                                                                    } else if (type == "bool") {
                                                                        if (value1 == "true" || value1 == "false") {
                                                                            values.push_back(value1);
                                                                            break;
                                                                        } else {
                                                                            send(clientSocket, dontResponse,
                                                                                 strlen(dontResponse), 0);
                                                                            messageToSend = "Wrong value for bool!!";
                                                                            send(clientSocket, messageToSend,
                                                                                 strlen(messageToSend), 0);
                                                                            continue;
                                                                        }
                                                                    } else {
                                                                        send(clientSocket, dontResponse,
                                                                             strlen(dontResponse), 0);
                                                                        messageToSend = "Problem with getting type of column!!";
                                                                        send(clientSocket, messageToSend,
                                                                             strlen(messageToSend), 0);
                                                                        break;
                                                                    }
                                                                }
                                                            }
                                                        }
                                                        break;
                                                    }
                                                    if (odisiel) {
                                                        break;
                                                    }
                                                }
                                                database->addRow(tableName, values);
                                                break;
                                            }
                                        continue;
                                        } else{
                                            send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                            messageToSend = "You dont have premission add !!";
                                            send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                        }
                                    }
                                    if (a == "5") {
                                        if(database->getTable(tableName)->canUserDelete(client)){
                                            while (!odpojilSa) {
                                                messageToSend = "Choose one option:\n"
                                                                "1. Select index of column\n"
                                                                "2. Write index of columns\n"
                                                                "Choose option: ";
                                                send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                                bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
                                                if (bytesRead <= 0) {
                                                    std::cout << "Client disconnected\n";
                                                    closeClient(clientSocket);
                                                    odpojilSa = true;
                                                    break;
                                                }
                                                if (std::string(buffer, bytesRead) == "@") {
                                                    break;
                                                }
                                                if (std::string(buffer, bytesRead) == "1") {
                                                    while (!odpojilSa) {
                                                        messageToSend = "Select index: ";
                                                        send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                                        bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
                                                        if (bytesRead <= 0) {
                                                            std::cout << "Client disconnected\n";
                                                            closeClient(clientSocket);
                                                            odpojilSa = true;
                                                            break;
                                                        }
                                                        if (std::string(buffer, bytesRead) == "@") {
                                                            break;
                                                        }
                                                        std::string valo(buffer, bytesRead);
                                                        if (std::stoi(valo)) {
                                                            int index = std::stoi(valo);
                                                            while (!odpojilSa) {
                                                                messageToSend = "Select value which you want remove: ";
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
                                                                send(clientSocket, dontResponse, strlen(dontResponse),
                                                                     0);
                                                                std::string sd = "Deleted rows: " + std::to_string(
                                                                        database->DeleteRow(tableName, index,
                                                                                            std::string(buffer,
                                                                                                        bytesRead)));
                                                                messageToSend = sd.c_str();
                                                                send(clientSocket, messageToSend, strlen(messageToSend),
                                                                     0);
                                                                break;
                                                            }
                                                        } else {
                                                            send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                                            messageToSend = "Wrong value for index!!";
                                                            send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                                        }
                                                    }
                                                }
                                                if (std::string(buffer, bytesRead) == "2") {
                                                    send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                                    messageToSend = database->getColumnsIndexes(tableName).c_str();
                                                    send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                                    continue;
                                                } else {
                                                    send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                                    messageToSend = "Wrong option!!";
                                                    send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                                }


                                            }
                                        }
                                        else{
                                            send(clientSocket, dontResponse, strlen(dontResponse),0);
                                            messageToSend = "You dont have right to delete !!";
                                            send(clientSocket, dontResponse, strlen(dontResponse),0);
                                        }
                                        continue;
                                    }
                                    else{
                                        send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                        messageToSend = "Invalid option !!";
                                        send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                    }
                                }
                                continue;
                            } else {
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
                if (option == "2") {
                    while (!odpojilSa) {
                        Table *table = nullptr;
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
                            delete table;
                            break;
                        }
                        if (std::string(buffer, bytesRead) == "@") {
                            delete table;
                            break;
                        }
                        if (std::string(buffer, bytesRead) == "") {
                            send(clientSocket, dontResponse, strlen(dontResponse), 0);
                            messageToSend = "No allowed name for table !!\n";
                            send(clientSocket, messageToSend, strlen(messageToSend), 0);
                            continue;
                        }
                        if (!database->existTable(std::string(buffer, bytesRead))) {
                            nameOfTable = std::string(buffer, bytesRead);
                            table = new Table(nameOfTable, client);
                            bool tableCreated = false;
                            while (!odpojilSa && !tableCreated) {
                                bool columnAdded = false;
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
                                if (std::string(buffer, bytesRead) == "@") {
                                    break;
                                    delete table;
                                }
                                if (std::string(buffer, bytesRead) == "1") {
                                    while (!odpojilSa && !columnAdded) {
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
                                        if (std::string(buffer, bytesRead) == "@") {
                                            break;
                                        }
                                        if (std::string(buffer, bytesRead).empty()) {
                                            send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                            messageToSend = "Wrong name for column!!";
                                            send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                            continue;
                                        } else {
                                            name = std::string(buffer, bytesRead);
                                            std::string primaryKeyText;
                                            while (!odpojilSa && !columnAdded) {
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
                                                if (std::string(buffer, bytesRead) == "@") {
                                                    break;
                                                }
                                                if (std::string(buffer, bytesRead) == "1") {
                                                    primaryKeyText = "true";
                                                    primaryKey = true;
                                                }
                                                else if (std::string(buffer, bytesRead) == "2") {
                                                    primaryKeyText = "false";
                                                    primaryKey = false;
                                                } else {
                                                    send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                                    messageToSend = "Wrong option!!";
                                                    send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                                    continue;
                                                }
                                                std::string isNotNull;
                                                bool back = false;
                                                if (!primaryKey) {
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
                                                        if (std::string(buffer, bytesRead) == "@") {
                                                            back = true;
                                                            break;
                                                        }
                                                        if (std::string(buffer, bytesRead) == "1") {
                                                            isNotNull = "true";
                                                            break;
                                                        } else if (std::string(buffer, bytesRead) == "2") {
                                                            isNotNull = "false";
                                                            break;
                                                        } else {
                                                            send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                                            messageToSend = "Wrong option!!";
                                                            send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                                            continue;
                                                        }
                                                    }
                                                } else {
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
                                                    if (std::string(buffer, bytesRead) == "1") {
                                                        table->addColumn(new StringColumn(name, isNotNull == "true",
                                                                                          primaryKey));
                                                        send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                                        messageToSend = "Column added to table";
                                                        send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                                        columnAdded = true;
                                                        break;
                                                    }
                                                    else if (std::string(buffer, bytesRead) == "2") {
                                                        table->addColumn(
                                                                new IntColumn(name, isNotNull == "true", primaryKey));
                                                        send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                                        messageToSend = "Column added to table";
                                                        send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                                        columnAdded = true;
                                                        break;
                                                    } else if (std::string(buffer, bytesRead) == "3") {
                                                        table->addColumn(new DoubleColumn(name, isNotNull == "true",
                                                                                          primaryKey));
                                                        send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                                        messageToSend = "Column added to table";
                                                        send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                                        columnAdded = true;
                                                        break;
                                                    } else if (std::string(buffer, bytesRead) == "4") {
                                                        table->addColumn(new BoolColumn(name, isNotNull == "true"));
                                                        send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                                        messageToSend = "Column added to table";
                                                        send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                                        columnAdded = true;
                                                        break;
                                                    } else if (std::string(buffer, bytesRead) == "5") {
                                                        table->addColumn(new DateColumn(name, isNotNull == "true"));
                                                        send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                                        messageToSend = "Column added to table";
                                                        send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                                        columnAdded = true;
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
                                if (std::string(buffer, bytesRead) == "2") {
                                    if (table == nullptr || table->getNumberOfColumns() <= 0) {
                                        send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                        messageToSend = "Table dont have any columns!!";
                                        send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                        continue;
                                    } else {
                                        if (table->existsInTablePrimaryKey()) {
                                            if(!database->existTable(table->getName())){
                                                database->addTable(table);
                                                tableCreated = true;
                                                send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                                messageToSend = "Table was added into database";
                                                send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                                continue;
                                            }else{
                                                send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                                messageToSend = "Table in your time what you was creating table was created by another user!!";
                                                send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                            }
                                        } else {
                                            send(clientSocket, dontResponse, strlen(dontResponse), 0);
                                            messageToSend = "Table dont have primary key column !!";
                                            send(clientSocket, messageToSend, strlen(messageToSend), 0);
                                        }
                                    }
                                    continue;
                                }
                            }

                        } else {
                            send(clientSocket, dontResponse, strlen(dontResponse), 0);
                            messageToSend = "Name for table already exists!!\n";
                            send(clientSocket, messageToSend, strlen(messageToSend), 0);
                        }
                    }
                    send(clientSocket, optionsMessage, strlen(optionsMessage), 0);
                    continue;
                }
                //mazanie tabuliek
                if (option == "3") {
                    while(!odpojilSa){
                        const char *messageToSend = "Select name of table: ";
                        send(clientSocket, messageToSend, strlen(messageToSend), 0);
                        bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
                        if (bytesRead <= 0) {
                            std::cout << "Client disconnected\n";
                            closeClient(clientSocket);
                            odpojilSa = true;
                            break;
                        }
                        if (std::string(buffer, bytesRead) == "@") {
                            break;
                        }
                        send(clientSocket, dontResponse, strlen(dontResponse), 0);
                        std::string message2 = database->deleteTable(std::string(buffer,bytesRead),client);
                        send(clientSocket, message2.c_str(), message2.size(), 0);
                    }
                    send(clientSocket, optionsMessage, strlen(optionsMessage), 0);
                    continue;
                }
                // zobrazenie uzivovatelovych tabuliek
                if(option == "4"){
                    send(clientSocket, dontResponse, strlen(dontResponse), 0);
                    std::string message2 = database->getUserTables(client);
                    send(clientSocket, message2.c_str(), message2.size(), 0);
                    send(clientSocket, optionsMessage, strlen(optionsMessage), 0);
                    continue;
                }
                //vypisanie vsetkych tabuliek ku ktorym mam prava
                if(option == "5"){
                    send(clientSocket, dontResponse, strlen(dontResponse), 0);
                    std::string message2 = database->getUserTablesWithRights(client);
                    send(clientSocket, message2.c_str(), message2.size(), 0);
                    send(clientSocket, optionsMessage, strlen(optionsMessage), 0);
                    continue;
                }

                if (option == "9") {
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
                                                       "6. Add user right to table\n"
                                                       "9. Quit\n"
                                                       "Choose your option: ";
                    send(clientSocket, invalidOptionMessage, strlen(invalidOptionMessage), 0);
                }
            }
        }
        // toto nemozem zmazat to je blbost delete client;
        if(client == nullptr){
            delete client;
        }
        std::cout << "Thread ended/ClientDisconect\n";
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

            // Nastavit timeout na 1 sekundu (1000 ms)
            timeval timeout;
            timeout.tv_sec = 1;
            timeout.tv_usec = 0;

            fd_set readSet;
            FD_ZERO(&readSet);
            FD_SET(serverSocket, &readSet);

            // Pripravensot prijatia noveho spojenia s timeoutom
            int result = select(serverSocket + 1, &readSet, nullptr, nullptr, &timeout);

            if (result > 0 && FD_ISSET(serverSocket, &readSet)) {
                SOCKET clientSocket = accept(serverSocket, (struct sockaddr *) &clientAddress, &clientAddressLength);

                if (clientSocket != INVALID_SOCKET) {
                    std::cout << "Client connected" << std::endl;
                    clientSockets.push_back(clientSocket);
                    std::thread clientThread(&Server::handleClient, this, clientSocket);
                    clientThread.detach();
                }
            }
        }
        // doplnenie zapisania udajov

        std::cout << "Server is not more running\n";
    }

    void stopServer() {
        running = false;
    }

    bool isRunning(){
        return running;
    }

    ~Server() {
       closesocket(serverSocket);
        for (SOCKET clientSocket : clientSockets) {
            closeClient(clientSocket);
        }
        delete database;

#ifdef _WIN32
        WSACleanup();
#endif
    }
};


bool IsKeyPressed(int key) {
    return GetAsyncKeyState(key) & 0x8000;
}

int main() {
    int port = 3050;
    Server server(port);
    std::thread acceptThread(&Server::acceptConnections, &server);
    //Ukoncim server stlacenim klavesy ESCAPE==Esc
    while (server.isRunning()) {
        if (IsKeyPressed(VK_ESCAPE)) {
            server.stopServer();
            break;
        }
    }
    acceptThread.join();
    return 0;
}
