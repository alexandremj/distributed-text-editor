#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

// implementation info
#define N_LINES 10
#define N_COLUMNS 50

// generic protocol info
#define OPCODE_LEN 2
#define CONTENT_LEN 50
#define REQ_LEN OPCODE_LEN+N_COLUMNS

// server operations definition
#define EXIT "00"
#define ADD "01"
#define GET "10"
#define GETALL "11"

// returns a printable string representation of the editor as received by the
// local client
char* printable_editor(char **editor) {
  char* printable_string;

  for(int i = 0; i < N_LINES; i++) {
    printable_string += i + '0';

    for (int j = 0; i < N_COLUMNS; j++) {
      printable_string += editor[N_LINES][N_COLUMNS];
    }

    printable_string += '\n';
  }

  return printable_string;
}


// socket instantiation and execution
int main()
{
  int sockfd;
  int len;
  struct sockaddr_in address;
  int result;
  char *req;
  char *res;

  char local_editor[N_LINES][N_COLUMNS];

  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  address.sin_family = AF_INET;
  //address.sin_addr.s_addr = inet_addr("127.0.0.1");
  address.sin_addr.s_addr = inet_addr("192.168.0.15");
  //address.sin_addr.s_addr = htonl(INADDR_ANY);
  address.sin_port = htons(9734);

  len = sizeof(address);
  result = connect(sockfd, (struct sockaddr *)&address, len);
  if(result == -1) {
		perror("oops: client1");
		exit(1);
  }
	write(sockfd, req, REQ_LEN);
	sleep(30);
	read(sockfd, res, REQ_LEN);
	printf("Response from server = %c\n", res);
	close(sockfd);
	exit(0);
}
