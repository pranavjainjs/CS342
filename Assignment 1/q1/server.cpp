#include <iostream>
#include "base64.c"
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#define MSG_LEN 1000

char rec_ack[] = "msg received"; // receive acknowledgement

void error(const char *msg)
{
    std::cout << msg << std::endl;
    exit(1);
}

int main(int argc, char *argv[])
{
    // if fewer arguments are entered
    if (argc < 2)
    {
        printf("PORT number was not provided");
    }

    int server_socket, client_connect, portno, n;
    portno = atoi(argv[1]);

    char buffer[1024];
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    socklen_t client_len;

    // creating a socket connection
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        error("Socket cannot be created");
    }

    std::memset(&server_address, 0, sizeof(server_address));

    server_address.sin_family = AF_INET;         // IPv4
    server_address.sin_addr.s_addr = INADDR_ANY; // accepts connections from all IPs
    server_address.sin_port = htons(portno);

    // binding socket to a port number
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        error("Binding Failed");
    }

    // listening for connections
    if (listen(server_socket, 10) < 0)
    {
        error("Listen failed");
    }

    // print the port number
    std::cout << "Listening on PORT : " << portno << std::endl;

    client_len = sizeof(client_address);

    while (1)
    {
        // accept the incoming connection request
        client_connect = accept(server_socket, (struct sockaddr *)&client_address, &client_len);

        if (client_connect < 0)
        {
            error("Accept Failed");
        }

        // retrieving client IP and port number
        char *clientIP = inet_ntoa(client_address.sin_addr);
        int clientPort = ntohs(client_address.sin_port);

        // creating a fork
        int concurrent = fork();

        if (concurrent == -1)
        {
            std::cout << "Could not establish connection with " << clientIP << ":" << clientPort << std::endl;
            exit(1);
        }
        else if (concurrent == 0)
        {
            close(server_socket);
            std::cout << "Connected to client at " << clientIP << ":" << clientPort << " \n\n";

            while (1)
            {
                std::memset(&buffer, 0, MSG_LEN);

                // reading incoming message
                n = read(client_connect, buffer, 1000);
                if (n < 0)
                {
                    error("Error on Reading");
                }
                else if (n == 0) // if client crashes
                {
                    break;
                }

                std::cout << "Client " << clientIP << ":" << clientPort << " sent a message\n";
                // print encoded message
                std::cout << "\tEncrypted : " << buffer << '\n';

                char *decrypt = decoder(buffer + 1);
                // print decoded message
                std::cout << "\tDecrypted : " << decrypt << "\n\n";

                if (buffer[0] == '3') // if client wants to close connection
                {
                    break;
                }
                else if (buffer[0] == '1') // if client wants to keep communicating
                {
                    std::memset(&buffer, 0, MSG_LEN);
                    strcpy(buffer + 1, encoder(rec_ack));
                    buffer[0] = '2';

                    // send acknowledgement (encoded)
                    n = write(client_connect, buffer, strlen(buffer));
                    if (n < 0)
                    {
                        error("Error on Writing");
                    }
                    std::cout << "\tAcknowledgement sent\n\n";
                }
                else
                {
                    std::cout << "Error in messages from client " << clientIP << " : " << clientPort << std::endl;
                }
            }

            // close the client socket
            close(client_connect);
            std::cout << clientIP << ":" << clientPort << " ended the connection" << std::endl;
            exit(0);
        }
    }

    close(server_socket);

    return 0;
}