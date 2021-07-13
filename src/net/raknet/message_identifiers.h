/*
    ____  _ _   _           _   
   |  _ \(_) \ | | ___  ___| |_ 
   | |_) | |  \| |/ _ \/ __| __|
   |  __/| | |\  |  __/\__ \ |_ 
   |_|   |_|_| \_|\___||___/\__|

   Copyright Alexander Argentakis & gamersnest.org

   This file is licensed under the GPL v2.0 license

 */

#ifndef MESSAGE_IDENTIFIERS

#define MESSAGE_IDENTIFIERS

#define ID_CONNECTED_PING 0x00
#define ID_UNCONNECTED_PING 0x01
#define ID_UNCONNECTED_PING_OPEN_CONNECTIONS 0x02
#define ID_CONNECTED_PONG 0x03
#define ID_OPEN_CONNECTION_REQUEST_1 0x05
#define ID_OPEN_CONNECTION_REPLY_1 0x06
#define ID_OPEN_CONNECTION_REQUEST_2 0x07
#define ID_OPEN_CONNECTION_REPLY_2 0x08
#define ID_CONNECTION_REQUEST 0x09
#define ID_CONNECTION_REQUEST_ACCEPTED 0x10
#define ID_NEW_INCOMING_CONNECTION 0x13
#define ID_DISCONNECTION_NOTIFICATION 0x15
#define ID_INCOMPATIBLE_PROTOCOL_VERSION 0x19
#define ID_UNCONNECTED_PONG 0x1c

#endif