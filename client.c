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
#define MAX_TITLE 100

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

void free_conn(JSON_Value *root_value, char *json_val, char *message, char *response) {
    json_free_serialized_string(json_val);
    json_value_free(root_value);
    free(message);
    free(response);
}

void print_books(char *response) {
    printf("Books available in library:\n");
    int id;
    char title[MAX_TITLE];
    response[strlen(response) - 1] = '\0';
    printf("%s\n", response);

    // char *tok = calloc(LINELEN, sizeof(char));
    // char *tok_begin = strstr(response, "{\"id\":");
    // char *tok_end;

    // while(tok_begin != NULL) {
    //     memset(tok, 0, LINELEN);
    //     memset(title, 0, MAX_TITLE);

    //     tok_end = strstr(response, "},");
    //     memcpy(tok, tok_begin, tok_end - tok_begin - 1);
    //     printf("%s\n", tok);

    //     sscanf(tok, "{\"id\":%d,\"title\":\"%s", &id, title);

    //     //printf("%d %s\n", id, title);
    //     printf("id : %d, title : %s\n", id, title);

    //     strcpy(response, response + (strlen(tok) + 3));

    //     tok_begin = strstr(response, "{\"id\":");
    // }

}

int main(int argc, char *argv[])
{
    char *message;
    char *response;
    char *cookies;
    char *token;
    int sockfd;
    int logged_in = 0;
    int has_token = 0;

    while(1) {
        char command[MAX_CMD];
        fgets(command, MAX_CMD, stdin);
        command[strlen(command) - 1] = '\0';

        if (strncmp(command, "exit", 4) == 0) {
            if (logged_in == 1) {
                free(cookies);
            }
            if (has_token == 1) {
                free(token);
            }
            break;
        } else if (strncmp(command, "register", 8) == 0) {
            if (logged_in == 1) {
                printf("ERROR: already logged in, disconnect first.\n");
                continue;
            }

            char username[MAX_CRED], password[MAX_CRED];

            // get data from stdin
            printf("username=");
            fgets(username, MAX_CRED, stdin);
            username[strlen(username) - 1] = '\0';

            printf("password=");
            fgets(password, MAX_CRED, stdin);
            password[strlen(password) - 1] = '\0';

            // check if data is correct
            if (!check_spaces(username) || !check_spaces(password)) {
                printf("ERROR: username and password cannot have any spaces.\n");
                continue;
            }

            // from README - parson
            JSON_Value *root_value = json_value_init_object();
            JSON_Object *root_object = json_value_get_object(root_value);
            json_object_set_string(root_object, "username", username);
            json_object_set_string(root_object, "password", password);
            char *json_val = json_serialize_to_string(root_value);

            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            // compute post request for server
            message = compute_post_request(HOST, "/api/v1/tema/auth/register", "application/json", &json_val, 1, NULL, 0, NULL);

            // communicate with server
            send_to_server(sockfd, message);
            //puts(message);
            response = receive_from_server(sockfd);
            //puts(response);

            // check error
            if (strstr(response, "error") != NULL) {
                printf("400 - Bad Request - The username %s is already taken.\n", username);
            } else {
                printf("201 - Created - User registered.\n");
            }

            // close connection/ free memory
            close_connection(sockfd);
            free_conn(root_value, json_val, message, response);
        } else if (strncmp(command, "login", 5) == 0) {
            if (logged_in == 1) {
                printf("ERROR: already logged in, disconnect first.\n");
                continue;
            }

            char username[MAX_CRED], password[MAX_CRED];

            // get data from stdin
            printf("username=");
            fgets(username, MAX_CRED, stdin);
            username[strlen(username) - 1] = '\0';

            printf("password=");
            fgets(password, MAX_CRED, stdin);
            password[strlen(password) - 1] = '\0';

            // check if data is correct
            if (!check_spaces(username) || !check_spaces(password)) {
                printf("ERROR: username and password cannot have any spaces.\n");
                continue;
            }

            // from README - parson
            JSON_Value *root_value = json_value_init_object();
            JSON_Object *root_object = json_value_get_object(root_value);
            json_object_set_string(root_object, "username", username);
            json_object_set_string(root_object, "password", password);
            char *json_val = json_serialize_to_string(root_value);

            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            // compute post request for server
            message = compute_post_request(HOST, "/api/v1/tema/auth/login", "application/json", &json_val, 1, NULL, 0, NULL);

            // communicate with server
            send_to_server(sockfd, message);

            response = receive_from_server(sockfd);
            //puts(response);

            // check error
            if (strstr(response, "error") != NULL) {
                printf("400 - Bad Request - Credentials are not good.\n");
            } else {
                printf("200 - Ok - User logged in.\n");

                logged_in = 1;

                // get cookies
                cookies = calloc(LINELEN, sizeof(char));
                char *begin = strstr(response, "connect.sid=");
                char *end = strstr(response, "Date");

                memcpy(cookies, begin, end - begin);
            }

            // close connection/ free memory
            close_connection(sockfd);
            free_conn(root_value, json_val, message, response);
        } else if (strncmp(command, "logout", 6) == 0) {
            if (logged_in == 0) {
                printf("400 - Bad Request - You must login first.\n");
                continue;
            }
            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            message = compute_get_request(HOST, "/api/v1/tema/auth/logout", NULL, &cookies, 1, NULL);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            logged_in = 0;

            // double check error
            if (strstr(response, "error") != NULL) {
                printf("400 - Bad Request - No user connected. You must login first.\n");
            } else {
                printf("200 - Ok - User logged out.\n");
            }

            if (has_token == 1) {
                free(token);
                has_token = 0;
            }

            // free memory
            close(sockfd);
            free(message);
            free(response);
            free(cookies);
        } else if (strncmp(command, "enter_library", 13) == 0) {
            if (logged_in == 0) {
                printf("400 - Bad Request - No user connected. You must login first.\n");
                continue;
            }

            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            message = compute_get_request(HOST, "/api/v1/tema/library/access", NULL, &cookies, 1, NULL);
            send_to_server(sockfd, message);
            //puts(message);
            response = receive_from_server(sockfd);
            //puts(response);

            if (strstr(response, "error") == NULL) {
                printf("200 - Ok - Access granted to library.\n");

                has_token = 1;
                token = calloc(LINELEN, sizeof(char));
                char *helper = strstr(basic_extract_json_response(response), ":");
                memcpy(token, helper + 2, strlen(helper) - 4);
            }

            close(sockfd);
            free(message);
            free(response);
        } else if (strncmp(command, "get_books", 9) == 0) {
            if (logged_in == 0) {
                printf("400 - Bad Request - No user connected. You must login first.\n");
                continue;
            }
            
            if (has_token == 0) {
                printf("403 - Forbidden - No authorization.\n");
                continue;
            }

            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            message = compute_get_request(HOST, "/api/v1/tema/library/books", NULL, &cookies, 1, token);
            send_to_server(sockfd, message);
            //puts(message);
            response = receive_from_server(sockfd);
            //puts(response);

            print_books(strstr(response, "[{"));

            close(sockfd);
            free(message);
            free(response);
        }
    }

    return 0;
}
