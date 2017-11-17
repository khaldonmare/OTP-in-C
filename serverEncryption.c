#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

#define MAX 900000

void setupAddressStruct(struct sockaddr_in *address, int portNumber)
{
  memset((char *)address, '\0', sizeof(*address));
  address->sin_family = AF_INET;
  address->sin_port = htons(portNumber);
  address->sin_addr.s_addr = INADDR_ANY;
}

char *encryption(char *buffer)
{
  char *cipher_text = calloc((MAX), sizeof(char));
  memset(cipher_text, '\0', MAX);
  char *plain_text = calloc((MAX), sizeof(char));
  memset(plain_text, '\0', MAX);
  char *key = calloc((MAX), sizeof(char));
  memset(key, '\0', MAX);

  plain_text = strtok(buffer, "\n"); // Get the first line
  strcat(cipher_text, plain_text);   // Copy the first line to line

  strcat(cipher_text, "\n"); // Add a newline character to line

  plain_text = strtok(NULL, "\n"); // Get the second line
  key = strtok(NULL, "\n");        // Get the third line

  char chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
  int plain_index;
  int key_index;

  // Encrypt the message
  for (int i = 0; i < strlen(plain_text); ++i)
  {
    for (plain_index = 0; plain_text[i] != chars[plain_index]; ++plain_index)
    {
    }

    for (key_index = 0; key[i] != chars[key_index]; ++key_index)
    {
    }

    int sum = (plain_index + key_index) % 27;
    strncat(cipher_text, &chars[sum], 1);
  }
  strcat(cipher_text, "\neom\n");
  return cipher_text;
}

int main(int argc, char *argv[])
{
  int connectionSocket, charsRead;
  char buffer[MAX];
  struct sockaddr_in serverAddress, clientAddress;
  socklen_t sizeOfClientInfo = sizeof(clientAddress);
  int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (listenSocket < 0)
  {
      perror("ERROR SERVER");
      exit(1);
  }
  setupAddressStruct(&serverAddress, atoi(argv[1]));
  if (bind(listenSocket,
           (struct sockaddr *)&serverAddress,
           sizeof(serverAddress)) < 0)
  {
      perror("ERROR SERVER");
      exit(1);
  }

  listen(listenSocket, 5);

  while (1)
  {
    connectionSocket = accept(listenSocket,
                              (struct sockaddr *)&clientAddress,
                              &sizeOfClientInfo);
    if (connectionSocket < 0)
    {
        perror("ERROR SERVER");
        exit(1);
    }

    pid_t pid;
    char inbuffer[MAX];
    pid = fork();
    switch (pid)
    {
    case -1:
      perror("Fork failed\n");
      exit(1);
      break;

    case 0:
      memset(buffer, '\0', sizeof(buffer));
      memset(inbuffer, '\0', sizeof(inbuffer));

      while (!strstr(buffer, "eom\n"))
      {
        charsRead = recv(connectionSocket, inbuffer, sizeof(inbuffer), 0);
        strcat(buffer, inbuffer);
      }

      if (buffer[0] == 'E')
      {
        strcpy(buffer, encryption(buffer));
      }
      else
      {
        strcpy(buffer, "B\neom\n");
      }

      int size = strlen(buffer);
      int charsSent = 0;
      while (charsSent < size)
      {
        charsRead = send(connectionSocket, buffer + charsSent, size - charsSent, 0);
        if (charsRead < 0)
        {
          perror("CLIENT ERROR");
            exit(1);
        }
        charsSent += charsRead;
      }
      close(connectionSocket);
      exit(0);

    default:
      wait(NULL);
      break;
    }
  }
  close(listenSocket);
  return 0;
}
