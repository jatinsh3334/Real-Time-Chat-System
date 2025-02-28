# Real Time Chatting System
A Visual Studio 2022-based real-time networking chat system built in C++ using Winsock. This project enables multiple clients to connect to a central server for message exchange and broadcasting via socket programming.

# Features

✔ Real-time chat communication between clients & server 

✔ Message broadcasting support

✔ Built using Winsock for efficient socket-based networking

✔ CLI-based interface for lightweight and fast execution

✔ Structured into server and client components


# Project Structure
<pre>Real-Time-Chat-System/
│── server/ 
│ ├── server.sln # Solution file for Visual Studio (Important)
│ ├── x64/ # (Generated, may not be required)
│ ├── Debug/ # (Generated, may not be required) 
│ ├── server/ # Contains actual server code
│       │ ├── main.cpp # Core server logic (Winsock implementation) 
│       │ ├── ... other files
│   
│── client/ # Similar structure as server 
│ ├── client.sln
│ ├── x64/
│ ├── Debug/ 
│ ├── client/ 
│     │ ├── main.cpp # Core client logic 
│     │ ├── ... other files 
│    
│── .gitignore # Ignoring unnecessary files
│── README.md # This file ``` </pre>

# Steps to Run

1) Clone the Repository

  git clone --depth=1 <repository-url>

  (Sometimes git clone <repository-url> fails, so using --depth=1 is recommended.)

2) Open Visual Studio 2022

  Navigate to the server/ folder and open server.sln.

  Navigate to the client/ folder and open client.sln in a new Visual Studio window.

3) Run the Project

  Start debugging both the server and client by running the debugger in Visual Studio.

  Command prompts will appear for both, indicating that they are running.

4) Start Chatting

  The server listens for connections.

  The client connects and can send messages in real time.

