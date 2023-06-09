#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <netinet/in.h>
#include <netdb.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <stdbool.h>


#define NET_SERVER_BUFFER_SIZE 2048
#define NET_SERVER_PORT        8838

int main(void) {
    char* buffer = (char *)malloc(NET_SERVER_BUFFER_SIZE);

    int socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if(socket < 0) {
        fprintf(stderr, "Error: unable to create socket!\n");
        free(buffer);

        return EXIT_FAILURE;
    }

    struct hostent* server = gethostbyname("127.0.0.1");
    if(server == NULL) {
        fprintf(stderr, "Error: cannot get host by its name\n");
        free(buffer);

        return EXIT_FAILURE;
    }

    struct sockaddr_in server_address;
    bzero((char *)&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&server_address.sin_addr.s_addr, server->h_length);
    server_address.sin_port   = htons(NET_SERVER_PORT);

    if(connect(socket_desc, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        fprintf(stderr, "Error: failed to connect to the server\n");
			  fprintf(stderr, "It could be off, do you want it to start?[y/N]");

				char input[256];
				scanf("%s", input);

				if(strcmp(input, "y") == 0) {
					pid_t pid = fork();
				
					if(pid == 0) {
						fprintf(stderr, "Attempting to run the server\n");
						execv("/bin/wt-daemon", NULL);
					}
					else {
						 waitpid(pid, 0, 0);
			       free(buffer);
 			       return EXIT_FAILURE;
					}
				} else {
			       free(buffer);
 			       return EXIT_FAILURE;
				}

   }


    while(true) {
        // Recieve from server
        bzero(buffer, NET_SERVER_BUFFER_SIZE);

        ssize_t bytes_num = read(socket_desc, buffer, NET_SERVER_BUFFER_SIZE);
        if(bytes_num < 0) {
            fprintf(stderr, "Error: failed to read from the server");
            free(buffer);

            return EXIT_FAILURE;
        }

        if(strcmp(buffer, "__recv_start") == 0) {
            do {
                ssize_t bytes_num = read(socket_desc, buffer, NET_SERVER_BUFFER_SIZE);
                if(bytes_num < 0) {
                    fprintf(stderr, "Error: failed to read from the server");
                    free(buffer);

                    return EXIT_FAILURE;
                }

                fprintf(stderr, "%s\n", buffer);
            } while(strcmp(buffer, "__recv_end") != 0);
        } else {
            fprintf(stderr, "%s\n", buffer);
        }

        // Send to server
        bzero(buffer, NET_SERVER_BUFFER_SIZE);
		    fprintf(stderr, "(daemon-ctl) $ ");
        fgets(buffer, NET_SERVER_BUFFER_SIZE, stdin);

        bytes_num = write(socket_desc, buffer, strlen(buffer));
        if(bytes_num < 0) {
            fprintf(stderr, "Error: failed to write to the server");
            free(buffer);

            return EXIT_FAILURE;
        }
    }


    free(buffer);

    return EXIT_SUCCESS;
}
