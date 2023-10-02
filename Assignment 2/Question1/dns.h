#include <string>
using std::string;

// DNS header structure
struct DNS_HEADER
{
    unsigned short id; // identification number

    unsigned char rd : 1;     // recursion desired
    unsigned char tc : 1;     // truncated message
    unsigned char aa : 1;     // authoritive answer
    unsigned char opcode : 4; // purpose of message
    unsigned char qr : 1;     // query/response flag

    unsigned char rcode : 4; // response code
    unsigned char cd : 1;    // checking disabled
    unsigned char ad : 1;    // authenticated data
    unsigned char z : 1;     // its z! reserved
    unsigned char ra : 1;    // recursion available

    unsigned short q_count;    // number of question entries
    unsigned short ans_count;  // number of answer entries
    unsigned short auth_count; // number of authority entries
    unsigned short add_count;  // number of resource entries
};

// Constant sized fields of query structure
struct QUESTION
{
    unsigned short qtype;
    unsigned short qclass;
};

// Constant sized fields of the resource record structure
#pragma pack(push, 1)
struct R_DATA
{
    unsigned short type;
    unsigned short _class;
    unsigned int ttl;
    unsigned short data_len;
};
#pragma pack(pop)

// Pointers to resource record contents
struct RES_RECORD
{
    unsigned char *name;
    struct R_DATA *resource;
    unsigned char *rdata;
};

// LinkedList node structure used in cache
struct LinkedList
{
    string url;
    char *results;
    LinkedList *next;

    LinkedList() // constructor
    {
        url = "";
        next = NULL;
    }
};

LinkedList *cache = new LinkedList();

// Check if a node exists in current Linked List or not...
LinkedList *In_Cache(string url)
{
    LinkedList *root = cache;
    while (root != NULL)
    {
        if (root->url == url)
        {
            return root;
        }
        root = root->next;
    }
    return NULL;
}

// Insert a node at start of Linked List....
void Insert(LinkedList *&node)
{
    if (cache->next == NULL)
    {
        cache->next = node;
        return;
    }
    LinkedList *temp = cache->next;
    cache->next = node;
    node->next = temp;
}

// Delete a node....
void Delete(LinkedList *&node)
{
    LinkedList *root = cache;
    LinkedList *prev = root;
    LinkedList *ahead = node->next;
    while (root != node)
    {
        prev = root;
        root = root->next;
    }
    prev->next = ahead;
}

// Evict the least recently used node from end of list..
void LRU_evict()
{
    LinkedList *root = cache;
    LinkedList *prev = root;
    while (root->next != NULL)
    {
        prev = root;
        root = root->next;
    }
    prev->next = NULL;
}

// Function Prototypes
void ModifyHostnameToDNF(unsigned char *, unsigned char *);
unsigned char *DecodeResponse(unsigned char *, unsigned char *, int *);
void read_answers(unsigned char buf[], unsigned char *reader, struct DNS_HEADER *dns);
void extract_result(char *res);
void gethostbyname(unsigned char *host, int query_type, LinkedList *&node);
