#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

// Implementation info
#define NUM_LINES 10
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
char editor[NUM_LINES][LINE_SIZE];


int get_hex_value(char hex) 
{
  switch (hex)
  {
    case '0':
      return 0;

    case '1':
      return 1;

    case '2':
      return 2;

    case '3':
      return 3;

    case '4':
      return 4;
    
    case '5':
      return 5;
    
    case '6':
      return 6;

    case '7':
      return 7;

    case '8':
      return 8;

    case '9':
      return 9;

    case 'a':
      return 10;

    case 'b':
      return 11;
    
    case 'c':
      return 12;
    
    case 'd':
      return 13;
    
    case 'e':
      return 14;
    
    case 'f':
      return 15;
  }
}

// Please note this assumes len(hex_string) == 2
// Variations will need different implementations
int hex_translate(char *hex_string)
{
  int result = 0;
  result += get_hex_value(hex_string[1]);
  result += get_hex_value(hex_string[0]) * 16;

  return result;
}

struct Request_Struct parse_req(char *raw_request)
{
  struct Request_Struct req;
  for (int i = 0; i < OPCODE_LEN; i++) {
    req.opcode[i] = raw_request[i];
  }

  for (int i = OPCODE_LEN; i < OPCODE_LEN+LINE_SPECIFIER_LEN; i++) {
    req.line_hex[i] = raw_request[i];
  }

  for (int i = OPCODE_LEN+LINE_SPECIFIER_LEN; i < REQ_LEN; i++) {
    req.content[i] = raw_request[i];
  }

  req.line = hex_translate(req.line_hex);

  return req;
}

char* exit_op()
{
  return "successful exit\n"; // TODO: make all returns have REQ_LEN length
}


char* add_line(int line, char *content)
{
  if (line < 0 || line >= NUM_LINES) {
    return "line number out of bounds\n";
  }

  pthread_mutex_lock(&locks[line]);
  for (int i = 0; i < LINE_SIZE; i++)
    editor[line][i] = content[i];
  pthread_mutex_unlock(&locks[line]);

  return "successful write\n";
}


char* get_line(int line)
{
  if (line < 0 || line >= NUM_LINES) {
    return "line number out of bounds\n";
  }

  pthread_mutex_lock(&locks[line]);
  char *r = editor[line];
  pthread_mutex_unlock(&locks[line]);
  return r;
}


char* getall_op()
{
  char *lines = "";
  char nullstring = '\n';
  for (int i = 0; i < NUM_LINES; i++) {
      strcat(lines, get_line(i));
      strcat(lines, &nullstring);
  }
  return lines;
}


int main()
{
  int server_sockfd, client_sockfd;
  int server_len, client_len;
  struct sockaddr_in server_address;
  struct sockaddr_in client_address;
  char *raw_req;

  // Mutex initialization
  for (int i = 0; i < NUM_LINES; i++) pthread_mutex_init(&locks[i], NULL);

  // TODO: make server actually run
  
  server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
  server_address.sin_family = AF_INET;
  //server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
  //server_address.sin_addr.s_addr = inet_addr("192.168.0.15");
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  server_address.sin_port = htons(9734);
  server_len = sizeof(server_address);
  bind(server_sockfd, (struct sockaddr *)&server_address, server_len);
	listen(server_sockfd, 5);
	while(1) {
		printf("server waiting\n");
		client_len = sizeof(client_address);
		client_sockfd = accept(server_sockfd,(struct sockaddr *)&client_address, &client_len);
		read(client_sockfd, raw_req, REQ_LEN);

    struct Request_Struct req = parse_req(raw_req);
    char* res;

    // Application logic
    if (strcmp(req.opcode, EXIT) == 0) {
      res = exit_op();
    } else if (strcmp(req.opcode, ADD) == 0) {
      res = add_line(req.line, req.content);
    } else if (strcmp(req.opcode, GET) == 0) {
      res = get_line(req.line);
    } else if (strcmp(req.opcode, GETALL) == 0) {
      res = getall_op();
    }
		write(client_sockfd, res, REQ_LEN);
		close(client_sockfd);
	}
}
