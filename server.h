
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define MAX_CLIENT_NUM 5

// Implementation info
#define NUM_LINES 256
#define LINE_SIZE 50

// Generic protocol info
#define OPCODE_LEN 2
#define LINE_SPECIFIER_LEN 2
#define REQ_LEN OPCODE_LEN+LINE_SPECIFIER_LEN+LINE_SIZE

// Server operations definition
#define EXIT "00"
#define ADD "01"
#define GET "10"
#define GETALL "11"

// Struct that defines the requests to our server
struct Request_Struct {
  char opcode[OPCODE_LEN];
  char line_hex[LINE_SPECIFIER_LEN];
  int line;
  char content[LINE_SIZE];
};


// We need one mutex for each line in the solution
pthread_mutex_t locks[NUM_LINES];

// Our main char matrix
char* editor[NUM_LINES];
