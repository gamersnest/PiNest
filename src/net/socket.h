/*
    ____  _ _   _           _   
   |  _ \(_) \ | | ___  ___| |_ 
   | |_) | |  \| |/ _ \/ __| __|
   |  __/| | |\  |  __/\__ \ |_ 
   |_|   |_|_| \_|\___||___/\__|

   Copyright Alexander Argentakis & gamersnest.org

   This file is licensed under the GPL v2.0 license

 */

#ifndef SOCKET_H

#define SOCKET_H

typedef struct
{
    char *buffer;
    int buffer_length;
    char *address;
    int port;
} sockin_t;

int create_socket(char *address, int port);

sockin_t receive_data(int sock);

void send_data(int sock, sockin_t in);

#endif