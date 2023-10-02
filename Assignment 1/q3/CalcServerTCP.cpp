#include <iostream>
#include <cstring>
#include <sstream>
#include <unistd.h>
#include <cmath>
#include <arpa/inet.h>

#define MSG_LEN 1000

std::string extract(char *input)
{
    double num1, num2, result;
    char op;

    // read into variables from stream
    std::istringstream iss(input);
    iss >> num1 >> op >> num2;

    if (!iss) // invalid input
    {
        std::cout << "Client error : Invalid input." << std::endl;
        return "Invalid input.";
    }

    switch (op)
    {
    case '+':
        result = num1 + num2;
        break;
    case '-':
        result = num1 - num2;
        break;
    case '*':
        result = num1 * num2;
        break;
    case '/':
        if (num2 == 0)
        {
            std::cout << "Client error : Division by zero is not allowed." << std::endl;
            return "Division by zero is not allowed.";
        }
        result = num1 / num2;
        break;
    case '^':
        result = std::pow(num1, num2);
        break;
    default:
        std::cout << "Unsupported operator." << std::endl;
    }
    std::ostringstream oss;
    oss << result;
    std::string output = oss.str(); // convert the result into string
    return output;
}

void error(std::string msg)
{
    std::cout << "Error : " << msg << '\n';
    exit(1);
}

std::string intToString(int value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        error("PORT number was not provided");
    }

    int server_socket, client_connect, portno, n;
    portno = atoi(argv[1]);

    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    socklen_t client_len;

    // initializing a socket connection
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        error("Socket cannot be created");
    }

    std::memset(&server_address, 0, sizeof(server_address));

    server_address.sin_family = AF_INET;         // IPv4
    server_address.sin_addr.s_addr = INADDR_ANY; // accepts from all IPs
    server_address.sin_port = htons(portno);     // port no

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        error("Binding Failed");
    }

    if (listen(server_socket, 10) < 0)
    {
        error("Listen failed");
    }

    std::cout << "Server is listening on PORT : " << portno << std::endl;

    client_len = sizeof(client_address);

    char buffer[1024];

    while (1)
    {
        client_connect = accept(server_socket, (struct sockaddr *)&client_address, &client_len);
        if (client_connect < 0)
        {
            error("Accept Failed");
        }

        // creates a fork
        int concurrent = fork();

        // retrive client address (IP and PORT number)
        char clientIP[INET_ADDRSTRLEN];
        int clientPort;
        inet_ntop(AF_INET, &(client_address.sin_addr), clientIP, INET_ADDRSTRLEN);
        clientPort = ntohs(client_address.sin_port);

        if (concurrent == -1) // if fork was unsuccessful
        {
            error("Count not establish connection with " + (std::string)clientIP + ":" + intToString(clientPort));
        }
        else if (concurrent == 0) // if fork successful
        {
            close(server_socket);
            std::cout << "Connected to client at " << clientIP << ":" << clientPort << "\n\n";

            while (1)
            {
                std::memset(&buffer, 0, MSG_LEN);

                // read from client
                n = read(client_connect, buffer, 1000);
                if (n < 0)
                {
                    error("Error on Reading");
                }
                else if (n == 0) // client crashes
                {
                    std::cout << "Client disconnected [" << clientIP << ":" << clientPort << "]\n";
                    break;
                }

                if (strncmp("/exit", buffer, 5) == 0) // exit command
                {
                    break;
                }
                std::string result = extract(buffer); // result
                std::cout << "Client at " << clientIP << ":" << clientPort << " asked\n";
                std::cout << "What is " << buffer << '\n';
                std::cout << "I replied : " << result << "\n\n";

                std::memset(&buffer, 0, MSG_LEN);

                n = write(client_connect, result.c_str(), result.size() + 1);

                if (n < 0)
                {
                    error("Error on Writing");
                }
            }

            std::cout << "Client at " << clientIP << ":" << clientPort << " closed connection.\n";
            close(client_connect);
            exit(0);
        }
    }
    close(server_socket);
    return 0;
}