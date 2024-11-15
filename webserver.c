#include <stdio.h>
#include <wiringPi.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#define BUFFER_SIZE 1024
#define PORT_DEFAULT 8080

const char* valid_names[] = {"arm","schouder","elleboog","pols","hand","vinger"};
const int valid_names_count = 6;
int is_valid_name(const char* name) {
  for (int i = 0; i < valid_names_count; ++i) {
    if (strcmp(name, valid_names[i]) == 0) {
      return 1; 
    }
  }
  return 0; 
}
void trim_newline(char *str) {
  char *pos;
  if ((pos = strchr(str, '\n')) != NULL) {
    *pos = '\0';
  }
  if ((pos = strchr(str, '\r')) != NULL) {
    *pos = '\0';
  }
}
int main(int argc, char *argv[]) {
  wiringPiSetupGpio();
pinMode(14, OUTPUT);
  int opt;
  int port = PORT_DEFAULT; 
  while ((opt = getopt(argc, argv, "p:")) != -1) {
    switch (opt) {
      case 'p':
        port = atoi(optarg); 
        break;
      default:
        fprintf(stderr, "Usage: %s [-p port]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
  }
  int server_fd, new_socket;
  struct sockaddr_in address;
  int addrlen = sizeof(address);
  char buffer[BUFFER_SIZE] = {0};
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);
  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }
  if (listen(server_fd, 3) < 0) {
    perror("listen failed");
    exit(EXIT_FAILURE);
  }
  printf("Listening on port %d...\n", port);
  if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
    perror("accept failed");
    exit(EXIT_FAILURE);
  }
  while (1) {
    int valread = read(new_socket, buffer, BUFFER_SIZE);
    if (valread > 0) {
      buffer[valread] = '\0'; 
      char *command = strtok(buffer, " ");
      char *name = strtok(NULL, " ");
      char *direction = strtok(NULL, " ");
      if (direction) {
        trim_newline(direction);
      }
      if (command && name && direction) {
        if (strcmp(command, "MOVE") == 0) {
          if (is_valid_name(name)) {
            if (strcmp(direction, "up") == 0 || strcmp(direction, "down")|| strcmp(direction, "right") == 0 || strcmp(direction, "left")  == 0) {
                //pins are 14 15 18 23 24 25
                digitalWrite(14, 255);
            }
          } else {
            printf("Invalid name: %s\n", name);
          }
        } else {
          printf("Invalid command: %s (expected 'MOVE')\n", command);
        }
      } else {
        printf("Invalid input format. Expected: MOVE <name> <up/down>\n");
      }
    }
  }
  close(new_socket);
  close(server_fd);
  return 0;
}
