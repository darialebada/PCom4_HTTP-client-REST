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

#define HOST "34.254.242.81"
#define PORT 8080

char* get_token(char* js) {
    char *token = strstr(js, "{\"");
    strcpy(token, token + 2);
    token[strlen(token) - 2] = '\0';

    return token;
}

int main(int argc, char *argv[])
{
    char *message;
    char *response;
    int sockfd;
        
    sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
    
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
