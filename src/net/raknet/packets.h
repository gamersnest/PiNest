/*
    ____  _ _   _           _   
   |  _ \(_) \ | | ___  ___| |_ 
   | |_) | |  \| |/ _ \/ __| __|
   |  __/| | |\  |  __/\__ \ |_ 
   |_|   |_|_| \_|\___||___/\__|

   Copyright Alexander Argentakis & gamersnest.org

   This file is licensed under the GPL v2.0 license

 */

#ifndef PACKETS_H

#define PACKETS_H

#include "../../binary_stream/binary_stream.h"

typedef struct
{
    char *hostname;
    unsigned short port;
    unsigned char version;
} address_t;

typedef struct
{
    unsigned long long client_timestamp;
} unconnected_ping_t;

typedef struct
{
    unsigned long long client_timestamp;
    unsigned long long server_guid;
    char *server_name;
} unconnected_pong_t;

typedef struct
{
    unsigned char protocol_version;
    unsigned short mtu_size;
} open_connection_request_1_t;

typedef struct
{
    unsigned long long server_guid;
    unsigned char use_security;
    unsigned short mtu_size;
} open_connection_reply_1_t;

typedef struct
{
    address_t server_address;
    unsigned short mtu_size;
    unsigned long long client_guid;
} open_connection_request_2_t;

typedef struct
{
    unsigned long long server_guid;
    address_t client_address;
    unsigned short mtu_size;
    unsigned char use_encryption;
} open_connection_reply_2_t;

typedef struct
{
    unsigned long long client_guid;
    unsigned long long request_timestamp;
} connection_request_t;

typedef struct
{
	unsigned char reliability;
    unsigned char is_fragmented;
    unsigned short body_length;
    unsigned int reliable_frame_index;
    unsigned int sequenced_frame_index;
    unsigned int ordered_frame_index;
    unsigned char order_channel;
    unsigned int compound_size;
    unsigned short compound_id;
    unsigned int index;
    char *body;
} frame_t;

typedef struct
{
    unsigned int sequence_number;
    frame_t *frames;
    unsigned int frame_count;
} frame_set_t;

typedef struct {
    unsigned int *sequence_numbers;
    unsigned int sequence_numbers_count;
} acknowledgement_t;

char *get_string(binary_stream_t *stream);
void put_string(char *value, binary_stream_t *stream);
unconnected_ping_t decode_unconnected_ping(binary_stream_t *stream);
binary_stream_t encode_unconnected_ping(unconnected_ping_t packet);
unconnected_pong_t decode_unconnected_pong(binary_stream_t *stream);
binary_stream_t encode_unconnected_pong(unconnected_pong_t packet);
open_connection_request_1_t decode_open_connection_request_1(binary_stream_t *stream);
binary_stream_t encode_open_connection_request_1(open_connection_request_1_t packet);
open_connection_reply_1_t decode_open_connection_reply_1(binary_stream_t *stream);
binary_stream_t encode_open_connection_reply_1(open_connection_reply_1_t packet);
open_connection_request_2_t decode_open_connection_request_2(binary_stream_t *stream);
binary_stream_t encode_open_connection_request_2(open_connection_request_2_t packet);
open_connection_reply_2_t decode_open_connection_reply_2(binary_stream_t *packet);
binary_stream_t encode_open_connection_reply_2(open_connection_reply_2_t packet);
connection_request_t decode_connection_request(binary_stream_t *stream);
binary_stream_t encode_connection_request(connection_request_t packet);
frame_set_t decode_frame_set(binary_stream_t *stream);
binary_stream_t encode_frame_set(frame_set_t packet);
acknowledgement_t decode_acknowledgement(binary_stream_t *stream);
binary_stream_t encode_acknowledgement(acknowledgement_t packet, unsigned char is_successful);

#endif