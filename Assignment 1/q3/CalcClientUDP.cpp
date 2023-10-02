// udp client program
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

int main(int argc, char *argv[])
{
    char buffer[1024];
    int client_socket, n;
    struct sockaddr_in server_address;
    struct hostent *host;
    int portno;
    socklen_t len;

    // if less arguments are entered
    if (argc < 3)
    {
        error("More arguments needed");
    }
    portno = atoi(argv[2]);

    host = gethostbyname(argv[1]);
    if (host == NULL)
    {
        error("No such host found");
    }

    // create datagram socket
    if ((client_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        error("socket creation failed");
    }

    // clear server_address
    std::memset(&server_address, 0, sizeof(server_address));

    // server_address.sin_addr.s_addr = inet_addr(argv[1]);
    server_address.sin_port = htons(portno);
    server_address.sin_family = AF_INET;
    memcpy(&(server_address.sin_addr.s_addr), host->h_addr, host->h_length);

    // Retrieve and store the local port number of the client socket
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    getsockname(client_socket, (struct sockaddr *)&clientAddr, &clientAddrLen); // Retrieve local address
    int clientPort = ntohs(clientAddr.sin_port);                                // Convert and store local port number

    std::cout << "Client is connecting using port : " << clientPort << std::endl;

    std::cout << "Connected to host at " << argv[1] << ":" << portno << '\n';

    while (1)
    {
        // prompt
        std::cout << "Write a binary expression num1 (op) num2\n"
                  << "Valid operators are + - * / ^\n"
                  << "enter /exit to close the socket\n";
        // read expression
        fgets(buffer, MSG_LEN, stdin);

        if (strncmp("/exit", buffer, 5) == 0) // exit sentinel
        {
            // send exit command
            sendto(client_socket, buffer, MSG_LEN, 0, (struct sockaddr *)&server_address, sizeof(server_address));
            break;
        }

        // send query to server
        sendto(client_socket, buffer, MSG_LEN, 0, (struct sockaddr *)&server_address, sizeof(server_address));

        // waiting for response
        n = recvfrom(client_socket, buffer, sizeof(buffer), 0, (struct sockaddr *)&server_address, &len);
        if (n < 0)
        {
            std::cout << "Server disconnected" << std::endl;
        }
        // print response
        std::cout << "Server Response : " << buffer << "\n\n";
    }
    // close the descriptor
    close(client_socket);
}
