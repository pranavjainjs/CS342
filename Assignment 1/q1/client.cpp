#include <iostream>
#include "base64.c"
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

char close_text[] = "connection_close"; // text message to close connection

void connection_close(char buffer[], int client_socket)
{
    strcpy(buffer + 1, encoder(close_text));      // Append encoded value of "c" after the type of the message
    buffer[0] = '3';                              // marker for closing connection
    printf("CONNECTION CLOSED\n");                // Message on console to be printed
    write(client_socket, buffer, strlen(buffer)); // Write in client socket
}

void read_message(char buffer[], int &size)
{
    char input; // Used to read user input character by character
    memset(buffer, 0, MSG_LEN);

    printf("ENTER THE MESSAGE\n\t");
    int i = 0;

    while (1)
    {
        // Store the message in the buffer, message size should not exceed 1000.
        scanf("%c", &input);
        if (input == '\n')
            break;

        if (i == MSG_LEN)
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
    int client_socket, portno, n;
    struct sockaddr_in server_address;
    struct hostent *server;

    // if there are fewer command line arguments than required
    if (argc < 3)
    {
        printf("MORE ARGUMENTS NEEDED");
    }
    portno = atoi(argv[2]); // port number
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0)
    {
        error("Error opening socket");
    }

    server = gethostbyname(argv[1]); // IP address
    if (server == NULL)
    {
        error("No such host found");
    }

    std::memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;     // IPv4
    server_address.sin_port = htons(portno); // extracting port number from network byte order
    memcpy(&(server_address.sin_addr.s_addr), server->h_addr, server->h_length);

    socklen_t length = sizeof(server_address); // Size of socket

    // connecting to the server
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

    char buffer[1024] = {0};
    char temp[1024] = {0};
    while (1)
    {
        printf("Send message?\nPress 'y' for YES or any other key for NO and press enter: ");

        char input;
        scanf("%c", &input);
        // close connection if client does not want to send messages
        if (input != 'y')
        {
            connection_close(buffer, client_socket);
            break;
        }

        getchar(); // Read a character from standard input.This function is a possible cancellation point

        // read message from the command line
        int size = 0;
        read_message(temp, size);
        if (size == 0)
            continue;

        // print the message
        std::cout << "Actual msg : " << temp << '\n';

        // encode the message
        char *encrypt = encoder(temp);
        buffer[0] = '1';
        strcpy(buffer + 1, encrypt);
        std::cout << "Encrypted msg : " << buffer << '\n';

        // send encrypted message
        n = write(client_socket, buffer, strlen(encrypt) + 1);
        if (n < 0)
        {
            error("Error on writing");
        }
        memset(buffer, 0, MSG_LEN);

        // read incoming message
        n = read(client_socket, buffer, 100);
        if (n < 0)
        {
            error("Error on reading");
        }
        else if (n == 0)
        {
            std::cout << "Server disconnected" << std::endl;
            break;
        }
        std::cout << "Server : " << buffer << "\n";

        // decode message
        char *decrypt = decoder(buffer + 1);
        if (buffer[0] != '2')
        {
            error("Error in acknowledgement received");
        }
        // printing the decrypted message
        std::cout << "Decrypted message : " << decrypt << "\n\n";

    }
    close(client_socket);
    return 0;
}