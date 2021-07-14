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
#include "../../binary_stream/binary_stream.h"
#include "../../net/raknet/packets.h"
#include "../../net/raknet/reliability_util.h"
#include "../../net/raknet/message_identifiers.h"
#ifdef _WIN32

#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")

#endif
#ifdef linux

#include <arpa/inet.h>

#endif

char magic[16] = {
    0x00, 0xff, 0xff, 0x00,
    0xfe, 0xfe, 0xfe, 0xfe,
    0xfd, 0xfd, 0xfd, 0xfd,
    0x12, 0x34, 0x56, 0x78
};

char *get_string(binary_stream_t *stream)
{
    unsigned short length = get_unsigned_short_be(stream);
    char *value = get_bytes(length, stream);
    value = realloc(value, (length + 1) * sizeof(char));
    return value;
}

void put_string(char *value, binary_stream_t *stream)
{
    put_unsigned_short_be(strlen(value), stream);
    put_bytes(value, strlen(value), stream);
}

address_t get_address(binary_stream_t *stream)
{
    address_t address;
    address.version = get_unsigned_byte(stream);
    if (address.version == 4)
    {
        unsigned char part_1 = ~get_unsigned_byte(stream) & 0xff;
        unsigned char part_2 = ~get_unsigned_byte(stream) & 0xff;
        unsigned char part_3 = ~get_unsigned_byte(stream) & 0xff;
        unsigned char part_4 = ~get_unsigned_byte(stream) & 0xff;
        size_t size = snprintf(NULL, 0, "%d.%d.%d.%d", part_1, part_2, part_3, part_4);
        address.hostname = malloc(size);
        sprintf(address.hostname, "%d.%d.%d.%d", part_1, part_2, part_3, part_4);
        address.port = get_unsigned_short_be(stream);
    }
    else
    {
        // Todo IPv6
    }
    return address;
}

void put_address(address_t address, binary_stream_t *stream)
{
    put_unsigned_byte(address.version, stream);
    if (address.version == 4)
    {
        size_t packed_address = inet_addr(address.hostname);
        put_unsigned_byte(~(packed_address & 0xff), stream);
        put_unsigned_byte(~((packed_address >> 8) & 0xff) & 0xff, stream);
        put_unsigned_byte(~((packed_address >> 16) & 0xff) & 0xff, stream);
        put_unsigned_byte(~((packed_address >> 24) & 0xff) & 0xff, stream);
        put_unsigned_short_be(address.port, stream);
    }
    else
    {
        // Todo IPv6
    }
}

unconnected_ping_t decode_unconnected_ping(binary_stream_t *stream)
{
    unconnected_ping_t packet;
    ++stream->offset;
    packet.client_timestamp = get_unsigned_long_be(stream);
    stream->offset += 16;
    return packet;
}

binary_stream_t encode_unconnected_ping(unconnected_ping_t packet)
{
    binary_stream_t stream;
    stream.buffer = malloc(0);
    stream.offset = 0;
    stream.size = 0;
    put_unsigned_byte(ID_UNCONNECTED_PING, &stream);
    put_unsigned_long_be(packet.client_timestamp, &stream);
    put_bytes(magic, 16, &stream);
    return stream;
}

unconnected_pong_t decode_unconnected_pong(binary_stream_t *stream)
{
    unconnected_pong_t packet;
    packet.client_timestamp = get_unsigned_long_be(stream);
    packet.server_guid = get_unsigned_long_be(stream);
    stream->offset += 16;
    packet.server_name = get_string(stream);
    return packet;
}

binary_stream_t encode_unconnected_pong(unconnected_pong_t packet)
{
    binary_stream_t stream;
    stream.buffer = malloc(0);
    stream.offset = 0;
    stream.size = 0;
    put_unsigned_byte(ID_UNCONNECTED_PONG, &stream);
    put_unsigned_long_be(packet.client_timestamp, &stream);
    put_unsigned_long_be(packet.server_guid, &stream);
    put_bytes(magic, 16, &stream);
    put_string(packet.server_name, &stream);
    return stream;
}

open_connection_request_1_t decode_open_connection_request_1(binary_stream_t *stream)
{
    open_connection_request_1_t packet;
    stream->offset += 17;
    packet.protocol_version = get_unsigned_byte(stream);
    packet.mtu_size = stream->size - 18;
    return packet;
}

binary_stream_t encode_open_connection_request_1(open_connection_request_1_t packet)
{
    binary_stream_t stream;
    stream.buffer = malloc(0);
    stream.offset = 0;
    stream.size = 0;
    put_unsigned_byte(ID_OPEN_CONNECTION_REQUEST_1, &stream);
    put_bytes(magic, 16, &stream);
    put_unsigned_byte(packet.protocol_version, &stream);
    int i;
    for (i = 0; i < packet.mtu_size; ++i)
    {
        put_unsigned_byte(0x00, &stream);
    }
    return stream;
}

open_connection_reply_1_t decode_open_connection_reply_1(binary_stream_t *stream)
{
    open_connection_reply_1_t packet;
    stream->offset += 17;
    packet.server_guid = get_unsigned_long_be(stream);
    packet.use_security = get_unsigned_byte(stream);
    packet.mtu_size = get_unsigned_short_be(stream);
    return packet;
}

binary_stream_t encode_open_connection_reply_1(open_connection_reply_1_t packet)
{
    binary_stream_t stream;
    stream.buffer = malloc(0);
    stream.offset = 0;
    stream.size = 0;
    put_unsigned_byte(ID_OPEN_CONNECTION_REPLY_1, &stream);
    put_bytes(magic, 16, &stream);
    put_unsigned_long_be(packet.server_guid, &stream);
    put_unsigned_byte(packet.use_security, &stream);
    put_unsigned_short_be(packet.mtu_size, &stream);
    return stream;
}

open_connection_request_2_t decode_open_connection_request_2(binary_stream_t *stream)
{
    open_connection_request_2_t packet;
    stream->offset += 17;
    packet.server_address = get_address(stream);
    packet.mtu_size = get_unsigned_short_be(stream);
    packet.client_guid = get_unsigned_long_be(stream);
    return packet;
}

binary_stream_t encode_open_connection_request_2(open_connection_request_2_t packet)
{
    binary_stream_t stream;
    stream.buffer = malloc(0);
    stream.offset = 0;
    stream.size = 0;
    put_unsigned_byte(ID_OPEN_CONNECTION_REQUEST_2, &stream);
    put_bytes(magic, 16, &stream);
    put_address(packet.server_address, &stream);
    put_unsigned_short_be(packet.mtu_size, &stream);
    put_unsigned_long_be(packet.client_guid, &stream);
    return stream;
}

open_connection_reply_2_t decode_open_connection_reply_2(binary_stream_t *stream)
{
    open_connection_reply_2_t packet;
    stream->offset += 17;
    packet.server_guid = get_unsigned_long_be(stream);
    packet.client_address = get_address(stream);
    packet.mtu_size = get_unsigned_short_be(stream);
    packet.use_encryption = get_unsigned_byte(stream);
    return packet;
}

binary_stream_t encode_open_connection_reply_2(open_connection_reply_2_t packet)
{
    binary_stream_t stream;
    stream.buffer = malloc(0);
    stream.offset = 0;
    stream.size = 0;
    put_unsigned_byte(ID_OPEN_CONNECTION_REPLY_2, &stream);
    put_bytes(magic, 16, &stream);
    put_unsigned_long_be(packet.server_guid, &stream);
    put_address(packet.client_address, &stream);
    put_unsigned_short_be(packet.mtu_size, &stream);
    put_unsigned_byte(packet.use_encryption, &stream);
    return stream;
}

connection_request_t decode_connection_request(binary_stream_t *stream)
{
    connection_request_t packet;
    ++stream->offset;
    packet.client_guid = get_unsigned_long_be(stream);
    packet.request_timestamp = get_unsigned_long_be(stream);
    return packet;
}

binary_stream_t encode_connection_request(connection_request_t packet)
{
    binary_stream_t stream;
    stream.buffer = malloc(0);
    stream.offset = 0;
    stream.size = 0;
    put_unsigned_byte(ID_CONNECTION_REQUEST, &stream);
    put_unsigned_long_be(packet.client_guid, &stream);
    put_unsigned_long_be(packet.request_timestamp, &stream);
    return stream;
}

frame_set_t decode_frame_set(packet_t packet)
{
    frame_set_t result;
    result.sequence_number = ((packet.buffer[3] & 0xff) << 16);
    result.sequence_number |= ((packet.buffer[2] & 0xff) << 8);
    result.sequence_number |= packet.buffer[1] & 0xff;
    int offset = 4;
    result.frames = malloc(sizeof(frame_t));
    result.frame_count = 0;
    frame_t frame;
    while (offset < packet.length)
    {
        frame_t frame;
        unsigned char flags = packet.buffer[offset] & 0xff;
        offset += 1;
        frame.reliability = (flags & 0xf4) >> 5;
        frame.is_fragmented = (flags & 0x10) > 0;
        frame.body_length = ((packet.buffer[offset] & 0xff) << 8);
        offset += 1;
        frame.body_length |= packet.buffer[offset] & 0xff;
        offset += 1;
        frame.body_length >>= 3;
        if (is_reliable(frame.reliability) == 1)
        {
            offset += 2;
            frame.reliable_frame_index = ((packet.buffer[offset] & 0xff) << 16);
            frame.reliable_frame_index |= ((packet.buffer[offset - 1] & 0xff) << 8);
            frame.reliable_frame_index |= packet.buffer[offset - 2] & 0xff;
            offset += 1;
        }
        if (is_sequenced(frame.reliability) == 1)
        {
            offset += 2;
            frame.sequenced_frame_index = ((packet.buffer[offset] & 0xff) << 16);
            frame.sequenced_frame_index |= ((packet.buffer[offset - 1] & 0xff) << 8);
            frame.sequenced_frame_index |= packet.buffer[offset - 2] & 0xff;
            offset += 1;
        }
        if (is_ordered(frame.reliability) == 1)
        {
            offset += 2;
            frame.ordered_frame_index = ((packet.buffer[offset] & 0xff) << 16);
            frame.ordered_frame_index |= ((packet.buffer[offset - 1] & 0xff) << 8);
            frame.ordered_frame_index |= packet.buffer[offset - 2] & 0xff;
            offset += 1;
            frame.order_channel = packet.buffer[offset] & 0xff;
        }
        if (frame.is_fragmented == 1)
        {
            frame.compound_size = ((packet.buffer[offset] & 0xff) << 24);
            offset += 1;
            frame.compound_size |= ((packet.buffer[offset] & 0xff) << 16);
            offset += 1;
            frame.compound_size |= ((packet.buffer[offset] & 0xff) << 8);
            offset += 1;
            frame.compound_size |= packet.buffer[offset] & 0xff;
            offset += 1;
            frame.compound_id = ((packet.buffer[offset] & 0xff) << 8);
            offset += 1;
            frame.compound_id |= packet.buffer[offset] & 0xff;
            offset += 1;
            frame.index = ((packet.buffer[offset] & 0xff) << 24);
            offset += 1;
            frame.index |= ((packet.buffer[offset] & 0xff) << 16);
            offset += 1;
            frame.index |= ((packet.buffer[offset] & 0xff) << 8);
            offset += 1;
            frame.index |= packet.buffer[offset] & 0xff;
            offset += 1;
        }
        frame.body = malloc(frame.body_length);
        int i;
        for (i = 0; i < frame.body_length; ++i)
        {
            frame.body[i] = packet.buffer[offset + i];
        }
        offset += frame.body_length;
        ++result.frame_count;
        result.frames = realloc(result.frames, result.frame_count * sizeof(frame_t));
        result.frames[result.frame_count - 1] = frame;
        break;
    }
    return result;
}

packet_t encode_frame_set(frame_set_t packet)
{
    unsigned int frames_length = 0;
    int i;
    for (i = 0; i < packet.frame_count; ++i)
    {
        frames_length += 3;
        if (is_reliable(packet.frames[i].reliability))
        {
            frames_length += 3;
        }
        if (is_sequenced(packet.frames[i].reliability))
        {
            frames_length += 3;
        }
        if (is_ordered(packet.frames[i].reliability))
        {
            frames_length += 4;
        }
        if (packet.frames[i].is_fragmented == 1)
        {
            frames_length += 4;
        }
        frames_length += packet.frames[i].body_length;
    }

    char *buffer = malloc(4 + frames_length);
    buffer[0] = 0x80;
    buffer[3] = (packet.sequence_number >> 16) & 0xff;
    buffer[2] = (packet.sequence_number >> 8) & 0xff;
    buffer[1] = packet.sequence_number & 0xff;
    int offset = 4;
    for (i = 0; i < packet.frame_count; ++i)
    {
        if (packet.frames[i].is_fragmented == 1)
        {
            buffer[offset] = packet.frames[i].reliability | 0x10;
        }
        else {
            buffer[offset] = packet.frames[i].reliability;
        }
        offset += 1;
        unsigned short body_bit_length = packet.frames[i].body_length << 3;
        buffer[offset] = (body_bit_length >> 8) & 0xff;
        offset += 1;
        buffer[offset] = body_bit_length & 0xff;
        offset += 1;
        if (is_reliable(packet.frames[i].reliability))
        {
            buffer[2 + offset] = (packet.frames[i].reliable_frame_index >> 16) & 0xff;
            buffer[1 + offset] = (packet.frames[i].reliable_frame_index >> 8) & 0xff;
            buffer[offset] = packet.frames[i].reliable_frame_index & 0xff;
            offset += 3;
        }
        if (is_sequenced(packet.frames[i].reliability))
        {
            buffer[2 + offset] = (packet.frames[i].sequenced_frame_index >> 16) & 0xff;
            buffer[1 + offset] = (packet.frames[i].sequenced_frame_index >> 8) & 0xff;
            buffer[offset] = packet.frames[i].sequenced_frame_index & 0xff;
            offset += 3;
        }
        if (is_ordered(packet.frames[i].reliability))
        {
            buffer[2 + offset] = (packet.frames[i].ordered_frame_index >> 16) & 0xff;
            buffer[1 + offset] = (packet.frames[i].ordered_frame_index >> 8) & 0xff;
            buffer[offset] = packet.frames[i].ordered_frame_index & 0xff;
            offset += 3;
            buffer[offset] = packet.frames[i].order_channel & 0xff;
            offset += 1;
        }
        if (packet.frames[i].is_fragmented == 1)
        {
            buffer[offset] = (packet.frames[i].compound_size >> 24) & 0xff;
            offset += 1;
            buffer[offset] = (packet.frames[i].compound_size >> 16) & 0xff;
            offset += 1;
            buffer[offset] = (packet.frames[i].compound_size >> 8) & 0xff;
            offset += 1;
            buffer[offset] = packet.frames[i].compound_size & 0xff;
            offset += 1;
            buffer[offset] = (packet.frames[i].compound_id >> 8) & 0xff;
            offset += 1;
            buffer[offset] = packet.frames[i].compound_id & 0xff;
            offset += 1;
            buffer[offset] = (packet.frames[i].index >> 24) & 0xff;
            offset += 1;
            buffer[offset] = (packet.frames[i].index >> 16) & 0xff;
            offset += 1;
            buffer[offset] = (packet.frames[i].index >> 8) & 0xff;
            offset += 1;
            buffer[offset] = packet.frames[i].index & 0xff;
            offset += 1;
        }
        int j;
        for (j = 0; j < packet.frames[i].body_length; ++j)
        {
            buffer[offset] = packet.frames[i].body[j] & 0xff;
            offset += 1;
        }
    }
    packet_t result;
    result.buffer = buffer;
    result.length = 4 + frames_length;
    return result;
}