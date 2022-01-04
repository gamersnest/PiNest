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

#else

#include <arpa/inet.h>

#endif

char MAGIC[16] = {
        0x00, static_cast<char>(0xff), static_cast<char>(0xff), 0x00,
        static_cast<char>(0xfe), static_cast<char>(0xfe), static_cast<char>(0xfe), static_cast<char>(0xfe),
        static_cast<char>(0xfd), static_cast<char>(0xfd), static_cast<char>(0xfd), static_cast<char>(0xfd),
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
        unsigned int size = snprintf(NULL, 0, "%d.%d.%d.%d", part_1, part_2, part_3, part_4);
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
        unsigned int packed_address = inet_addr(address.hostname);
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

connection_request_accepted_t decode_connection_request_accepted(binary_stream_t *stream)
{
    connection_request_accepted_t packet;
    ++stream->offset;
    packet.client_address = get_address(stream);
    int i;
    for (i = 0; i < 10; ++i)
    {
        get_address(stream);
    }
    packet.request_timestamp = get_unsigned_long_be(stream);
    packet.accepted_timestamp = get_unsigned_long_be(stream);
    return packet;
}

binary_stream_t encode_connection_request_accepted(connection_request_accepted_t packet)
{
    binary_stream_t stream;
    stream.buffer = malloc(0);
    stream.offset = 0;
    stream.size = 0;
    put_unsigned_byte(ID_CONNECTION_REQUEST_ACCEPTED, &stream);
    put_address(packet.client_address, &stream);
    int i;
    for (i = 0; i < 10; ++i)
    {
        address_t address;
        address.hostname = "255.255.255.255";
        address.port = 0;
        address.version = 4;
        put_address(address, &stream);
    }
    put_unsigned_long_be(packet.request_timestamp, &stream);
    put_unsigned_long_be(packet.accepted_timestamp, &stream);
    return stream;
}

connected_ping_t decode_connected_ping(binary_stream_t *stream)
{
    connected_ping_t packet;
    packet.client_timestamp = get_unsigned_long_be(stream);
    return packet;
}

binary_stream_t encode_connected_ping(connected_ping_t packet)
{
    binary_stream_t stream;
    stream.buffer = malloc(0);
    stream.offset = 0;
    stream.size = 0;
    put_unsigned_byte(ID_CONNECTED_PING, &stream);
    put_unsigned_long_be(packet.client_timestamp, &stream);
    return stream;
}

connected_pong_t decode_connected_pong(binary_stream_t *stream)
{
    connected_pong_t packet;
    packet.client_timestamp = get_unsigned_long_be(stream);
    return packet;
}

binary_stream_t encode_connected_pong(connected_pong_t packet)
{
    binary_stream_t stream;
    stream.buffer = malloc(0);
    stream.offset = 0;
    stream.size = 0;
    put_unsigned_byte(ID_CONNECTED_PONG, &stream);
    put_unsigned_long_be(packet.client_timestamp, &stream);
    return stream;
}

frame_set_t decode_frame_set(binary_stream_t *stream)
{
    frame_set_t packet;
    ++stream->offset;
    packet.sequence_number = get_unsigned_triad_le(stream);
    packet.frames = malloc(0);
    packet.frame_count = 0;
    frame_t frame;
    while (stream->offset < stream->size)
    {
        frame_t frame;
        unsigned char flags = get_unsigned_byte(stream) & 0xff;
        frame.reliability = (flags & 0xf4) >> 5;
        frame.is_fragmented = (flags & 0x10) > 0;
        frame.body_length = get_unsigned_short_be(stream) >> 3;
        if (is_reliable(frame.reliability) == 1)
        {
            frame.reliable_frame_index = get_unsigned_triad_le(stream);
        }
        if (is_sequenced(frame.reliability) == 1)
        {
            frame.sequenced_frame_index = get_unsigned_triad_le(stream);
        }
        if (is_ordered(frame.reliability) == 1)
        {
            frame.ordered_frame_index = get_unsigned_triad_le(stream);
            frame.order_channel = get_unsigned_byte(stream);
        }
        if (frame.is_fragmented == 1)
        {
            frame.compound_size = get_unsigned_int_be(stream);
            frame.compound_id = get_unsigned_short_be(stream);
            frame.index = get_unsigned_int_be(stream);
        }
        frame.body = get_bytes(frame.body_length, stream);
        ++packet.frame_count;
        packet.frames = realloc(packet.frames, packet.frame_count * sizeof(frame_t));
        packet.frames[packet.frame_count - 1] = frame;
        break;
    }
    return packet;
}

binary_stream_t encode_frame_set(frame_set_t packet)
{
    binary_stream_t stream;
    stream.buffer = malloc(0);
    stream.offset = 0;
    stream.size = 0;
    put_unsigned_byte(0x80, &stream);
    put_unsigned_triad_le(packet.sequence_number, &stream);
    int i;
    for (i = 0; i < packet.frame_count; ++i)
    {
        if (packet.frames[i].is_fragmented == 1)
        {
            put_unsigned_byte(packet.frames[i].reliability | 0x10, &stream);
        }
        else
        {
            put_unsigned_byte(packet.frames[i].reliability, &stream);
        }
        put_unsigned_short_be(packet.frames[i].body_length << 3, &stream);
        if (is_reliable(packet.frames[i].reliability))
        {
            put_unsigned_triad_le(packet.frames[i].reliable_frame_index, &stream);
        }
        if (is_sequenced(packet.frames[i].reliability))
        {
            put_unsigned_triad_le(packet.frames[i].sequenced_frame_index, &stream);
        }
        if (is_ordered(packet.frames[i].reliability))
        {
            put_unsigned_triad_le(packet.frames[i].ordered_frame_index, &stream);
            put_unsigned_byte(packet.frames[i].order_channel, &stream);
        }
        if (packet.frames[i].is_fragmented == 1)
        {
            put_unsigned_int_be(packet.frames[i].compound_size, &stream);
            put_unsigned_short_be(packet.frames[i].compound_id, &stream);
            put_unsigned_int_be(packet.frames[i].index, &stream);
        }
        put_bytes(packet.frames[i].body, packet.frames[i].body_length, &stream);
    }
    return stream;
}

acknowledgement_t decode_acknowledgement(binary_stream_t *stream)
{
    acknowledgement_t packet;
    ++stream->offset;
    packet.sequence_numbers = malloc(0);
    packet.sequence_numbers_count = 0;
    unsigned short record_count = get_unsigned_short_be(stream);
    int i;
    for (i = 0; i < record_count; ++i)
    {
        unsigned char is_single = get_unsigned_byte(stream);
        int record_start = get_unsigned_triad_le(stream);
        int record_end = record_start;
        if (is_single == 0)
        {
            record_end = get_unsigned_triad_le(stream);
        }
        int record;
        for (record = record_start; record <= record_end; ++record)
        {
            ++packet.sequence_numbers_count;
            packet.sequence_numbers = realloc(packet.sequence_numbers, packet.sequence_numbers_count * sizeof(unsigned int));
            packet.sequence_numbers[packet.sequence_numbers_count - 1] = record;
        }
    }
    return packet;
}

binary_stream_t encode_acknowledgement(acknowledgement_t packet, unsigned char is_successful)
{
    binary_stream_t stream;
    stream.buffer = malloc(0);
    stream.offset = 0;
    stream.size = 0;
    if (is_successful == 0)
    {
        put_unsigned_byte(0xa0, &stream);
    }
    else
    {
        put_unsigned_byte(0xc0, &stream);
    }
    binary_stream_t records_stream;
    records_stream.buffer = malloc(0);
    records_stream.offset = 0;
    records_stream.size = 0;
    unsigned short cnt = 0;
    if (packet.sequence_numbers_count > 0)
    {
        unsigned int start_index = packet.sequence_numbers[0];
        unsigned int end_index = packet.sequence_numbers[0];
        int pointer;
        for (pointer = 1; pointer < packet.sequence_numbers_count; ++pointer)
        {
            unsigned int current_index = packet.sequence_numbers[pointer];
            unsigned int diff = current_index - end_index;
            if (diff == 1)
            {
                end_index = current_index;
            }
            else if (diff > 1)
            {
                if (start_index == end_index)
                {
                    put_unsigned_byte(1, &records_stream);
                    put_unsigned_triad_le(start_index, &records_stream);
                    start_index = end_index = current_index;
                }
                else
                {
                    put_unsigned_byte(0, &records_stream);
                    put_unsigned_triad_le(start_index, &records_stream);
                    put_unsigned_triad_le(end_index, &records_stream);
                    start_index = end_index = current_index;
                }
                ++cnt;
            }
        }
        if (start_index == end_index)
        {
            put_unsigned_byte(1, &records_stream);
            put_unsigned_triad_le(start_index, &records_stream);
        }
        else
        {
            put_unsigned_byte(0, &records_stream);
            put_unsigned_triad_le(start_index, &records_stream);
            put_unsigned_triad_le(end_index, &records_stream);
        }
        ++cnt;
    }
    put_short_be(cnt, &stream);
    put_bytes(records_stream.buffer, records_stream.size, &stream);
    return stream;
}
