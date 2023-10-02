#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#define MSG_LEN 1000

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void read_message(char buffer[], int &size)
{
    char input; // Used to read user input character by character
    memset(buffer, 0, MSG_LEN);
    // prompt with instructions
    std::cout << "Write a binary expression num1 (op) num2\n"
              << "Valid operators are + - * / ^\n"
              << "enter /exit to close the socket\n";

    int i = 0;
    while (i <= 1000)
    {
        // Store the message in the buffer, message size should not exceed 1000.
        scanf("%c", &input);
        if (input == '\n')
            break;

        if (i == 1000)
        {
            // Maximum input size is 1000 characters
            std::cout << "The first 1000 characters of message are being sent\n";
            break;
        }
        buffer[i++] = input;
    }
    size = i;
}

int main(int argc, char *argv[])
{
    int client_socket, portno, n;
    struct sockaddr_in server_address;
    struct hostent *server;

    // if fewer arguments are passed
    if (argc < 3)
    {
        error("More arguments needed");
    }
    portno = atoi(argv[2]); // port number

    client_socket = socket(AF_INET, SOCK_STREAM, 0); // initiate a socket connection
    if (client_socket < 0)
    {
        error("Error opening socket");
    }
    server = gethostbyname(argv[1]); // get server name
    if (server == NULL)
    {
        error("No such host found");
    }

    // clear server_address
    std::memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;     // IPv4
    server_address.sin_port = htons(portno); // port number
    memcpy(&(server_address.sin_addr.s_addr), server->h_addr, server->h_length);

    socklen_t length = sizeof(server_address); // Size of socket

    // establish a connection between the client and the server using a socket.
    if (connect(client_socket, (struct sockaddr *)&server_address, length) < 0)
    {
        error("Connection Failed");
    }

    // retrive local address (IP and PORT)
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    getsockname(client_socket, (struct sockaddr *)&clientAddr, &clientAddrLen);
    int clientPort = ntohs(clientAddr.sin_port);

    // connection established
    std::cout << "Connected to host at " << argv[1] << ":" << portno << '\n';
    std::cout << "Client is connected using port : " << clientPort << "\n\n";

    char buffer[1024];
    while (1)
    {
        int size = 0;

        read_message(buffer, size); // read expression

        if (strncmp("/exit", buffer, 5) == 0) // if /exit sentinel is entered
        {
            // send message to server
            n = write(client_socket, buffer, strlen(buffer));
            if (n < 0)
            {
                error("Error on exit");
            }
            break;
        }

        // send message to server
        n = write(client_socket, buffer, strlen(buffer));
        if (n < 0)
        {
            error("Error on writing");
        }
        memset(buffer, 0, MSG_LEN);

        // reading response from server
        n = read(client_socket, buffer, 100);
        if (n < 0)
        {
            error("Error on reading");
        }
        else if (n == 0) // if server crashes abruptly
        {
            std::cout << "Server disconnected" << std::endl;
            break;
        }
        // printing response from server
        std::cout << "Server Response : " << buffer << "\n\n";
    }
    close(client_socket);
    return 0;
}