/*
    ____  _ _   _           _   
   |  _ \(_) \ | | ___  ___| |_ 
   | |_) | |  \| |/ _ \/ __| __|
   |  __/| | |\  |  __/\__ \ |_ 
   |_|   |_|_| \_|\___||___/\__|

   Copyright Alexander Argentakis & gamersnest.org

   This file is licensed under the GPL v2.0 license

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../net/raknet/packets.h"
#include "../../net/raknet/message_identifiers.h"
#include <arpa/inet.h>

char magic[16] = {
    0x00, 0xff, 0xff, 0x00,
    0xfe, 0xfe, 0xfe, 0xfe,
    0xfd, 0xfd, 0xfd, 0xfd,
    0x12, 0x34, 0x56, 0x78
};

unconnected_ping_t decode_unconnected_ping(packet_t packet)
{
    unconnected_ping_t result;
    result.client_timestamp = (((unsigned long long) (packet.buffer[1] & 0xff)) << ((unsigned long long) 56));
    result.client_timestamp |= (((unsigned long long) (packet.buffer[2] & 0xff)) << ((unsigned long long) 48));
    result.client_timestamp |= (((unsigned long long) (packet.buffer[3] & 0xff)) << ((unsigned long long) 40));
    result.client_timestamp |= (((unsigned long long) (packet.buffer[4] & 0xff)) << ((unsigned long long) 32));
    result.client_timestamp |= (((unsigned long long) (packet.buffer[5] & 0xff)) << ((unsigned long long) 24));
    result.client_timestamp |= (((unsigned long long) (packet.buffer[6] & 0xff)) << ((unsigned long long) 16));
    result.client_timestamp |= (((unsigned long long) (packet.buffer[7] & 0xff)) << ((unsigned long long) 8));
    result.client_timestamp |= ((unsigned long long) (packet.buffer[8] & 0xff));
    return result;
}

packet_t encode_unconnected_ping(unconnected_ping_t packet)
{
    char *buffer = malloc(25);
    buffer[0] = ID_UNCONNECTED_PING;
    buffer[1] = (packet.client_timestamp >> 56) & 0xff;
    buffer[2] = (packet.client_timestamp >> 48) & 0xff;
    buffer[3] = (packet.client_timestamp >> 40) & 0xff;
    buffer[4] = (packet.client_timestamp >> 32) & 0xff;
    buffer[5] = (packet.client_timestamp >> 24) & 0xff;
    buffer[6] = (packet.client_timestamp >> 16) & 0xff;
    buffer[7] = (packet.client_timestamp >> 8) & 0xff;
    buffer[8] = packet.client_timestamp & 0xff;
    int i;
    for (i = 0; i < 16; ++i)
    {
        buffer[i + 9] = magic[i];
    }
    packet_t result;
    result.buffer = buffer;
    result.length = 25;
    return result;
}

unconnected_pong_t decode_unconnected_pong(packet_t packet)
{
    unconnected_pong_t result;
    result.client_timestamp = (((unsigned long long) (packet.buffer[1] & 0xff)) << ((unsigned long long) 56));
    result.client_timestamp |= (((unsigned long long) (packet.buffer[2] & 0xff)) << ((unsigned long long) 48));
    result.client_timestamp |= (((unsigned long long) (packet.buffer[3] & 0xff)) << ((unsigned long long) 40));
    result.client_timestamp |= (((unsigned long long) (packet.buffer[4] & 0xff)) << ((unsigned long long) 32));
    result.client_timestamp |= (((unsigned long long) (packet.buffer[5] & 0xff)) << ((unsigned long long) 24));
    result.client_timestamp |= (((unsigned long long) (packet.buffer[6] & 0xff)) << ((unsigned long long) 16));
    result.client_timestamp |= (((unsigned long long) (packet.buffer[7] & 0xff)) << ((unsigned long long) 8));
    result.client_timestamp |= ((unsigned long long) (packet.buffer[8] & 0xff));
    result.server_guid = (((unsigned long long) (packet.buffer[9] & 0xff)) << ((unsigned long long) 56));
    result.server_guid |= (((unsigned long long) (packet.buffer[10] & 0xff)) << ((unsigned long long) 48));
    result.server_guid |= (((unsigned long long) (packet.buffer[11] & 0xff)) << ((unsigned long long) 40));
    result.server_guid |= (((unsigned long long) (packet.buffer[12] & 0xff)) << ((unsigned long long) 32));
    result.server_guid |= (((unsigned long long) (packet.buffer[13] & 0xff)) << ((unsigned long long) 24));
    result.server_guid |= (((unsigned long long) (packet.buffer[14] & 0xff)) << ((unsigned long long) 16));
    result.server_guid |= (((unsigned long long) (packet.buffer[15] & 0xff)) << ((unsigned long long) 8));
    result.server_guid |= ((unsigned long long) (packet.buffer[16] & 0xff));
    unsigned short server_name_length = (packet.buffer[33] & 0xff) << 8;
    server_name_length |= (packet.buffer[34] & 0xff);
    result.server_name = malloc(server_name_length + 1);
    int i;
    for (i = 0; i < server_name_length; ++i)
    {
        result.server_name[i] = packet.buffer[i + 35];
    }
    result.server_name[server_name_length] = 0x00;
    return result;
}

packet_t encode_unconnected_pong(unconnected_pong_t packet)
{
    unsigned short server_name_length = strlen(packet.server_name);
    char *buffer = malloc(35 + server_name_length);
    buffer[0] = ID_UNCONNECTED_PONG;
    buffer[1] = (packet.client_timestamp >> 56) & 0xff;
    buffer[2] = (packet.client_timestamp >> 48) & 0xff;
    buffer[3] = (packet.client_timestamp >> 40) & 0xff;
    buffer[4] = (packet.client_timestamp >> 32) & 0xff;
    buffer[5] = (packet.client_timestamp >> 24) & 0xff;
    buffer[6] = (packet.client_timestamp >> 16) & 0xff;
    buffer[7] = (packet.client_timestamp >> 8) & 0xff;
    buffer[8] = packet.client_timestamp & 0xff;
    buffer[9] = (packet.server_guid >> 56) & 0xff;
    buffer[10] = (packet.server_guid >> 48) & 0xff;
    buffer[11] = (packet.server_guid >> 40) & 0xff;
    buffer[12] = (packet.server_guid >> 32) & 0xff;
    buffer[13] = (packet.server_guid >> 24) & 0xff;
    buffer[14] = (packet.server_guid >> 16) & 0xff;
    buffer[15] = (packet.server_guid >> 8) & 0xff;
    buffer[16] = packet.server_guid & 0xff;
    int i;
    for (i = 0; i < 16; ++i)
    {
        buffer[i + 17] = magic[i];
    }
    buffer[33] = (server_name_length >> 8) & 0xff;
    buffer[34] = server_name_length & 0xff;
    
    for (i = 0; i < server_name_length; ++i)
    {
        buffer[i + 35] = packet.server_name[i];
    }
    packet_t result;
    result.buffer = buffer;
    result.length = 35 + server_name_length;
    return result;
}

open_connection_request_1_t decode_open_connection_request_1(packet_t packet)
{
    open_connection_request_1_t result;
    result.protocol_version = packet.buffer[17] & 0xff;
    result.mtu_size = packet.length - 18;
    return result;
}

packet_t open_connection_request_1(open_connection_request_1_t packet)
{
    char *buffer = malloc(18 + packet.mtu_size);
    buffer[0] = ID_OPEN_CONNECTION_REQUEST_1;
    int i;
    for (i = 0; i < 16; ++i)
    {
        buffer[i + 1] = magic[i];
    }
    buffer[17] = packet.protocol_version;
    for (i = 0; i < packet.mtu_size; ++i)
    {
        buffer[i + 18] = 0x00;
    }
    packet_t result;
    result.buffer = buffer;
    result.length = 18 + packet.mtu_size;
    return result;
}

open_connection_reply_1_t decode_open_connection_reply_1(packet_t packet)
{
    open_connection_reply_1_t result;
    result.server_guid = (((unsigned long long) (packet.buffer[17] & 0xff)) << ((unsigned long long) 56));
    result.server_guid |= (((unsigned long long) (packet.buffer[18] & 0xff)) << ((unsigned long long) 48));
    result.server_guid |= (((unsigned long long) (packet.buffer[19] & 0xff)) << ((unsigned long long) 40));
    result.server_guid |= (((unsigned long long) (packet.buffer[20] & 0xff)) << ((unsigned long long) 32));
    result.server_guid |= (((unsigned long long) (packet.buffer[21] & 0xff)) << ((unsigned long long) 24));
    result.server_guid |= (((unsigned long long) (packet.buffer[22] & 0xff)) << ((unsigned long long) 16));
    result.server_guid |= (((unsigned long long) (packet.buffer[23] & 0xff)) << ((unsigned long long) 8));
    result.server_guid |= ((unsigned long long) (packet.buffer[24] & 0xff));
    result.use_security = packet.buffer[25] & 0xff;
    result.mtu_size = (packet.buffer[26] & 0xff) << 8;
    result.mtu_size |= (packet.buffer[27] & 0xff);
    return result;
}

packet_t encode_open_connection_reply_1(open_connection_reply_1_t packet)
{
    char *buffer = malloc(28);
    buffer[0] = ID_OPEN_CONNECTION_REPLY_1;
    int i;
    for (i = 0; i < 16; ++i)
    {
        buffer[i + 1] = magic[i];
    }
    buffer[17] = (packet.server_guid >> 56) & 0xff;
    buffer[18] = (packet.server_guid >> 48) & 0xff;
    buffer[19] = (packet.server_guid >> 40) & 0xff;
    buffer[20] = (packet.server_guid >> 32) & 0xff;
    buffer[21] = (packet.server_guid >> 24) & 0xff;
    buffer[22] = (packet.server_guid >> 16) & 0xff;
    buffer[23] = (packet.server_guid >> 8) & 0xff;
    buffer[24] = packet.server_guid & 0xff;
    buffer[25] = packet.use_security;
    buffer[26] = (packet.mtu_size >> 8) & 0xff;
    buffer[27] = packet.mtu_size & 0xff;
    packet_t result;
    result.buffer = buffer;
    result.length = 28;
    return result;
}

open_connection_request_2_t decode_open_connection_request_2(packet_t packet)
{
    open_connection_request_2_t result;
    result.server_address = malloc(8);
    sprintf(
        result.server_address,
        "%d.%d.%d.%d",
        ~packet.buffer[18] & 0xff,
        ~packet.buffer[19] & 0xff,
        ~packet.buffer[20] & 0xff,
        ~packet.buffer[21] & 0xff
    );
    result.server_port = (packet.buffer[22] & 0xff) << 8;
    result.server_port |= (packet.buffer[23] & 0xff);
    result.mtu_size = (packet.buffer[24] & 0xff) << 8;
    result.mtu_size |= (packet.buffer[25] & 0xff);
    result.client_guid = (((unsigned long long) (packet.buffer[26] & 0xff)) << ((unsigned long long) 56));
    result.client_guid |= (((unsigned long long) (packet.buffer[27] & 0xff)) << ((unsigned long long) 48));
    result.client_guid |= (((unsigned long long) (packet.buffer[28] & 0xff)) << ((unsigned long long) 40));
    result.client_guid |= (((unsigned long long) (packet.buffer[29] & 0xff)) << ((unsigned long long) 32));
    result.client_guid |= (((unsigned long long) (packet.buffer[30] & 0xff)) << ((unsigned long long) 24));
    result.client_guid |= (((unsigned long long) (packet.buffer[31] & 0xff)) << ((unsigned long long) 16));
    result.client_guid |= (((unsigned long long) (packet.buffer[32] & 0xff)) << ((unsigned long long) 8));
    result.client_guid |= ((unsigned long long) (packet.buffer[33] & 0xff));
    return result;
}

packet_t encode_open_connection_request_2(open_connection_request_2_t packet)
{
    char *buffer = malloc(34);
    buffer[0] = ID_OPEN_CONNECTION_REQUEST_2;
    int i;
    for (i = 0; i < 16; ++i)
    {
        buffer[i + 1] = magic[i];
    }
    buffer[17] = 4;
    in_addr_t address = inet_addr(packet.server_address);
    buffer[18] = ~(address & 0xff);
    buffer[19] = ~((address >> 8) & 0xff) & 0xff;
    buffer[20] = ~((address >> 16) & 0xff) & 0xff;
    buffer[21] = ~((address >> 24) & 0xff) & 0xff;
    buffer[22] = (packet.server_port >> 8) & 0xff;
    buffer[23] = packet.server_port & 0xff;
    buffer[24] = (packet.mtu_size >> 8) & 0xff;
    buffer[25] = packet.mtu_size & 0xff;
    buffer[26] = (packet.client_guid >> 56) & 0xff;
    buffer[27] = (packet.client_guid >> 48) & 0xff;
    buffer[28] = (packet.client_guid >> 40) & 0xff;
    buffer[29] = (packet.client_guid >> 32) & 0xff;
    buffer[30] = (packet.client_guid >> 24) & 0xff;
    buffer[31] = (packet.client_guid >> 16) & 0xff;
    buffer[32] = (packet.client_guid >> 8) & 0xff;
    buffer[33] = packet.client_guid & 0xff;
    packet_t result;
    result.buffer = buffer;
    result.length = 34;
    return result;
}

open_connection_reply_2_t decode_open_connection_reply_2(packet_t packet)
{
    open_connection_reply_2_t result;
    result.server_guid = (((unsigned long long) (packet.buffer[17] & 0xff)) << ((unsigned long long) 56));
    result.server_guid |= (((unsigned long long) (packet.buffer[18] & 0xff)) << ((unsigned long long) 48));
    result.server_guid |= (((unsigned long long) (packet.buffer[19] & 0xff)) << ((unsigned long long) 40));
    result.server_guid |= (((unsigned long long) (packet.buffer[20] & 0xff)) << ((unsigned long long) 32));
    result.server_guid |= (((unsigned long long) (packet.buffer[21] & 0xff)) << ((unsigned long long) 24));
    result.server_guid |= (((unsigned long long) (packet.buffer[22] & 0xff)) << ((unsigned long long) 16));
    result.server_guid |= (((unsigned long long) (packet.buffer[23] & 0xff)) << ((unsigned long long) 8));
    result.server_guid |= ((unsigned long long) (packet.buffer[24] & 0xff));
    result.client_address = malloc(8);
    sprintf(
        result.client_address,
        "%d.%d.%d.%d",
        ~packet.buffer[26] & 0xff,
        ~packet.buffer[27] & 0xff,
        ~packet.buffer[28] & 0xff,
        ~packet.buffer[29] & 0xff
    );
    result.client_port = (packet.buffer[30] & 0xff) << 8;
    result.client_port |= (packet.buffer[31] & 0xff);
    result.mtu_size = (packet.buffer[32] & 0xff) << 8;
    result.mtu_size |= (packet.buffer[33] & 0xff);
    result.use_encryption = packet.buffer[34] & 0xff;
    return result;
}

packet_t encode_open_connection_reply_2(open_connection_reply_2_t packet)
{
    char *buffer = malloc(35);
    buffer[0] = ID_OPEN_CONNECTION_REPLY_2;
    int i;
    for (i = 0; i < 16; ++i)
    {
        buffer[i + 1] = magic[i];
    }
    buffer[17] = (packet.server_guid >> 56) & 0xff;
    buffer[18] = (packet.server_guid >> 48) & 0xff;
    buffer[19] = (packet.server_guid >> 40) & 0xff;
    buffer[20] = (packet.server_guid >> 32) & 0xff;
    buffer[21] = (packet.server_guid >> 24) & 0xff;
    buffer[22] = (packet.server_guid >> 16) & 0xff;
    buffer[23] = (packet.server_guid >> 8) & 0xff;
    buffer[24] = packet.server_guid & 0xff;
    buffer[25] = 4;
    in_addr_t address = inet_addr(packet.client_address);
    buffer[26] = ~(address & 0xff);
    buffer[27] = ~((address >> 8) & 0xff) & 0xff;
    buffer[28] = ~((address >> 16) & 0xff) & 0xff;
    buffer[29] = ~((address >> 24) & 0xff) & 0xff;
    buffer[30] = (packet.client_port >> 8) & 0xff;
    buffer[31] = packet.client_port & 0xff;
    buffer[32] = (packet.mtu_size >> 8) & 0xff;
    buffer[33] = packet.mtu_size & 0xff;
    buffer[34] = packet.use_encryption;
    packet_t result;
    result.buffer = buffer;
    result.length = 35;
    return result;
}

connection_request_t decode_connection_request(packet_t packet)
{
    connection_request_t result;
    result.client_guid = (((unsigned long long) (packet.buffer[1] & 0xff)) << ((unsigned long long) 56));
    result.client_guid |= (((unsigned long long) (packet.buffer[2] & 0xff)) << ((unsigned long long) 48));
    result.client_guid |= (((unsigned long long) (packet.buffer[3] & 0xff)) << ((unsigned long long) 40));
    result.client_guid |= (((unsigned long long) (packet.buffer[4] & 0xff)) << ((unsigned long long) 32));
    result.client_guid |= (((unsigned long long) (packet.buffer[5] & 0xff)) << ((unsigned long long) 24));
    result.client_guid |= (((unsigned long long) (packet.buffer[6] & 0xff)) << ((unsigned long long) 16));
    result.client_guid |= (((unsigned long long) (packet.buffer[7] & 0xff)) << ((unsigned long long) 8));
    result.client_guid |= ((unsigned long long) (packet.buffer[8] & 0xff));
    result.request_timestamp = (((unsigned long long) (packet.buffer[9] & 0xff)) << ((unsigned long long) 56));
    result.request_timestamp |= (((unsigned long long) (packet.buffer[10] & 0xff)) << ((unsigned long long) 48));
    result.request_timestamp |= (((unsigned long long) (packet.buffer[11] & 0xff)) << ((unsigned long long) 40));
    result.request_timestamp |= (((unsigned long long) (packet.buffer[12] & 0xff)) << ((unsigned long long) 32));
    result.request_timestamp |= (((unsigned long long) (packet.buffer[13] & 0xff)) << ((unsigned long long) 24));
    result.request_timestamp |= (((unsigned long long) (packet.buffer[14] & 0xff)) << ((unsigned long long) 16));
    result.request_timestamp |= (((unsigned long long) (packet.buffer[15] & 0xff)) << ((unsigned long long) 8));
    result.request_timestamp |= ((unsigned long long) (packet.buffer[16] & 0xff));
    return result;
}

packet_t encode_connection_request(connection_request_t packet)
{
    char *buffer = malloc(17);
    buffer[0] = ID_CONNECTION_REQUEST;
    buffer[1] = (packet.client_guid >> 56) & 0xff;
    buffer[2] = (packet.client_guid >> 48) & 0xff;
    buffer[3] = (packet.client_guid >> 40) & 0xff;
    buffer[4] = (packet.client_guid >> 32) & 0xff;
    buffer[5] = (packet.client_guid >> 24) & 0xff;
    buffer[6] = (packet.client_guid >> 16) & 0xff;
    buffer[7] = (packet.client_guid >> 8) & 0xff;
    buffer[8] = packet.client_guid & 0xff;
    buffer[9] = (packet.request_timestamp >> 56) & 0xff;
    buffer[10] = (packet.request_timestamp >> 48) & 0xff;
    buffer[11] = (packet.request_timestamp >> 40) & 0xff;
    buffer[12] = (packet.request_timestamp >> 32) & 0xff;
    buffer[13] = (packet.request_timestamp >> 24) & 0xff;
    buffer[14] = (packet.request_timestamp >> 16) & 0xff;
    buffer[15] = (packet.request_timestamp >> 8) & 0xff;
    buffer[16] = packet.request_timestamp & 0xff;
    packet_t result;
    result.buffer = buffer;
    result.length = 17;
    return result;
}