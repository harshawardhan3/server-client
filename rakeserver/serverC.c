#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h> 
#define PORT 9999

FILE *popen(const char *command, const char *type);
int pclose(FILE *stream);

int code_executor(char *code_line, int socknum){

  FILE *fp;
  char path[1024];

  /* Open the command for reading. */
  fp = popen(code_line, "r");
  if (fp == NULL) {
    printf("Failed to run command\n" );
    exit(1);
  }

  /* Read the output a line at a time - output it. */
  while (fgets(path, sizeof(path), fp) != NULL) {
    printf("%s", path);
    send(socknum, path, 1024, 0);                                   //send the result back to client
  }

  /* close */
  int exit_code = pclose(fp);    //if exit_code!=0 , the action is not executed successfully

  return exit_code;

}

int main(int argc, char const* argv[])
{
    int z=0;
    while(1){
            char buffer[1024];
            int server_fd, new_socket;
            struct sockaddr_in address;
            int opt = 1;
            int addrlen = sizeof(address);
    
        // Creating socket file descriptor
            if ((server_fd = socket(AF_INET, SOCK_STREAM, 0))
            == 0) {
                perror("socket failed");
                exit(EXIT_FAILURE);
            }
            else
            printf("Socket successsfully Created ..\n");
            // Forcefully attaching socket to the port 8080
            if (setsockopt(server_fd, SOL_SOCKET,
                        SO_REUSEADDR | SO_REUSEPORT, &opt,
                        sizeof(opt))) {
                perror("setsockopt");
                exit(EXIT_FAILURE);
            }
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = INADDR_ANY;
            address.sin_port = htons(PORT);
    
            // Forcefully attaching socket to the port 8080
            if (bind(server_fd, (struct sockaddr*)&address,
                    sizeof(address))
                < 0) {
                perror("bind failed");
                exit(EXIT_FAILURE);
            }
            else
            printf("Socket successsfully binded ..\n");
            if (listen(server_fd, 3) < 0) {
                perror("listen");
                exit(EXIT_FAILURE);
            }
            else
            printf("Server is listening ..\n");
            if ((new_socket
                = accept(server_fd, (struct sockaddr*)&address,
                        (socklen_t*)&addrlen))
                < 0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }
            else{
                printf("Server accept connection ..\n");
                recv(new_socket, buffer, 1024, 0);
                if(strcmp(buffer,"rnum")==0){
                    char rnum[1024];                                     //if client is asking for random num
                    srand(getpid()+z);
                    sprintf(rnum,"%d",rand()%1000);
                    send(new_socket, rnum, 1024, 0);
                }
                
                else if(strcmp(buffer, "terminate")==0){
                send(new_socket, "server terminated", 1024, 0);
                break;}
                
                else{                                                              // if it is sending command (without require files)
                    int exit_status = code_executor(buffer, new_socket);
                        if (exit_status != 0){
                            printf("execution interrupted\n");
                            break;
                            send(new_socket, "failed!", 1024, 0);
                        }
                        send(new_socket, "end", 1024, 0);                   //tell the client to stop recieving
                        close(new_socket);
                }
            }
            ++z;
            close(server_fd);
    }
        return 0;
}
