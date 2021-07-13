/*
    ____  _ _   _           _   
   |  _ \(_) \ | | ___  ___| |_ 
   | |_) | |  \| |/ _ \/ __| __|
   |  __/| | |\  |  __/\__ \ |_ 
   |_|   |_|_| \_|\___||___/\__|

   Copyright Alexander Argentakis & gamersnest.org

   This file is licensed under the GPL v2.0 license

 */

#include "./net/raknet/packets.h"
#include "./net/raknet/message_identifiers.h"
#include "./net/socket.h"
#ifdef _WIN32

#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")

#endif
#ifdef linux

#include <arpa/inet.h>

#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
	char *address;
	unsigned short port;
	unsigned long long guid;
	unsigned short mtu_size;
} connection_t;

connection_t *connections;
unsigned int connection_count;

void init_connections()
{
	connections = malloc(sizeof(connection_t));
	connection_count = 0;
}

void add_connection(connection_t connection)
{
	int i;
	for (i = 0; i < connection_count; ++i)
	{
		if (connection.address == connections[i].address && connection.port == connections[i].port)
		{
			return;
		}
	}
	++connection_count;
    connections = realloc(connections, connection_count * sizeof(connection_t));
    connections[connection_count - 1] = connection;
}

connection_t *get_connection(char *address, unsigned short port)
{
	int i;
	for (i = 0; i < connection_count; ++i)
	{
		if (address == connections[i].address && port == connections[i].port)
		{
			return &connections[i];
		}
	}
	return NULL;
}

void remove_connection(char *address, unsigned short port)
{
	int i;
	for (i = 0; i < connection_count; ++i)
	{
		if (address == connections[i].address && port == connections[i].port)
		{
			connection_t *temp;
			unsigned int cnt = 0;
			int j;
			for (j = 0; j < i; ++j) {
				++cnt;
    			temp = realloc(temp, cnt * sizeof(connection_t));
    			temp[cnt - 1] = connections[j];
			}
			int k;
			for (k = i + 1; k < connection_count; ++k)
			{
				++cnt;
    			temp = realloc(temp, cnt * sizeof(connection_t));
    			temp[cnt - 1] = connections[k];
			}
			--connection_count;
			connections = temp;
			break;
		}
	}
}

int main(int argc, char *argv[])
{
	init_connections();
    int sock = create_socket("0.0.0.0", 19132);
    while (1)
    {
		sockin_t out = receive_data(sock);
		printf("0x%X -> %d\n", out.buffer[0] & 0xff, out.buffer_length);
		if ((out.buffer[0] & 0xff) == ID_UNCONNECTED_PING || out.buffer[0] == ID_UNCONNECTED_PING_OPEN_CONNECTIONS)
		{
			packet_t data;
			data.buffer = out.buffer;
			data.length = out.buffer_length;
			unconnected_ping_t packet = decode_unconnected_ping(data);
			unconnected_pong_t new_packet;
			new_packet.client_timestamp = packet.client_timestamp;
			new_packet.server_guid = 12345678;
			new_packet.server_name = "MCCPP;Demo;Dedicated Server"; /*"MCCPP;Demo;Hello world";*/
			packet_t new_data = encode_unconnected_pong(new_packet);
			sockin_t st;
			st.buffer = new_data.buffer;
			st.buffer_length = new_data.length;
			st.address = out.address;
			st.port = out.port;
			send_data(sock, st);
		}
		else if ((out.buffer[0] & 0xff) == ID_OPEN_CONNECTION_REQUEST_1)
		{
			packet_t data;
			data.buffer = out.buffer;
			data.length = out.buffer_length;
			open_connection_request_1_t packet = decode_open_connection_request_1(data);
			open_connection_reply_1_t new_packet;
			new_packet.server_guid = 12345678;
			new_packet.use_security = 0;
			new_packet.mtu_size = packet.mtu_size;
			packet_t new_data = encode_open_connection_reply_1(new_packet);
			sockin_t st;
			st.buffer = new_data.buffer;
			st.buffer_length = new_data.length;
			st.address = out.address;
			st.port = out.port;
			send_data(sock, st);
		}
		else if ((out.buffer[0] & 0xff) == ID_OPEN_CONNECTION_REQUEST_2)
		{
			packet_t data;
			data.buffer = out.buffer;
			data.length = out.buffer_length;
			open_connection_request_2_t packet = decode_open_connection_request_2(data);
			open_connection_reply_2_t new_packet;
			new_packet.server_guid = 12345678;
			new_packet.client_address = out.address;
			new_packet.client_port = out.port;
			new_packet.mtu_size = packet.mtu_size;
			new_packet.use_encryption = 0;
			packet_t new_data = encode_open_connection_reply_2(new_packet);
			sockin_t st;
			st.buffer = new_data.buffer;
			st.buffer_length = new_data.length;
			st.address = out.address;
			st.port = out.port;
			send_data(sock, st);
			connection_t new_connection;
			new_connection.address = out.address;
			new_connection.port = out.port;
			new_connection.guid = packet.client_guid;
			new_connection.mtu_size = packet.mtu_size;
			add_connection(new_connection);
			printf("%lld\n", new_connection.guid);
			printf("%lld\n", get_connection(out.address, out.port)->guid);
		}
		else if ((out.buffer[0] & 0xff) >= 0x80 && (out.buffer[0] & 0xff) <= 0x8f)
		{
			packet_t data;
			data.buffer = out.buffer;
			data.length = out.buffer_length;
			frame_set_t packet = decode_frame_set(data);
			printf("SEQUENCE NUMBER -> %u\n", packet.sequence_number);
			printf("FRAME COUNT -> %u\n", packet.frame_count);
			printf("CUSTOM PACKET -> 0x%X\n", packet.frames[0].body[0] & 0xff);
		}
	}
}