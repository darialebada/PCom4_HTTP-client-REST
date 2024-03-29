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

#define MAX_CMD 100
#define MAX_CRED 50
#define MAX_BOOK 150
#define MAX_ID 15

int check_spaces(char *string) {
    if (strstr(string, " ") != NULL) {
        return 0;
    }
    return 1;
}

void free_conn_json(JSON_Value *root_value, char *json_val, char *message, char *response) {
    json_free_serialized_string(json_val);
    json_value_free(root_value);
    free(message);
    free(response);
}

void free_conn(char *message, char *response) {
    free(message);
    free(response);
}

void print_books(char *response) {
    if (response == NULL) {
        printf("No books available in the library.\n");
        return;
    }
    printf("Books available in library:\n");
    
    // get JSON using parson.c (from README)
    JSON_Value *root_value = json_parse_string(response);
    JSON_Array *arr = json_value_get_array(root_value);
    JSON_Object *obj;

    for (int i = 0; i < json_array_get_count(arr); i++) {
        obj = json_array_get_object(arr, i);
        printf("id: %.0f; title: %s\n", json_object_get_number(obj, "id"), json_object_get_string(obj, "title"));
    }

    json_value_free(root_value);
}

void print_book(char *response) {
    printf("200 - Ok - Book details are:\n");
    JSON_Value *root_value = json_parse_string(response);

    printf("id: %.0f\n", json_object_get_number(json_object(root_value), "id"));
    printf("title: %s\n", json_object_get_string(json_object(root_value), "title"));
    printf("author: %s\n", json_object_get_string(json_object(root_value), "author"));
    printf("publisher: %s\n", json_object_get_string(json_object(root_value), "publisher"));
    printf("genre: %s\n", json_object_get_string(json_object(root_value), "genre"));
    printf("page_count: %.0f\n", json_object_get_number(json_object(root_value), "page_count"));

    json_value_free(root_value);
}

int
check_book(char title[MAX_BOOK], char author[MAX_BOOK], char genre[MAX_BOOK], char publisher[MAX_BOOK],
           char page_count[MAX_BOOK]) {
    // if page_count is not a number
    if (atoi(page_count) == 0) {
        return 0;
    }
    // if the next values are strings
    if (atoi(title) != 0) {
        return 0;
    }
    if (atoi(author) != 0) {
        return 0;
    }
    if (atoi(genre) != 0) {
        return 0;
    }
    if (atoi(publisher) != 0) {
        return 0;
    }

    return 1;
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
        // read command from stdin
        char command[MAX_CMD];
        fgets(command, MAX_CMD, stdin);
        command[strlen(command) - 1] = '\0';

        if (strcmp(command, "exit\0") == 0) {
            // free any remained allocated memory
            if (logged_in == 1) {
                free(cookies);
            }
            if (has_token == 1) {
                free(token);
            }
            break;
        } else if (strcmp(command, "register\0") == 0) {
            // check if user is already connected
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

            // get JSON using parson.c (from README)
            JSON_Value *root_value = json_value_init_object();
            JSON_Object *root_object = json_value_get_object(root_value);
            json_object_set_string(root_object, "username", username);
            json_object_set_string(root_object, "password", password);
            char *json_val = json_serialize_to_string(root_value);

            // open connection with server
            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            // compute POST request for server
            message = compute_post_request(HOST, "/api/v1/tema/auth/register", "application/json", &json_val, 1, NULL, 0, NULL);

            // communicate with server
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            // check errors
            if (strstr(response, "error") != NULL) {
                printf("400 - Bad Request - The username %s is already taken.\n", username);
            } else {
                printf("201 - Created - User registered.\n");
            }

            // close connection/ free memory
            close_connection(sockfd);
            free_conn_json(root_value, json_val, message, response);
        } else if (strcmp(command, "login\0") == 0) {
            // check if user is already connected
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

            // get JSON using parson.c (from README)
            JSON_Value *root_value = json_value_init_object();
            JSON_Object *root_object = json_value_get_object(root_value);
            json_object_set_string(root_object, "username", username);
            json_object_set_string(root_object, "password", password);
            char *json_val = json_serialize_to_string(root_value);

            // open connection with server
            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            // compute POST request for server
            message = compute_post_request(HOST, "/api/v1/tema/auth/login", "application/json", &json_val, 1, NULL, 0, NULL);

            // communicate with server
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            // check errors
            if (strstr(response, "error") != NULL) {
                printf("400 - Bad Request - Credentials are not good.\n");
            } else {
                printf("200 - Ok - User logged in.\n");

                // user connected
                logged_in = 1;

                // get cookies from response
                cookies = calloc(LINELEN, sizeof(char));
                char *begin = strstr(response, "connect.sid=");
                char *end = strstr(response, "Date");
                memcpy(cookies, begin, end - begin);
            }

            // close connection/ free memory
            close_connection(sockfd);
            free_conn_json(root_value, json_val, message, response);
        } else if (strcmp(command, "logout\0") == 0) {
            // check if user is already connected
            if (logged_in == 0) {
                printf("400 - Bad Request - You must login first.\n");
                continue;
            }

            // open connection with server
            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            // compute get request for server
            message = compute_get_request(HOST, "/api/v1/tema/auth/logout", NULL, &cookies, 1, NULL);

            // communicate with server
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            // user not connected
            logged_in = 0;

            // check errors
            if (strstr(response, "error") != NULL) {
                printf("400 - Bad Request - No user connected. You must login first.\n");
            } else {
                printf("200 - Ok - User logged out.\n");
            }

            // free token memory only if the user had a token
            if (has_token == 1) {
                free(token);
                has_token = 0;
            }

            // close connection/ free memory
            close(sockfd);
            free(cookies);
            free_conn(message, response);
        } else if (strcmp(command, "enter_library\0") == 0) {
            // check if user is already connected
            if (logged_in == 0) {
                printf("400 - Bad Request - No user connected. You must login first.\n");
                continue;
            }

            // open connection with server
            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            // compute GET request for server
            message = compute_get_request(HOST, "/api/v1/tema/library/access", NULL, &cookies, 1, NULL);

            // communicate with server
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            // check errors
            if (strstr(response, "error") == NULL) {
                printf("200 - Ok - Access granted to library.\n");

                has_token = 1;
                token = calloc(LINELEN, sizeof(char));
                char *helper = strstr(basic_extract_json_response(response), ":");
                memcpy(token, helper + 2, strlen(helper) - 4);
            }

            // close connection/ free memory
            close(sockfd);
            free(message);
            free(response);
        } else if (strcmp(command, "get_books\0") == 0) {
            // check if user is already connected
            if (logged_in == 0) {
                printf("400 - Bad Request - No user connected. You must login first.\n");
                continue;
            }
            // check if user has access to the library
            if (has_token == 0) {
                printf("403 - Forbidden - No authorization.\n");
                continue;
            }

            // open connection with server
            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            //compute GET request for server
            message = compute_get_request(HOST, "/api/v1/tema/library/books", NULL, &cookies, 1, token);

            // communicate with server
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            // check errors
            if (strstr(response, "error") == NULL) {
                // print books info
                print_books(strstr(response, "[{"));
            } else {
                printf("400 - Bad Request\n");
            }

            // close connection/ free memory
            close(sockfd);
            free_conn(message, response);
        } else if (strcmp(command, "get_book\0") == 0) {
            // check if user is already connected
            if (logged_in == 0) {
                printf("400 - Bad Request - No user connected. You must login first.\n");
                continue;
            }
            // check is user has access to the library
            if (has_token == 0) {
                printf("403 - Forbidden - No authorization.\n");
                continue;
            }

            // read data from stdin
            char id[MAX_ID];
            printf("id=");
            fgets(id, MAX_ID, stdin);
            id[strlen(id) - 1] = '\0';

            // check if data is valid
            if (atoi(id) == 0) {
                printf("400 - Bad Request - ID must be a number.\n");
                continue;
            }
            // get path for request
            char path[] = "/api/v1/tema/library/books/";
            strcat(path, id);

            // open connection with server
            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            // compute GET request for server
            message = compute_get_request(HOST, path, NULL, &cookies, 1, token);

            // communicate with server
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            
            // check errors
            if (strstr(response, "error") != NULL) {
                printf("404 - Not Found - There is no book in library with given ID.\n");
            } else {
                // print book's info
                print_book(strstr(response, "{\"id\""));
            }

            // close connection/ free memory
            close(sockfd);
            free_conn(message, response);
        } else if (strcmp(command, "add_book\0") == 0) {
            // check if user is already connected
            if (logged_in == 0) {
                printf("400 - Bad Request - No user connected. You must login first.\n");
                continue;
            }
            // check is user has access to the library
            if (has_token == 0) {
                printf("403 - Forbidden - No authorization.\n");
                continue;
            }

            char title[MAX_BOOK];
            char author[MAX_BOOK];
            char genre[MAX_BOOK];
            char publisher[MAX_BOOK];
            char page_count[MAX_BOOK];

            // get data from stdin
            printf("title=");
            fgets(title, MAX_BOOK, stdin);
            title[strlen(title) - 1] = '\0';

            printf("author=");
            fgets(author, MAX_BOOK, stdin);
            author[strlen(author) - 1] = '\0';

            printf("genre=");
            fgets(genre, MAX_BOOK, stdin);
            genre[strlen(genre) - 1] = '\0';

            printf("publisher=");
            fgets(publisher, MAX_BOOK, stdin);
            publisher[strlen(publisher) - 1] = '\0';

            printf("page_count=");
            fgets(page_count, MAX_BOOK, stdin);
            page_count[strlen(page_count) - 1] = '\0';

            // check if data is valid
            if (check_book(title, author, genre, publisher, page_count) == 0) {
                printf("400 - Bad Request - Invalid data.\n");
                continue;
            }

            // get JSON using parson.c (from README)
            JSON_Value *root_value = json_value_init_object();
            JSON_Object *root_object = json_value_get_object(root_value);
            json_object_set_string(root_object, "title", title);
            json_object_set_string(root_object, "author", author);
            json_object_set_string(root_object, "genre", genre);
            json_object_set_number(root_object, "page_count", atoi(page_count));
            json_object_set_string(root_object, "publisher", publisher);
            char *json_val = json_serialize_to_string(root_value);

            // open connection with server
            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            // compute POST request for server
            message = compute_post_request(HOST, "/api/v1/tema/library/books", "application/json", &json_val, 1, NULL, 0, token);
            
            // communicate with server
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            // check errors
            if (strstr(response, "error") == NULL) {
                printf("200 - Ok - Book added to library.\n");

            } else {
                printf("400 - Bad Request\n");
            }

            // close connection/ free memory
            close(sockfd);
            free_conn_json(root_value, json_val, message, response);
        } else if (strcmp(command, "delete_book\0") == 0) {
            // check if user is already connected
            if (logged_in == 0) {
                printf("400 - Bad Request - No user connected. You must login first.\n");
                continue;
            }
            // check is user has access to the library
            if (has_token == 0) {
                printf("403 - Forbidden - No authorization.\n");
                continue;
            }

            // read data from stdin
            char id[MAX_ID];
            printf("id=");
            fgets(id, MAX_ID, stdin);
            id[strlen(id) - 1] = '\0';

            // check if data is valid
            if (atoi(id) == 0) {
                printf("400 - Bad Request - ID must be a number.\n");
                continue;
            }
            // get path for request
            char path[] = "/api/v1/tema/library/books/";
            strcat(path, id);

            // open connection with server
            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            // compute DELETE request for server
            message = compute_delete_request(HOST, path, token);

            // communicate with server
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            // check errors
            if (strstr(response, "error") != NULL) {
                printf("404 - Not Found - There is no book in library with given ID.\n");
            } else {
                printf("200 - Ok - Book deleted from library.\n");
            }

            // close connection/ free memory
            close(sockfd);
            free_conn(message, response);
        } else {
            // invalid command
            printf("ERROR - Usage: register - login - enter_library - get_books - get_book - add_book - delete_book - logout - exit\n");
        }
    }

    return 0;
}
