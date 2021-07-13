/*
    ____  _ _   _           _   
   |  _ \(_) \ | | ___  ___| |_ 
   | |_) | |  \| |/ _ \/ __| __|
   |  __/| | |\  |  __/\__ \ |_ 
   |_|   |_|_| \_|\___||___/\__|

   Copyright Alexander Argentakis & gamersnest.org

   This file is licensed under the GPL v2.0 license

 */

#ifdef _WIN32

#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")

#endif
#ifdef linux

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#endif
#include "../net/socket.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>


int create_socket(char *address, int port)
{
    #ifdef _WIN32

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
	{
        printf("Failed. Error Code : %d", WSAGetLastError());
        exit(EXIT_FAILURE);
	}

    #endif
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    struct sockaddr_in s_address;
    memset(&s_address, 0, sizeof(s_address));
    s_address.sin_family = AF_INET;
    s_address.sin_addr.s_addr = inet_addr(address);
    s_address.sin_port = htons(port);
    if (bind(sock, (struct sockaddr *) &s_address, sizeof(s_address)) < 0)
    {
        perror("Failed to bind port!");
        exit(EXIT_FAILURE);
    }
    return sock;
}

sockin_t receive_data(int sock)
{
    struct sockaddr_in s_address;
    memset(&s_address, 0, sizeof(s_address));
    char buffer[65535];
    unsigned int s_address_length = sizeof(s_address);
    int length = recvfrom(
        sock,
        (char *) buffer,
        65535,
        0,
        (struct sockaddr *) &s_address,
        &s_address_length
    );
    sockin_t result;
    result.buffer = (char *) buffer;
    result.buffer_length = length;
    result.address = inet_ntoa(s_address.sin_addr);
    result.port = htons(s_address.sin_port);
    return result;
}

void send_data(int sock, sockin_t in)
{
    struct sockaddr_in s_address;
    memset(&s_address, 0, sizeof(s_address));
    s_address.sin_family = AF_INET;
    s_address.sin_addr.s_addr = inet_addr(in.address);
    s_address.sin_port = htons(in.port);
    sendto(
        sock,
        (char *) in.buffer,
        in.buffer_length,
        0,
        (struct sockaddr *) &s_address,
        sizeof(s_address)
    );
}