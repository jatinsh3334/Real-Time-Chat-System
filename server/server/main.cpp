#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <tchar.h>
#include <thread>
#include <vector>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <mutex> // Added for thread-safety
using namespace std;

mutex clientsMutex; // Mutex to ensure thread-safety while modifying the client list

void SetColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void ShowStatusBar(int activeUsers, int port) {
    SetColor(11); // Cyan
    cout << "\nChat Server | Active Users: " << activeUsers
        << " | Listening on Port: " << port << endl;
    SetColor(15); // Reset to White
}

#pragma comment(lib,"ws2_32.lib")

bool Initialize() {
    WSADATA data;
    return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}

void displayLoading() {
    const string spinner = "|/-\\"; // Spinner characters
    const int loadingSteps = 30;   // Number of steps in the loading animation
    const int delayMs = 100;       // Delay in milliseconds for each step

    cout << "\nInitializing Chat Server...\n" << endl;
    cout << "[";
    for (int i = 0; i < loadingSteps; i++) {
        cout << spinner[i % spinner.size()]; // Rotate through spinner characters
        cout.flush();
        this_thread::sleep_for(chrono::milliseconds(delayMs));
        cout << "\b"; // Erase the last character
    }
    cout << "] Ready!\n" << endl;
}

void displayBanner() {
    const string banner = R"(
**************************************************
*                                                *
*   W E L C O M E   T O   C H A T   S E R V E R  *
*                                                *
*      Your real-time communication starts here  *
*                                                *
**************************************************
)";
    for (char ch : banner) {
        cout << ch;
        cout.flush(); // Flush output for real-time typing effect
        this_thread::sleep_for(chrono::milliseconds(5)); // Typing delay
    }
    cout << endl;
}




void InteractWithClient(SOCKET clientSocket, vector<SOCKET>& clients, int& activeUsers) {
    cout << "Client connected!" << endl;

    char buffer[4096];
    while (1) {
        int bytesrecvd = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesrecvd <= 0) {
            cout << "Client got disconnected!" << endl;
            break;
        }

        string message(buffer, bytesrecvd);
        string timestampedMessage =  message;

        cout << "Message from client: " << timestampedMessage << endl;

        
        for (auto client : clients) {
            if (client != clientSocket) {
                send(client, timestampedMessage.c_str(), timestampedMessage.length(), 0);
            }
        }
    }

v
    {
        lock_guard<mutex> lock(clientsMutex); // Ensure thread-safety while modifying the client list
        auto it = find(clients.begin(), clients.end(), clientSocket);
        if (it != clients.end()) {
            clients.erase(it);
            --activeUsers; // Decrease active user count
        }
    }
    closesocket(clientSocket);
}

int main() {
    displayBanner(); // Show the dynamic banner
    displayLoading(); // Show the spinning loading animation
    cout << "Welcome to the Chat Server! Let's start chatting...\n" << endl;

    int activeUsers = 0;
    int port = 30185;

    // Display status bar initially
    ShowStatusBar(activeUsers, port);

    if (!Initialize()) {
        cout << "Initialization Failed!!!" << endl;
        return 1;
    }

    cout << "Server program" << endl;

    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == INVALID_SOCKET) {
        cout << "Socket creation Failed!!!" << endl;
        return 1;
    }

    sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);
    if (InetPton(AF_INET, _T("0.0.0.0"), &serveraddr.sin_addr) != 1) {
        cout << "Setting address structure failed" << endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    if (bind(listenSocket, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr))) {
        cout << "Bind failed" << endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        cout << "Listen failed!!!" << endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    cout << "Server has started listening on port: " << port << endl;
    vector<SOCKET> clients;

    while (1) {
        SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) {
            cout << "Invalid client socket" << endl;
            continue;
        }

        {
            lock_guard<mutex> lock(clientsMutex); // Thread-safe update of client list
            clients.push_back(clientSocket);
            ++activeUsers; // Increase active user count
        }

        ShowStatusBar(activeUsers, port); // Update status bar with active users

        thread t1(InteractWithClient, clientSocket, ref(clients), ref(activeUsers));
        t1.detach();
    }

    closesocket(listenSocket);
    WSACleanup();
    return 0;
}
