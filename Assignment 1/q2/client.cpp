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

    std::cout << "Client : ";
    int i = 0;

    while (1)
    {
        // Store the message in the buffer, message size should not exceed 1000.
        scanf("%c", &input);
        if (input == '\n')
            break;

        if (i == 1000)
        {
            // Maximum input size is 1000 characters
            std::cout << "The first 1000 characters of message are being sent\n\t";
            break;
        }
        buffer[i++] = input;
    }
    size = i;
}

int main(int argc, char *argv[])
{
    // if fewer arguments are entered
    if (argc < 3)
    {
        printf("MORE ARGUMENTS NEEDED");
        exit(1);
    }
    int client_socket, portno, n;
    portno = atoi(argv[2]); // get port number

    struct sockaddr_in server_address;
    struct hostent *server;
    client_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (client_socket < 0) // if socket connection could not be created
    {
        error("Error opening socket");
    }

    server = gethostbyname(argv[1]);
    if (server == NULL)
    {
        error("No such host found");
    }

    std::memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;     // IPv4
    server_address.sin_port = htons(portno); // short port number
    memcpy(&(server_address.sin_addr.s_addr), server->h_addr, server->h_length);

    // reading username
    std::string name;
    std::cout << "Enter your username : ";
    std::cin >> name;
    std::cout << std::endl;

    getchar();

    socklen_t length = sizeof(server_address); // Size of socket

    // if could not connect
    if (connect(client_socket, (struct sockaddr *)&server_address, length) < 0)
    {
        printf("COULD NOT CONNECT TO THE SERVER\n");
        exit(0);
    }

    // retrieve the local address (IP address and port)
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    getsockname(client_socket, (struct sockaddr *)&clientAddr, &clientAddrLen);
    int clientPort = ntohs(clientAddr.sin_port);

    std::cout << "Connected to server " << argv[1] << ":" << argv[2] << "\n";
    std::cout << "Client is connected using port : " << clientPort << "\n\n";

    // send username to server
    n = write(client_socket, name.c_str(), name.size() + 1);
    if (n < 0)
    {
        error("Error on sending name");
    }

    char buffer[1024] = {0};
    while (1)
    {
        int size = 0;
        read_message(buffer, size);

        // if user enters exit sentinel
        if (strncmp(buffer, "/exit", 5) == 0)
        {
            n = write(client_socket, buffer, strlen(buffer));
            if (n < 0) // error handling
            {
                error("Error on closing connection");
            }
            break;
        }

        // send message to the server
        n = write(client_socket, buffer, strlen(buffer));
        if (n < 0)
        {
            error("Error on writing");
        }

        // clear buffer
        memset(buffer, 0, MSG_LEN);

        // read incoming message
        n = read(client_socket, buffer, MSG_LEN);
        if (n < 0) // if read fails
        {
            error("Error on reading");
        }
        else if (n == 0) // if server crashes
        {
            std::cout << "Server disconnected" << std::endl;
            break;
        }
        // print incoming message
        std::cout << "Server : " << buffer << std::endl;
    }

    // close the socket descriptor
    close(client_socket);
    return 0;
}