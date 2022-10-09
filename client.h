#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

// Implementation info
#define NUM_LINES 256
#define LINE_SIZE 50

// Generic protocol info
#define OPCODE_LEN 2
#define LINE_SPECIFIER_LEN 2
#define REQ_LEN OPCODE_LEN+LINE_SPECIFIER_LEN+LINE_SIZE
#define CLIENT_SLEEP_TIME 2

// Server operations definition
#define EXIT "00"
#define ADD "01"
#define GET "10"
#define GETALL "11"

#define INET_ADDR "200.135.76.223"

int sockfd;
int len;
struct sockaddr_in address;
int result;
char req[REQ_LEN];
char res[REQ_LEN];

// local copy of the editor, as last seen by a get_all operation
char local_editor[NUM_LINES][LINE_SIZE];

// returns a printable version of the text editor
char* printable_editor(char **editor);

// wraps the socket connection logic for connection
void client_connect();

// standardizes the sleep() calls across client code
void client_sleep();

// main client application logic
void run(size_t n_reqs, char reqs[n_reqs][REQ_LEN]);
