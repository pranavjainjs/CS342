#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <map>

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

    printf("Server : ");
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
    std::cout << std::endl;
}

int main(int argc, char **argv)
{
    // if less arguments are entered
    if (argc < 2)
    {
        std::cout << "Enter more arguments" << std::endl;
    }
    int n, opt = 1;
    int master_socket, new_socket, addr_len;
    int activity, i, var_read, sd, max_sd_used;
    int max_clients = 10;
    int client_socket[10] = {0};
    struct sockaddr_in address;

    std::map<int, std::string> names;

    fd_set all_fds;

    // initiate a socket connection
    master_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (master_socket == 0)
    {
        error("socket failed");
    }

    if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
    {
        error("setsockopt");
    }

    int portno = atoi(argv[1]);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(portno);

    addr_len = sizeof(address);

    // bind socket to a port
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        error("bind failed");
    }
    std::cout << "Listening on port " << portno << std::endl;

    // listen for incoming connections
    if (listen(master_socket, 11) < 0)
    {
        error("listen");
    }

    char buffer[1024] = {0};

    while (1)
    {
        // clear the socket set
        FD_ZERO(&all_fds);

        // add master socket to set
        FD_SET(master_socket, &all_fds);
        max_sd_used = master_socket;

        // add child sockets to set
        for (i = 0; i < max_clients; i++)
        {
            sd = client_socket[i]; // socket descriptor

            if (sd > 0) // if valid socket descriptor then add to read list
                FD_SET(sd, &all_fds);

            if (sd > max_sd_used) // highest file descriptor number, need it for the select function
                max_sd_used = sd;
        }

        // wait for an activity on one of the sockets.
        // wait time is infinite.
        // timeout is null
        activity = select(max_sd_used + 1, &all_fds, nullptr, nullptr, nullptr);

        if ((activity < 0) && (errno != EINTR))
        {
            printf("select error");
        }

        // If something happened on the master socket, then its an incoming connection
        if (FD_ISSET(master_socket, &all_fds))
        {
            // accept the incoming connection
            if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t *)&addr_len)) < 0)
            {
                error("accept");
            }

            // read the username of new user
            n = read(new_socket, buffer, MSG_LEN);
            if (n < 0)
            {
                error("reading name");
            }
            printf("%s joined the server from %s:%d.\n", buffer, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

            // storing username corresponding to PORT number
            names[ntohs(address.sin_port)] = buffer;

            // add new socket to array of sockets
            for (i = 0; i < max_clients; i++)
            {
                // if position is empty
                if (client_socket[i] == 0)
                {
                    client_socket[i] = new_socket;
                    break;
                }
            }
        }

        // else its some IO operation on some other socket
        for (i = 0; i < max_clients; i++)
        {
            sd = client_socket[i];

            if (FD_ISSET(sd, &all_fds))
            {
                // retrieve the address (IP address and port) of the peer
                getpeername(sd, (struct sockaddr *)&address, (socklen_t *)&addr_len);

                // Somebody disconnected, get his details and print
                if ((var_read = read(sd, buffer, MSG_LEN)) == 0 || strncmp("/exit", buffer, 5) == 0)
                {
                    std::cout << names[ntohs(address.sin_port)];
                    std::cout << " disconnected [" << inet_ntoa(address.sin_addr) << ":" << ntohs(address.sin_port) << "]\n\n";

                    // Close the socket and mark as 0 in list for reuse
                    close(sd);
                    client_socket[i] = 0;
                }

                // read the incoming message
                else
                {
                    buffer[var_read] = '\0';

                    // print the incoming msg
                    std::cout << names[ntohs(address.sin_port)] << " : " << buffer << std::endl;

                    int size = 0;

                    // read reply of server
                    read_message(buffer, size);

                    int n = write(sd, buffer, size);
                    if (n < 0)
                    {
                        error("Error on writing");
                    }
                }
            }
        }
    }
    // empty the set and close the socket
    FD_ZERO(&all_fds);
    close(master_socket);
    return 0;
}