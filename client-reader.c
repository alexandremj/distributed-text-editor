#include "client.h"

char* printable_editor(char **editor)
{
  char* printable_string;

  for (int i = 0; i < NUM_LINES; i++) {
    printable_string += i + '0';

    for (int j = 0; i < LINE_SIZE; j++) {
      printable_string += editor[NUM_LINES][LINE_SIZE];
    }

    printable_string += '\n';
  }

  return printable_string;
}


void client_connect()
{
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = inet_addr(INET_ADDR);

  //address.sin_addr.s_addr = htonl(INADDR_ANY);
  address.sin_port = htons(9734);

  len = sizeof(address);
  result = connect(sockfd, (struct sockaddr *)&address, len);
  if(result == -1) {
		perror("oops: client1");
		exit(1);
  }
}


void client_sleep()
{
  sleep(CLIENT_SLEEP_TIME);
}


void run(size_t n_reqs, char reqs[n_reqs][REQ_LEN])
{
  client_connect();
  for (int req_counter = 0; req_counter < n_reqs; req_counter++) {
    strcpy(req, reqs[req_counter]);
    printf("request: ");
    for (int i = 0; i < REQ_LEN; i++){
      printf("%c", req[i]);
    }
    printf("\n");

    printf("1write\n");
    if (write(sockfd, &req, REQ_LEN) == -1) {
      perror("Error on write: ");
      // exit(2);
    }
    printf("2write\n");

    client_sleep();
    printf("1read\n");
    if (read(sockfd, &res, REQ_LEN) == -1) {
      perror("Error on read: ");
      // exit(3);
    }
    printf("2read\n");

    printf("response: ");
    for (int i = 0; i < REQ_LEN; i++){
      printf("%c", res[i]);
    }
    printf("\n\n");



    // TODO: this should be outside the loop, but currently placing it outside
    // breaks the application after the second request
    // if (close(sockfd) == -1) {
    //   perror("Error on close: ");
    //   // exit(4);
    // }
  }

	exit(0);
}

void run_test() {
  size_t n_reqs = 11;
  char reqs[n_reqs][REQ_LEN];

  strcpy(reqs[0], "1000Hello, World!000000000000000000000000000000000000");
  strcpy(reqs[1], "1001Hello, World!000000000000000000000000000000000000");
  strcpy(reqs[2], "1002Hello, World!000000000000000000000000000000000000");
  strcpy(reqs[3], "1003Hello, World!000000000000000000000000000000000000");
  strcpy(reqs[4], "1004Hello, World!000000000000000000000000000000000000");
  strcpy(reqs[5], "1005Hello, World!000000000000000000000000000000000000");
  strcpy(reqs[6], "1006Hello, World!000000000000000000000000000000000000");
  strcpy(reqs[7], "1007Hello, World!000000000000000000000000000000000000");
  strcpy(reqs[8], "1008Hello, World!000000000000000000000000000000000000");
  strcpy(reqs[9], "1009Hello, World!000000000000000000000000000000000000");
  strcpy(reqs[10], "0000000000000000000000000000000000000000000000000000a");

  run(n_reqs, reqs);
}

void op_add_line()
{
  printf("Adding line\n");
}

void op_get_line()
{
  printf("getting line X\n");
}

void op_get_all()
{
  printf("Getting ALL\n");
}

char* parse_opcode(char input)
{
  int int_input = char_to_int(input);
  if (int_input > 3) return "XX";
  char* bin = int_to_bin(int_input);
  return ( strlen(bin) == 1 ? concat("0", bin) : bin);
}

void handle_op(char* opcode)
{
  if (!strcmp(opcode, "XX")) return;
  if (!strcmp(opcode, ADD)) op_add_line();
  if (!strcmp(opcode, GET)) op_get_line();
  if (!strcmp(opcode, GETALL)) op_get_all();
}

void client_cli()
{
  char user_input;
  do {
    user_input = '-';
    printf(CLI_STRING);
    scanf("%s", &user_input);

    char* parsed_input = parse_opcode(user_input);
    printf("User input: %s\n", parsed_input);

    handle_op(parsed_input);
  } while (user_input != '0');
}

int main()
{
  run_test();
  //client_connect();
  // client_cli();
  //exit(0)
}
