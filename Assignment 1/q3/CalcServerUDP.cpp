// server program for udp connection
#include <iostream>
#include <sstream>
#include <cmath>
#include <cstring>
#include <string.h>
#include <arpa/inet.h>

#define MSG_LEN 1000

void error(const char *msg)
{
    std::cout << msg << '\n';
    exit(1);
}

std::string extract(char *input)
{
    double num1, num2, result;
    char op;

    std::istringstream iss(input);
    iss >> num1 >> op >> num2;

    if (!iss)
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
    std::string output = oss.str();
    return output;
}

// Driver code
int main(int argc, char *argv[])
{
    // if less arguments provided
    if (argc < 2)
    {
        error("PORT number was not provided");
    }

    char buffer[1024];
    int server_socket;
    int portno = atoi(argv[1]);
    struct sockaddr_in server_address, client_address;

    // clear the server address
    std::memset(&server_address, 0, sizeof(server_address));
    // Create a UDP Socket
    server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(portno);
    server_address.sin_family = AF_INET;
    

    // bind server address to socket descriptor
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)))
    {
        error("Bind failed");
    }

    std::cout << "Server is linstening on PORT " << portno << '\n';

    while (1)
    {
        std::memset(&buffer, 0, MSG_LEN);

        // receive the datagram
        socklen_t len = sizeof(client_address);
        int n = recvfrom(server_socket, buffer, sizeof(buffer),
                        0, (struct sockaddr *)&client_address, &len); // receive message from server

        // retrive client address (IP and PORT number)
        char clientIP[INET_ADDRSTRLEN];
        int clientPort;
        inet_ntop(AF_INET, &(client_address.sin_addr), clientIP, INET_ADDRSTRLEN);
        clientPort = ntohs(client_address.sin_port);

        if (n == 0) // if client crashes
        {
            std::cout << "Client at " << clientIP << ":" << clientPort << " closed connection.\n";
            std::cout << "Server is linstening on PORT " << portno << "\n\n";
            continue;
        }

        buffer[n] = '\0';
        if (strncmp(buffer, "/exit", 5) == 0) // if exit sentinel provided
        {
            std::cout << "Client at " << clientIP << ":" << clientPort << " closed connection.\n";
            std::cout << "Server is linstening on PORT " << portno << "\n\n";
            continue;
        }

        // evaluate the expression
        std::string result = extract(buffer);
        std::cout << "Client at " << clientIP << ":" << clientPort << " asked\n";
        std::cout << "\tWhat is " << buffer;
        std::cout << "\tI replied : " << result << "\n\n"; // print answer

        // send the response
        sendto(server_socket, result.c_str(), result.size() + 1, 0,
               (struct sockaddr *)&client_address, sizeof(client_address));
    }
}
