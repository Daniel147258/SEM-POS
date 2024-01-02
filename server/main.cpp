#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <vector>
#include <algorithm>

const int PORT = 8080;

std::vector<int> connectedClients;

void* handleClient(void* arg) {
    int clientSocket = *((int*)arg);
    char buffer[1024];

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytesRead <= 0) {
            std::cout << "Client disconnected\n";
            close(clientSocket);

            // Remove the client from the connectedClients vector
            auto it = std::find(connectedClients.begin(), connectedClients.end(), clientSocket);
            if (it != connectedClients.end()) {
                connectedClients.erase(it);
            }

            pthread_exit(NULL);
        }

        std::cout << "Received from client: " << buffer << std::endl;

        const char* response = "Hello from server!";
        send(clientSocket, response, strlen(response), 0);

        //TODO
    }
}

int main() {
    int serverSocket, clientSocket;
    sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error creating socket\n";
        return -1;
    }

    // Set up server address struct
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Error binding socket\n";
        close(serverSocket);
        return -1;
    }

    // Listen for incoming connections
    if (listen(serverSocket, SOMAXCONN) == -1) {
        std::cerr << "Error listening for connections\n";
        close(serverSocket);
        return -1;
    }

    std::cout << "Server listening on port " << PORT << "...\n";

    while (true) {
        // Accept a connection
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket == -1) {
            std::cerr << "Error accepting connection\n";
            continue;
        }

        // Add the client to the connectedClients vector
        connectedClients.push_back(clientSocket);

        // Create a thread to handle the client
        pthread_t thread;
        if (pthread_create(&thread, NULL, handleClient, &clientSocket) != 0) {
            std::cerr << "Error creating thread\n";
            close(clientSocket);

            // Remove the client from the connectedClients vector
            auto it = std::find(connectedClients.begin(), connectedClients.end(), clientSocket);
            if (it != connectedClients.end()) {
                connectedClients.erase(it);
            }
        }

        // Detach the thread to allow it to run independently
        pthread_detach(thread);
    }

    // Close the server socket
    close(serverSocket);

    return 0;
}

