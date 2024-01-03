#include <iostream>
#include <vector>
#include <thread>
#include <cstring>
#include <algorithm>

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
    bool running;

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

        std::cout << "Server listening on port " << port << std::endl;
    }


    bool vypisMoznosti1(int clientSocket){
        bool odpojilSa = false;
        const char *optionsMessage = "Select one option\n1. "
                                    "Select table\n2. Create table\n3. Delete table\n9. Quit\n"
                                    "Choose your option: ";
        send(clientSocket, optionsMessage, strlen(optionsMessage), 0);
        while (true){

            char buffer[1024];
            memset(buffer, 0, sizeof(buffer));
            int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

            if (bytesRead <= 0) {
                std::cout << "Client disconnected\n";
                closeClient(clientSocket);
                odpojilSa = true;
                break;
            }

            // Spracovanie odpovede od klienta
            int option = atoi(buffer);
            switch (option) {
                case 1:
                    //function
                    break;
                case 2:
                    //function
                    break;
                case 3:
                    //function
                    break;
                case 9:
                    odpojilSa = true;
                    closeClient(clientSocket);
                    break;
                default:
                    const char *invalidOptionMessage = "Invalid option\n"
                                                       "Select one option\n"
                                                       "1. Select table\n"
                                                       "2. Create table\n"
                                                       "3. Delete table\n"
                                                       "9. Quit\n"
                                                       "Choose your option: ";
                    send(clientSocket, invalidOptionMessage, strlen(invalidOptionMessage), 0);
                    break;
            }
        }
        return odpojilSa;
    }

    void handleClient(int clientSocket) {
        while (true) {
           if(vypisMoznosti1(clientSocket)){
               break;
           }
        }
    }

    void closeClient(int clientSocket) {
        closesocket(clientSocket);
        auto it = std::find(clientSockets.begin(), clientSockets.end(), clientSocket);
        if (it != clientSockets.end()) {
            clientSockets.erase(it);
        }
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
