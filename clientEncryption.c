#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netdb.h>
#include <stdbool.h>
#include <fcntl.h>
#include <ctype.h>

#define MAX 900000

/**
EXAMPLE OF OTP CLIENT ENCRYPTION LOGIC
*/

// Set up the address struct
void setupAddressStruct(struct sockaddr_in* address, 
                        int portNumber, 
                        char* hostname){
  memset((char*) address, '\0', sizeof(*address));
  address->sin_family = AF_INET;
  address->sin_port = htons(portNumber);
  struct hostent* hostInfo = gethostbyname(hostname);
  if (hostInfo == NULL) { 
    fprintf(stderr, "CLIENT: ERROR, no such host\n"); 
    exit(0); 
  }
  memcpy((char*) &address->sin_addr.s_addr,
        hostInfo->h_addr_list[0],
        hostInfo->h_length);
}

void create_msg(char *buffer, const char *plaintext, const char *key) {
  FILE *fp;
  char line[MAX];
  strcpy(buffer, "E\n");
  fp = fopen(plaintext, "r");
  fgets(line, sizeof(line), fp);
  fclose(fp);
  strcat(buffer, line);
  strcat(buffer, "\n");
  memset(line, '\0', sizeof(line));
  fp = fopen(key, "r");
  fgets(line, sizeof(line), fp);
  fclose(fp);
  strcat(buffer, line);
  strcat(buffer, "\n");
  strcat(buffer, "eom\n");
}

int main(int argc, char *argv[]){
  int socketFD, portNumber, charsWritten, charsRead;
  struct sockaddr_in serverAddress;
  char buffer[MAX];

  // Get file lengths
  int plain_text = open(argv[1], O_RDONLY);
  int plain_length = lseek(plain_text, 0 , SEEK_END);
  int key_text = open(argv[2], O_RDONLY);
  int key_length = lseek(key_text, 0, SEEK_END);

  setupAddressStruct(&serverAddress, atoi(argv[3]), "localhost");
  if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){
      perror("CLIENT ERROR");
      exit(1);
  }

  create_msg(buffer, argv[1], argv[2]);

  charsWritten = send(socketFD, buffer, strlen(buffer), 0);
  if (charsWritten < 0){
      perror("CLIENT ERROR");
      exit(1);
  }
  if (charsWritten < strlen(buffer)){
    printf("CLIENT: WARNING: Not all data written to socket!\n");
  }
  memset(buffer, '\0', sizeof(buffer));

  while(!strstr(buffer, "eom\n")) {
    char inbuffer[MAX];
    memset(inbuffer, '\0', sizeof(inbuffer));

    charsRead = recv(socketFD, inbuffer, sizeof(inbuffer), 0);
    if (charsRead < 0){
        perror("CLIENT ERROR");
        exit(1);
        
    }
    strcat(buffer, inbuffer);
  }

  char *from = strtok(buffer, "\n");
  if(from[0] == 'B'){
      perror("CLIENT ERROR");
      exit(1);
  }

  else{
    fprintf(stdout, "%s\n", strtok(NULL, "\n"));
  }

  close(socketFD);
  return 0;
}
