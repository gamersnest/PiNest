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

typedef struct
{
    char *buffer;
    int length;
} packet_t;

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
    char *server_address;
    unsigned short server_port;
    unsigned short mtu_size;
    unsigned long long client_guid;
} open_connection_request_2_t;

typedef struct
{
    unsigned long long server_guid;
    char *client_address;
    unsigned short client_port;
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

struct frame_set_node *append_frame(struct frame_set_node *frame_set, frame_t frame);
unconnected_ping_t decode_unconnected_ping(packet_t packet);
packet_t encode_unconnected_ping(unconnected_ping_t packet);
unconnected_pong_t decode_unconnected_pong(packet_t packet);
packet_t encode_unconnected_pong(unconnected_pong_t packet);
open_connection_request_1_t decode_open_connection_request_1(packet_t packet);
packet_t open_connection_request_1(open_connection_request_1_t packet);
open_connection_reply_1_t decode_open_connection_reply_1(packet_t packet);
packet_t encode_open_connection_reply_1(open_connection_reply_1_t packet);
open_connection_request_2_t decode_open_connection_request_2(packet_t packet);
packet_t encode_open_connection_request_2(open_connection_request_2_t packet);
open_connection_reply_2_t decode_open_connection_reply_2(packet_t packet);
packet_t encode_open_connection_reply_2(open_connection_reply_2_t packet);
frame_set_t decode_frame_set(packet_t packet);
packet_t encode_frame_set(frame_set_t packet);

#endif