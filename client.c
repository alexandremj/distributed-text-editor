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

    if (write(sockfd, &req, REQ_LEN) == -1) {
      perror("Error on write: ");
      // exit(2);
    }

    client_sleep();
    if (read(sockfd, &res, REQ_LEN) == -1) {
      perror("Error on read: ");
      // exit(3);
    }

    printf("response: ");
    for (int i = 0; i < REQ_LEN; i++){
      printf("%c", res[i]);
    }
    printf("\n\n");



    // TODO: this should be outside the loop, but currently placing it outside
    // breaks the application after the second request
    if (close(sockfd) == -1) {
      perror("Error on close: ");
      // exit(4);
    }
  }

	exit(0);
}


int main()
{
  size_t n_reqs = 11;
  char reqs[n_reqs][REQ_LEN];

  strcpy(reqs[0], "0100Hello, World!000000000000000000000000000000000000");
  strcpy(reqs[1], "0101Hello, World!000000000000000000000000000000000000");
  strcpy(reqs[2], "0102Hello, World!000000000000000000000000000000000000");
  strcpy(reqs[3], "0103Hello, World!000000000000000000000000000000000000");
  strcpy(reqs[4], "0104Hello, World!000000000000000000000000000000000000");
  strcpy(reqs[5], "0105Hello, World!000000000000000000000000000000000000");
  strcpy(reqs[6], "0106Hello, World!000000000000000000000000000000000000");
  strcpy(reqs[7], "0107Hello, World!000000000000000000000000000000000000");
  strcpy(reqs[8], "0108Hello, World!000000000000000000000000000000000000");
  strcpy(reqs[9], "0109Hello, World!000000000000000000000000000000000000");
  strcpy(reqs[10], "00000000000000000000000000000000000000000000000000000");

  run(n_reqs, reqs);
}
