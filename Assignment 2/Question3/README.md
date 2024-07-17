# Chat Application in C++

This Chat Application, developed in C++, provides a seamless and efficient platform for real-time communication among multiple users. With robust features and a user-friendly interface, it enables both private messaging and group discussions.

## Features:
- Multiple Simultaneous Users: Support for numerous users, ensuring concurrent connections without compromising performance.
- Broadcasting: Allows users to broadcast messages to all connected users, facilitating group discussions.
- Private Messaging: Enables private, one-on-one conversations between users, ensuring privacy and security.
- Graceful Exit: Provides users with a simple and intuitive way to exit the application gracefully, maintaining smooth user experience.

## Implementation Details:
- Network Communication: Implemented using TCP sockets, ensuring reliable and secure data transmission.
- File Descriptors (FD_SET): Utilized FD_SET file descriptor set for efficient management of input and output operations, enhancing application responsiveness.

## Usage
Compile and Run the Server: (on port 8080)
```
g++ server.cpp -o server
./server 8080
```

The server will start and wait for incoming client connections.

Run Clients: (./exe [IPaddress] [port number])
```
g++ client.cpp -o client
./client localhost 8080
```

Clients can connect to the server, send messages, and engage in conversations.

## Contributing
Contributions are welcome! Fork the repository and create a pull request with your enhancements.
