#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"

#define HOST "34.254.242.81"
#define PORT 8080

#define MAX_CMD 50
#define MAX_CRED 50

char* get_token(char* js) {
    char *token = strstr(js, "{\"");
    strcpy(token, token + 2);
    token[strlen(token) - 2] = '\0';

    return token;
}

int check_spaces(char *string) {
    if (strstr(string, " ") != NULL) {
        return 0;
    }
    return 1;
}

int main(int argc, char *argv[])
{
    char *message;
    char *response;
    int sockfd;
    int logged_in = 0;

    while(1) {
        char command[MAX_CMD];
        fgets(command, MAX_CMD, stdin);

        if (strncmp(command, "exit", 4) == 0) {
            break;
        } else if (strncmp(command, "register", 8) == 0) {
            char username[MAX_CRED], password[MAX_CRED];
            memset(username, 0, MAX_CRED);
            memset(password, 0, MAX_CRED);

            // get data from stdin
            printf("username=");
            fgets(username, MAX_CRED, stdin);
            username[strlen(username) - 1] = '\0';
            //printf("%s ", username);

            printf("password=");
            fgets(password, MAX_CRED, stdin);
            password[strlen(password) - 1] = '\0';
            //sscanf(password, "%s ")
            //printf("%s ", password);

            // check if data is correct
            if (!check_spaces(username) || !check_spaces(password)) {
                printf("ERROR: username and password cannot have any spaces.\n");
                continue;
            }
        
            if (logged_in == 1) {
                printf("ERROR: you are already logged in.\n");
                continue;
            }

            // from README - parson
            JSON_Value *root_value = json_value_init_object();
            JSON_Object *root_object = json_value_get_object(root_value);
            char *json_val = NULL;
            json_object_set_string(root_object, "username", username);
            json_object_set_string(root_object, "password", password);
            json_val = json_serialize_to_string(root_value);

            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            message = compute_post_request(HOST, "/api/v1/tema/auth/register", "application/json", &json_val, 1, NULL, 0, NULL);

            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            //puts(response);

            close_connection(sockfd);
            json_free_serialized_string(json_val);
            json_value_free(root_value);
        }
    }
    
    // Ex 1.1: GET dummy from main server
    // Ex 1.2: POST dummy and print response from main server
    // Ex 2: Login into main server
    // Ex 3: GET weather key from main server
    // Ex 4: GET weather data from OpenWeather API
    // Ex 5: POST weather data for verification to main server
    // Ex 6: Logout from main server

    // BONUS: make the main server return "Already logged in!"

    // free the allocated data at the end!

    return 0;
}
