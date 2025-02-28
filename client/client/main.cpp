#include<iostream>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<thread>
#include<string>
#include <conio.h> // For _kbhit()
#include <windows.h> // For Sleep()
#include <iomanip>    // For timestamp formatting
#include <ctime>      // For time functions
#include <sstream>    // For stringstream to format timestamp
using namespace std;

#pragma comment(lib, "ws2_32.lib")

// Function to set console text color
void SetColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

// Initialize Winsock
bool Initialize()
{
    WSADATA data;
    return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}

// Function to get the current timestamp in desired format (Date and Time with AM/PM)
string GetTimestamp() {
    // Get the current time
    time_t t = time(0);  // Current time
    struct tm now;
    localtime_s(&now, &t);  // Safe version of localtime

    // Format the timestamp in "Month Day, Year, Hour:Minute AM/PM"
    stringstream ss;
    ss << put_time(&now, "%B %d, %Y, %I:%M %p");  // Format: "Month Day, Year, Hour:Minute AM/PM"
    return ss.str();
}

// Function to send messages
void SendMsg(SOCKET s)
{
    cout << "Enter your chat name:" ;
    string name;
    getline(cin, name); // Get name
    string message;

    // Set text color to white for input prompts and messages being typed
    SetColor(15); // White

    while (1)
    {
        SetColor(15); // White color for the user input
        getline(cin, message); // Get the message

        // Get the timestamp
        string timestamp = GetTimestamp();
        string msg = timestamp + " | " + name + ": " + message;

        // Display message dynamically in the same line
        cout << msg; // Use \r to overwrite the line
        //cout.flush();  // Ensure that the output is printed immediately

        // Send the message
        int bytesent = send(s, msg.c_str(), msg.length(), 0);

        if (bytesent == SOCKET_ERROR)
        {
            cout << "Error sending message" << endl;
            break;
        }

        // Display the sent message and add a separator after each message
        cout << "\n----------------------\n"; // Separator line

        if (message == "quit")
        {
            cout << "Stopping the application" << endl;
            break;
        }
    }

    closesocket(s);
    WSACleanup();
}


// Function to receive messages
void ReceiveMsg(SOCKET s)
{
    char buffer[4096];
    int recvlength;

    while (1)
    {
        recvlength = recv(s, buffer, sizeof(buffer), 0);
        if (recvlength <= 0) {
            cout << "Disconnected from the server" << endl;
            break;
        }

        // Display the received message (already timestamped by the server)
        string msg(buffer, recvlength);

        // Set text color to green for received messages
        SetColor(11); // Green
        cout << msg << endl;
        cout << "----------------------\n"; // Separator line

        // After displaying the message in green, reset the color to white for user input
        SetColor(15); // White
    }
}

int main()
{
    if (!Initialize())
    {
        cout << "Initialize Winsock Failed" << endl;
        return 1;
    }

    SOCKET s;
    s = socket(AF_INET, SOCK_STREAM, 0);

    if (s == INVALID_SOCKET)
    {
        cout << "Invalid socket created" << endl;
        return 1;
    }

    int port = 30185;
    string serveraddress = "127.0.0.1";
    sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);
    inet_pton(AF_INET, serveraddress.c_str(), &(serveraddr.sin_addr));

    if (connect(s, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR)
    {
        cout << "Not able to connect to server" << endl;
        closesocket(s);
        WSACleanup();
        return 1;
    }

    cout << "Successfully connected to server" << endl;


    cout << endl;

    // Set text color to white before starting the message input
    SetColor(15); // White for the input prompt

    // Create threads for sending and receiving messages
    thread senderthread(SendMsg, s);
    thread receiver(ReceiveMsg, s);

    senderthread.join();
    receiver.join();

    return 0;
}
