// Necessary header files
#include <bits/stdc++.h>
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

using namespace std;

// For color printing in linux terminal
#define RESET "\033[0m"
#define BLACK "\033[30m"              /* Black */
#define RED "\033[31m"                /* Red */
#define GREEN "\033[32m"              /* Green */
#define YELLOW "\033[33m"             /* Yellow */
#define BLUE "\033[34m"               /* Blue */
#define MAGENTA "\033[35m"            /* Magenta */
#define CYAN "\033[36m"               /* Cyan */
#define WHITE "\033[37m"              /* White */
#define BOLDBLACK "\033[1m\033[30m"   /* Bold Black */
#define BOLDRED "\033[1m\033[31m"     /* Bold Red */
#define BOLDGREEN "\033[1m\033[32m"   /* Bold Green */
#define BOLDYELLOW "\033[1m\033[33m"  /* Bold Yellow */
#define BOLDBLUE "\033[1m\033[34m"    /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m" /* Bold Magenta */
#define BOLDCYAN "\033[1m\033[36m"    /* Bold Cyan */
#define BOLDWHITE "\033[1m\033[37m"   /* Bold White */

// Some required declarations
const int MAX = 1500;
const char *addres = "127.0.0.1";
#define SIZE 1000

// Global variables
int sockfd;
bool ya = false;
set<int> st;
bool hai = false;
char buffer_temp[MAX];
int option;

// Error message
void error(string msg)
{
    cout << BOLDRED << msg << RESET << endl;
    cout << endl;
    exit(0);
}

// Read function
void Read()
{
    // Loop goes on till the client disconnects
    while (ya == false)
    {
        // Declaring a buffer to store the message
        char buffer[MAX];
        bzero(buffer, MAX);

        // Reading the message
        read(sockfd, buffer, sizeof(buffer));

        // If the client received a message
        if (buffer[0] != '\0')
        {
            // If it is a broadcasted message
            if (buffer[0] == '0')
            {
                int i = 1;
                string clientid = "";
                while (buffer[i] != ' ')
                {
                    clientid += buffer[i];
                    i++;
                }
                i++;
                string msg = "";
                while (buffer[i] != '\0')
                {
                    msg += buffer[i];
                    i++;
                }
                cout << BOLDMAGENTA << "Message Broadcasted by Client " << clientid << ": ";
                cout << BOLDCYAN << msg << RESET << endl;
            }
            // If the server has been closed
            else if (buffer[0] == '$')
            {
                cout << endl;
                cout << BOLDRED << "Server has been closed. Thus, we are closing your connection." << endl;
                cout << endl;
                cout << BOLDRED << "Closing............................." << endl;
                cout << endl;
                sleep(3);
                cout << BOLDRED << "Successfully closed!" << endl;
                cout << endl;
                exit(0);
            }
            else if (buffer[0] == '%') // If the client tries to message itself
            {
                cout << BOLDRED << "Hey! You can't message yourself." << RESET << endl;
            }
            else if (buffer[0] == '&') // If the client is trying to get information about other clients
            {
                cout << BOLDMAGENTA << "The Client Ids of the clients that exist in the server are: ";
                string cll = "";
                int i = 2;
                while (buffer[i] != '\0')
                {
                    cll += buffer[i];
                    i++;
                }
                cout << BOLDCYAN << cll << RESET << endl;
            }
            else if (buffer[0] == '*') // When client gets its id
            {
                string yourclientid = "";
                int i = 1;
                while (buffer[i] != '\0')
                {
                    yourclientid += buffer[i];
                    i++;
                }
                cout << BOLDBLUE << "Your Client Id is " << BOLDWHITE << yourclientid << RESET << endl;
            }
            else if (buffer[0] == 'Y') // If the client exists
            {
                cout << BOLDMAGENTA << "Message sent to the Client " << option << ": ";
                cout << BOLDCYAN << buffer_temp << RESET << endl;
            }
            else if (buffer[0] == 'N') // If the client doesn't exist
            {
                cout << BOLDRED << "Oops! Sorry, this client doesn't exist." << RESET << endl;
            }
            else // If it is a private message
            {
                int i = 0;
                string clientid = "";
                while (buffer[i] != ' ')
                {
                    clientid += buffer[i];
                    i++;
                }
                i++;
                string msg = "";
                while (buffer[i] != '\0')
                {
                    msg += buffer[i];
                    i++;
                }
                cout << BOLDMAGENTA << "Message Received from Client " << clientid << ": ";
                cout << BOLDCYAN << msg << RESET << endl;
            }
            bzero(buffer, MAX);
        }
    }
}

// Main function
int main(int argc, char *argv[])
{
    // If IP address and Port Number are not provided in the command line
    if (argc < 3)
    {
        cout << endl;
        error("Please provide IP address and server's Port Number");
    }

    // Socket creation
    struct sockaddr_in servaddr, clienaddr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1)
    {
        cout << endl;
        error("Oops! Socket creation failed. Try again.");
    }
    else
    {
        cout << endl;
        cout << BOLDGREEN << "Client socket has been created successfully!" << RESET << endl;
    }
    fflush(stdout);

    // Necessary
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    int port = stoi(argv[2]);
    servaddr.sin_port = htons(port);

    // Connecting to the server
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        cout << endl;
        error("Oops! Failed to connect to the server. Try again.");
    }
    else
    {
        cout << endl;
        cout << BOLDGREEN << "Successfully connected to the server!" << RESET << endl;
        cout << endl;
        fflush(stdout);
    }

    // Thread to read
    thread t2(Read);

    // Directions to send messsages
    cout << BOLDBLUE << "To broadcast a message, type the message in the following manner." << endl;
    cout << "If the message you want to send is 'Hello, how are you ?', then type," << endl;
    cout << BOLDWHITE << "0Hello, how are you ?" << endl;
    cout << endl;

    cout << BOLDBLUE << "To send a private message, type the message in the following manner." << endl;
    cout << "If the message you want to send is 'Hello, how are you ?' and the Client Id is <client_id>, then type," << endl;
    cout << BOLDWHITE << "<client_id>Hello, how are you?" << endl;
    cout << endl;

    cout << BOLDBLUE << "To find the Client Ids of the clients connected to the server,type " << BOLDWHITE << "'0INFO'." << RESET << endl;
    cout << endl;

    // Loop runs
    while (1)
    {
        char buffer[MAX];
        bzero(buffer, MAX);

        // Taking input for broadcasting or sending a privat message or exiting
        cin >> option;

        // To close the client
        if (option == -1)
        {
            buffer[0] = 'e';
            buffer[1] = 'x';
            buffer[2] = 'i';
            buffer[3] = 't';
            buffer[4] = '\0';
            char *p = buffer;

            write(sockfd, p, sizeof(p));
            close(sockfd);

            cout << endl;
            cout << BOLDYELLOW << "You've successfully disconnected." << endl;
            cout << endl;
            cout << BOLDYELLOW << "Bye!" << RESET << endl;
            cout << endl;
            break;
        }
        // Otherwise
        else
        {
            // Taking input
            cin.getline(buffer_temp, MAX);

            // To broadcast
            if (option == 0)
            {
                if (buffer_temp[0] == 'I' && buffer_temp[1] == 'N' && buffer_temp[2] == 'F' && buffer_temp[3] == 'O' && buffer_temp[4] == '\0')
                {
                    buffer[0] = '#';
                    buffer[1] = '\0';
                    write(sockfd, buffer, strlen(buffer));
                    bzero(buffer, sizeof(buffer));
                }
                else
                {
                    int i = 0;
                    buffer[0] = '0';
                    while (buffer_temp[i] != '\0')
                    {
                        buffer[i + 1] = buffer_temp[i];
                        i++;
                    }
                    buffer[i + 1] = '\0';

                    cout << BOLDMAGENTA << "Message sent for Broadcasting: ";
                    cout << BOLDCYAN << buffer_temp << RESET << endl;
                    write(sockfd, buffer, strlen(buffer));
                    bzero(buffer, sizeof(buffer));
                }
            }
            // To send a private message
            else
            {
                string clientnum = to_string(option);
                int i = 0;
                while (i < clientnum.size())
                {
                    buffer[i] = clientnum[i];
                    i++;
                }
                buffer[i] = ' ';
                i++;
                int j = 0;
                while (buffer_temp[j] != '\0')
                {
                    buffer[i] = buffer_temp[j];
                    i++;
                    j++;
                }
                buffer[i] = '\0';
                write(sockfd, buffer, strlen(buffer));
                bzero(buffer, sizeof(buffer));
            }
        }
    }
    ya = true;
    t2.join();
    return 0;
}