// Necessary header files
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>    
#include <arpa/inet.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <bits/stdc++.h>

using namespace std;
// For color printing in linux terminal
#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */
// Some required declarations
#define TRUE 1
#define FALSE 0
#define SIZE 100
const int MAX = 1500;

// Global variables
map<int, int> client;
map<int, int> sockets;
int cnt = 1;
char temp[MAX];
set <int> clients_existing;

// Error message
void error(string msg)
{
    cout << BOLDRED << msg << RESET << endl;
    exit(0);
}

// Reading function
void Read()
{
    // Loop runs
    while(1)
    {
        string s; cin >> s;
        // If the server has to exit
        if(s == "Exit")
        {
            cout << endl;
            cout << BOLDYELLOW << "You've chosen to exit the server." << endl;
            cout << endl;
            cout << "Exiting.........................." << endl;
            cout << endl;
            sleep(2);
            cout << "Bye!" << endl;
            cout << endl;

            // Sending all the clients message that the server is closed
            for(int i = 0; i < cnt; i++)
            {
                int sockfd = sockets[i];
                if(sockfd > 0)
                {
                    char buffer2[MAX];
                    bzero(buffer2, sizeof(buffer2));
                    buffer2[0] = '$';
                    buffer2[1] = '\0';
                    send(sockfd, buffer2, strlen(buffer2), 0);
                }
            }
            exit(0);
        }
        // Otherwise, give a warning
        else
        {
            cout << BOLDRED << "Hey, you've entered a wrong word. If you want to exit, type 'Exit' and press enter." << RESET << endl;
        }
    }
}

// Main function
int main(int argc, char *argv[])
{
    int opt = TRUE;
    int master_socket, addrlen, new_socket, client_socket[30], max_clients = 30, activity, i, valread, sd;
    int max_sd;
    struct sockaddr_in address;

    char buffer[1500]; 

    fd_set readfds;

    // Initializing all the client sockets with 0
    for (i = 0; i < max_clients; i++)
    {
        client_socket[i] = 0;
    }

    master_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // SOcket creation
    if (master_socket == -1)
    {
        cout << endl;
        error("Oops! Socket creation failed. Try again.");
    }

    // Necessary
    int PORT = stoi(argv[1]);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Binding to the IP address
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        cout << endl;
        error("Oops! Binding failed. Try again.");
    }

    // Printing information
    cout << endl;
    cout << BOLDGREEN << "Server is working successfully!" <<  RESET << endl;
    cout << endl;
    cout << BOLDGREEN << "To exit the server, type 'Exit' and press enter." << endl;

    // Listening
    if(listen(master_socket,20) == -1)
	{
        cout << endl;
		error("Oops! Sorry, listening has failed. Please try again :(");
	}

    addrlen = sizeof(address);
    fflush(stdout);

    // Loop runs
    while (TRUE)
    {
        // Clearing the fd_set
        FD_ZERO(&readfds);
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;

        // Checking what are the sockets available
        for (i = 1; i < max_clients; i++)
        {
            sd = client_socket[i];

            if (sd > 0)
            {
                FD_SET(sd, &readfds);
            }
            if (sd > max_sd)
            {
                max_sd = sd;
            }
        }

        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR))
        {
            printf("select error");
        }

        // If a new client tries to connect to the server
        if (FD_ISSET(master_socket, &readfds))
        {

            if ((new_socket = accept(master_socket,(struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
            {
                cout << endl;
                error("Oops! Sorry, server - client connection couldn't be establised. Please try again :(");
            }

            cout << endl;
            cout << BOLDCYAN << "New client connection has been established with the port number " << ntohs(address.sin_port) << endl;
            cout << BOLDCYAN << "The client's name is Client " << cnt << RESET << endl;

            clients_existing.insert(cnt);
            char sending_its_name[MAX];
            int z = 0;
            sending_its_name[z] = '*';
            string its_name = to_string(cnt);
            while(z < its_name.size())
            {
                sending_its_name[z+1] = its_name[z];
                z++;
            }
            sending_its_name[z+1] = '\0';
            write(new_socket, sending_its_name, strlen(sending_its_name));

            fflush(stdout);

            for (i = 1; i < max_clients; i++)
            {
                if (client_socket[i] == 0)
                {
                    client_socket[i] = new_socket;
                    client[cnt] = i;
                    sockets[cnt] = new_socket;
                    cnt++;
                    break;
                }
            }
        }

        // Iterating through all the clients
        for (i = 1; i < max_clients; i++)
        {
            bzero(buffer, sizeof(buffer));
            sd = client_socket[i];

            // If the socket exists
            if (FD_ISSET(sd, &readfds))
            {
                // Reading a message from the client
                int valread = read(sd, buffer, 1500);

                char buffer1[MAX];
                bzero(buffer1, sizeof(buffer1));

                int k = 0;
                for (; buffer[k] != '\0'; k++)
                {
                    buffer1[k] = buffer[k];
                }
                buffer1[k] = '\0';

                // If the client wants to exit
                if (valread == 0 || strncmp(buffer1, "exit", 4) == 0)
                {
                    getpeername(sd, (struct sockaddr *)&address,(socklen_t *)&addrlen);
                    
                    for (auto &it : client)
                    {
                        if (it.second == i)
                        {
                            cout << endl;
	                        cout << BOLDMAGENTA << "Client " << it.first << "'s connection has been closed" << RESET << endl;
                            clients_existing.erase(it.first);
                            it.second = 0;
                            break;
                        }
                    }
                    close(sd);
                    client_socket[i] = 0;
                }
                else
                {
                    int cl = 0;
                    for (auto &it : client)
                    {
                        if (it.second == i)
                        {
                            cl = it.first;
                            break;
                        }
                    }

                    // If the client wants to broadcast
                    if(buffer1[0] == '0')
                    {
                        char buffer2[MAX];
                        bzero(buffer2, sizeof(buffer2));

                        string sending = to_string(cl);
                        buffer2[0] = '0';
                        int k = 0;
                        while(k < sending.size())
                        {
                            buffer2[k+1] = sending[k];
                            k++;
                        }
                        k++;
                        buffer2[k] = ' ';
                        k++;
                        int p = 1;
                        while(buffer1[p] != '\0')
                        {
                            buffer2[k] = buffer1[p];
                            k++;
                            p++;
                        }
                        buffer2[k] = '\0';

                        // Sending to all the other clients 
                        for(int j = 1; j < max_clients; j++)
                        {
                            if(client_socket[j] > 0 && i != j)
                            {
                                int sockfd = client_socket[j];
                                write(sockfd, buffer2, strlen(buffer2));
                            }
                        }
                    }
                    else if(buffer1[0] == '#')
                    {
                        char buffer2[MAX];
                        bzero(buffer2, sizeof(buffer2));
                        int p = 0;
                        buffer2[p] = '&';
                        p++;
                        buffer2[p] = ' ';
                        p++;
                        for(auto it = clients_existing.begin(); it != clients_existing.end(); it++)
                        {
                            // if(*it != i)
                            // {
                                string num = to_string(*it);
                                int q = 0;
                                while(q < num.size())
                                {
                                    buffer2[p] = num[q];
                                    p++;
                                    q++;
                                }
                                buffer2[p] = ' ';
                                p++;
                            // }
                        }
                        p--;
                        buffer2[p] = '\0';

                        int tempfd = sockets[cl];
                        send(tempfd, buffer2, strlen(buffer2), 0);
                    }
                    // If the client wants to send a private message to some other client
                    else
                    {
                        string clientnum = "";
                        int k = 0;
                        while(buffer1[k] != ' ')
                        {
                            clientnum += buffer1[k];
                            k++;
                        }
                        int x = stoi(clientnum);
                        int sockfd = sockets[x];

                        // If the client exists and sending, receiving clients are different
                        if(clients_existing.count(x) && x != cl)
                        {
                            char buffer2[MAX];
                            bzero(buffer2, sizeof(buffer2));

                            buffer2[0] = 'Y'; buffer2[1] = '\0';
                            int tempfd = sockets[cl];
                            send(tempfd, buffer2, strlen(buffer2), 0);

                            bzero(buffer2, sizeof(buffer2));

                            k++; int j = 0;
                            string sending = to_string(cl);
                            int p = 0;
                            while(p < sending.size())
                            {
                                buffer2[j] = sending[p];
                                j++;
                                p++;
                            }
                            buffer2[j] = ' ';
                            j++;
                            while(buffer1[k] != '\0')
                            {
                                buffer2[j] = buffer1[k];
                                j++;
                                k++;
                            }
                            buffer2[j] = '\0';

                            send(sockfd, buffer2, strlen(buffer2), 0);
                        }
                        // if sending and receiving clients are same
                        else if(x == cl)
                        {
                            char buffer2[MAX];
                            bzero(buffer2, sizeof(buffer2));

                            buffer2[0] = '%'; buffer2[1] = '\0';
                            int tempfd = sockets[cl];
                            send(tempfd, buffer2, strlen(buffer2), 0);
                        }
                        // if the client doesn't exist
                        else
                        {
                            char buffer2[MAX];
                            bzero(buffer2, sizeof(buffer2));

                            buffer2[0] = 'N'; buffer2[1] = '\0';

                            int tempfd = sockets[cl];
                            send(tempfd, buffer2, strlen(buffer2), 0);
                        }
                        
                    }
                }
            }
        }
    }
    return 0;
}