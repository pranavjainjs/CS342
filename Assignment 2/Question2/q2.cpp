// Including necessary Libraries.......
#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include<bits/stdc++.h>
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
using namespace std;

int sz;  // --> Size of Cache.

// Linked List structure for Caching..
struct LinkedList{
    string url;
    char * results;
    LinkedList*next;
    LinkedList(){
        url="";
        next=NULL;
    }
};

LinkedList*cache=new LinkedList();

// Check if a node exists in current Linked List or not...
bool In_Cache(string url)
{
    LinkedList*root=cache;
    while (root!=NULL)
    {
        if (root->url==url)
        {
            return true;
        }
        root=root->next;
    }
    return false;
}

// Insert a node at start of Linked List....
void Insert(LinkedList*&node){
    if (cache->next==NULL)
    {
        cache->next=node;
        return ;
    }
    LinkedList*temp=cache->next;
    cache->next=node;
    node->next=temp;
}
// Delete a node....
void Delete(LinkedList*&node){
    LinkedList*root=cache;
    LinkedList*prev=root;
    LinkedList*ahead=node->next;
    while (root!=node)
    {
        prev=root;
        root=root->next;
    }
    prev->next=ahead;
    // delete(node);
}

void Display()
{
    LinkedList*head=cache;
    head=head->next;
    cout<<"Order of urls stored in cache from Most recently accessed to Least recently accessed: "<<endl;
    while (head)
    {
        cout<<head->url<<endl;
        head=head->next;
    }
    
}

// Evict the least recently used node from end of list..
void LRU_evict(){
    LinkedList*root=cache;
    LinkedList*prev=root;
    while (root->next!=NULL)
    {
        prev=root;
        root=root->next;
    }
    prev->next=NULL;
    // delete(root);
}

int main() 
{
    sz=0;
    cache->url="-1";                                        // This is just head of the linked list...
    // Creating a client socket, clientsocket is the file descriptor
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);  
    // Here AF_INET means iPv4 address family
    // Here SOCK_STREAM means TCP protocol
    // If the value of the file descriptor is -1, it means the socket couldn't be created. If it's 0, it's been created.


    // If the client socket couldn't be created....
    if (clientSocket == -1) {
        perror("Socket creation failed");
        return 1;
    }

    cout<<"Enter Domain Name: ";           // Enter HostName such as google.com or atcoder.jp 
    string host_name; 
    cin >> host_name;
    cout<<"Enter desired Cache Size: ";
    int mx_sz;                          // Max Size of Cache.
    cin>>mx_sz;
    const char * s = host_name.c_str();
    struct hostent *host;               
	host = gethostbyname(s);            // Get the IP address of the HostName.
    struct sockaddr_in server;    
    server.sin_port = htons(80);    // The htons() function converts the unsigned short integer hostshort from host byte order to network byte order.
    server.sin_family = AF_INET;        // For IPv4
    server.sin_addr.s_addr = *((unsigned long*)host->h_addr);
	socklen_t length = sizeof(struct sockaddr_in);
    memset(&(server.sin_zero), 0, 8);
    // Description of connect()
    // connect connects the clientsocket to the server. If the connection is successful, it returns 0. Otherwise, it returns -1
    if(connect(clientSocket, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
    {
        printf("Error with client connecting to server\n");
        close(clientSocket);
        return 0;
    }
    while(1){
        cout<<"Do you want to exit? (Enter 'Y' if you want to): ";
        string s;cin>>s;
        if (s=="Y")
        {
            break;
        }
        cout<<"Enter url :";        // Enter the whole url of the page whose contents you want. eg) ()
        string url;cin>>url;
        // If we found the url already in cache we can access contents from the cache and just update the url to the start of the cache.
        if (In_Cache(url))
        {
            cout<<"Url found in Cache"<<endl;
            LinkedList*node;
            // char buf[100000 + 1];
            char*buf;
            LinkedList*root=cache;
            while (root!=NULL)
            {
                if (root->url==url)
                {
                    node=root;
                    buf=root->results;
                }
                root=root->next;
            }
            LinkedList*temp=node;
            Delete(node);
            cout<<"Deleted the node"<<endl;
            Insert(temp);                   // Insert at start of linked list...
            cout<<"Inserted the node"<<endl;
            cout<<"--------------------------------------------Start--------------------------------------"<<endl;
            puts(buf);
            printf("------------------------------------------------------------------------------\n");
            cout<<endl;
            cout<<"Do you want to have a look at the contents of the cache? (Enter 'Y' if you want to) ";
            cin>>s;
            cout<<endl;
            if (s=="Y")
            {
                Display();
            }
            cout<<endl;
            continue;
        }
        LinkedList*node=new LinkedList();
        string page="";                     // We store the file path without the hostname in this variable.. 
        int i=0;
        int cnt=0;
        while (i<url.size() and cnt<3)
        {
            if (url[i]=='/')
            {
                cnt++;
            }
            i++;
        }
        page=url.substr(i,url.size()-i+1);
        // Create the HTTP GET request
        string a = "GET /"+page+" HTTP/1.1\r\n";
        string b  = "Host: " + host_name + "\r\n";
        string c="Connection: close\r\n"
                                      "\r\n";
        // string c = "\n";
        string p = a + b + c;
        const char * httpreq = p.c_str();           
        cout<<p<<endl;
        printf("----------------------Start Send-------------------------------\n");
        int send_result = send(clientSocket, httpreq, strlen(httpreq), 0);

        printf("send_result: %d\n", send_result);

        char buf[100000 + 1] = {0};

        printf("-------------------------Start Recv----------------------\n");

        int bytes_readed = recv(clientSocket, buf, 100000, 0);
        printf("end recv: read %d bytes\n", bytes_readed);

        buf[100000] = '\0';

        printf("-------------------------------------------------------------------------- buf:\n");
        puts(buf);
        printf("------------------------------------------------------------------------------\n");
        cout<<endl;
        node->url=url;
        node->next=NULL;
        node->results=buf;
        // if Cache is full perform LRU Evict...
        if (sz==mx_sz)
        {
            cout<<"Cache Full...Evicting"<<endl;
            LRU_evict();
            cout<<"LRU Eviction done"<<endl;
            cout<<endl;
            Insert(node);
        }
        else
        {
            cout<<"Cache Not Full...Inserting"<<endl;
            cout<<endl;
            Insert(node);
            sz++;
        }
        cout<<"Do you want to have a look at the contents of the cache? (Enter 'Y' if you want to) ";
        cin>>s;
        cout<<endl;
        if (s=="Y")
        {
            Display();
        }
        cout<<endl;
    }
    close(clientSocket);
    return 0;
}