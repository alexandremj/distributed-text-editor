#include "server.h"

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
  printf("Raw request: ");
  for (int i = 0; i < REQ_LEN; i++)
    printf("%c", raw_request[i]);
  
  printf("\n");
  struct Request_Struct req;

  for (int i = 0; i < OPCODE_LEN; i++) {
    req.opcode[i] = raw_request[i];
  }

  printf("Opcode: %c%c\n", req.opcode[0], req.opcode[1]);


  for (int i = OPCODE_LEN; i < OPCODE_LEN+LINE_SPECIFIER_LEN; i++) {
    req.line_hex[i-OPCODE_LEN] = raw_request[i];
  }
  printf("Line hex: %c%c\n", req.line_hex[0], req.line_hex[1]);


  printf("Content: ");
  for (int i = OPCODE_LEN+LINE_SPECIFIER_LEN; i < REQ_LEN; i++) {
    req.content[i] = raw_request[i];
    printf("%c", req.content[i]);
  }
  printf("\n");

  req.line = hex_translate(req.line_hex);
  printf("Line: %d\n", req.line);

  return req;
}

void exit_op(int client_sockfd)
{
  if (close(client_sockfd) == -1) {
    perror("Error on close: ");
  }
  return;
}


char* add_line(int line, char *content)
{
  if (line < 0 || line >= NUM_LINES) {
    return "--------------line number out of bounds---------------";
  }

  pthread_mutex_lock(&locks[line]);
  printf("\nLine %d before: %s\n", line, editor[line]);
  editor[line] = content;
  printf("\nLine %d after: %s\n", line, editor[line]);
  // for (int i = 0; i < LINE_SIZE; i++)
  //   strcpy(editor[line], content);
  pthread_mutex_unlock(&locks[line]);

  return "--------------------successful write--------------------";
}


char* get_line(int line)
{
  if (line < 0 || line >= NUM_LINES) {
    return "--------------line number out of bounds---------------";
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

int compare_opcode(char *opcode, char *operation)
{
  if (opcode[0] == operation[0] && opcode[1] == operation[1]) return 0;
  return 1;
}

void* run_client(void* arg)
{
  int client_sockfd = *(int *)arg;

  char raw_req[REQ_LEN];

  printf("\n Running client %d\n", client_sockfd);
  while(1) {
    if(read(client_sockfd, &raw_req, REQ_LEN) == -1) {
      perror("Error on read: ");
    }

    printf("Parsing request\n");

    struct Request_Struct req = parse_req(raw_req);
    char res[REQ_LEN] = "";

    printf("\nParsing response\n");

    // Application logic
    if (compare_opcode(req.opcode, EXIT) == 0) {
      printf("Exit response\n");
		  write(client_sockfd, &"--------------------successful exit--------------------", REQ_LEN);
      exit_op(client_sockfd);
      return NULL;
    } else if (compare_opcode(req.opcode, ADD) == 0) {
      printf("Add response\n");
      strcpy(res, add_line(req.line, req.content));
    } else if (compare_opcode(req.opcode, GET) == 0) {
      printf("Get response\n");
      strcpy(res, get_line(req.line));
    } else if (compare_opcode(req.opcode, GETALL) == 0) {
      printf("Get All response\n");
      strcpy(res, getall_op());
    } else {
      printf("Operation not found, exiting...-----------------------\n");
      strcpy(res, "Operation not found, exiting...----------------------");
    }

    for (int i = 0; i < REQ_LEN; i++) printf("%c", res[i]);
    printf("\n");
		if (write(client_sockfd, &res, REQ_LEN) == -1) {
      perror("Error on write: ");
    }
  }
}

int main()
{
  int server_sockfd, client_sockfd;
  int server_len, client_len;
  struct sockaddr_in server_address;
  struct sockaddr_in client_address;

  pthread_t threads[MAX_CLIENT_NUM+10];
  int connected_clients = 0;
  // Mutex initialization
  for (int i = 0; i < NUM_LINES; i++) pthread_mutex_init(&locks[i], NULL);

  // editor initialization
  for (int i = 0; i < NUM_LINES; i++) {
    editor[i] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
  }
  
  server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  server_address.sin_port = htons(9734);
  server_len = sizeof(server_address);
  bind(server_sockfd, (struct sockaddr *)&server_address, server_len);
	listen(server_sockfd, 5);

	while(1) {
		printf("\nServer waiting...\n");
		client_len = sizeof(client_address);
		client_sockfd = accept(server_sockfd,(struct sockaddr *)&client_address, &client_len);
    printf("CLIENT %d STARTING", client_sockfd);
    pthread_create(&threads[connected_clients], NULL, run_client, &client_sockfd);
    connected_clients++;
	}

  for (int i = 0; i < MAX_CLIENT_NUM; i++) {
      pthread_join(threads[i], NULL);
  }
}
