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

#define INET_ADDR "127.0.0.1"

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

char* parse_opcode(char input);
void handle_op(char* opcode);
void client_cli();

#define CLI_STRING "\n\
Client CLI - Please type a command: \n\n\
  0 - exit\n\
  1 - add_line \n\
  2 - get_line \n\
  3 - get_all_lines \n\
"

// These functions are here because I think it's too basic to be on code
// Other languages have this by standard
unsigned int int_to_int(unsigned int k) {
  return (k == 0 || k == 1 ? k : ((k % 2) + 10 * int_to_int(k / 2)));
}

char* int_to_str(int x)
{
  int length = snprintf( NULL, 0, "%d", x );
  char* str = malloc( length + 1 );
  snprintf( str, length + 1, "%d", x );
  return str;
}

char* int_to_bin(int x) { return int_to_str(int_to_int(x)); }

int char_to_int(char c) { return (int) c - 48; }

char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}