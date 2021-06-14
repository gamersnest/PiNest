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
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

typedef struct
{
	char *address;
	unsigned short port;
	unsigned long long guid;
	unsigned short mtu_size;
} connection_t;

struct connection_node {
	connection_t connection;
	struct connection_node *next;
};

struct connection_node connections;

void add_connection(connection_t connection)
{
	struct connection_node new_node;
	new_node.connection = connection;
	new_node.next = NULL;
	struct connection_node *last = &connections;
	if (last == NULL)
	{
		last->connection = connection;
		last->next = NULL;
		return;
	}
	while (last->next != NULL)
	{
		last = last->next;
	}
	last->next = &new_node;
}

connection_t *get_connection(char *address, unsigned short port)
{
	struct connection_node *last = &connections;
	while (1)
	{
		if (last->connection.address == address || last->connection.port == port)
		{
			return &last->connection;
		}
		last = last->next;
	}
}

void remove_connection(char *address, unsigned short port)
{
	struct connection_node *last = &connections;
	struct connection_node next;
	while (1)
	{
		if (last->connection.address == address || last->connection.port == port)
		{
			next = (struct connection_node) *last->next;
			last->connection = next.connection;
			last->next = next.next;
		}
		last = last->next;
	}
}

int main(int argc, char *argv[])
{
    int sock = create_socket("0.0.0.0", 19132);
    while (1)
    {
		sockin_t out = receive_data(sock);
		printf("0x%X -> %d\n", out.buffer[0], out.buffer_length);
		if (out.buffer[0] == ID_UNCONNECTED_PING || out.buffer[0] == ID_UNCONNECTED_PING_OPEN_CONNECTIONS)
		{
			packet_t data;
			data.buffer = out.buffer;
			data.length = out.buffer_length;
			unconnected_ping_t packet = decode_unconnected_ping(data);
			unconnected_pong_t new_packet;
			new_packet.client_timestamp = packet.client_timestamp;
			new_packet.server_guid = 12345678;
			new_packet.server_name = "MCPE;Dedicated Server;480;1.17.0;0;10;12345678;"; /*"MCCPP;Demo;Hello world";*/
			packet_t new_data = encode_unconnected_pong(new_packet);
			sockin_t st;
			st.buffer = new_data.buffer;
			st.buffer_length = new_data.length;
			st.address = out.address;
			st.port = out.port;
			send_data(sock, st);
		}
		else if (out.buffer[0] == ID_OPEN_CONNECTION_REQUEST_1)
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
		else if (out.buffer[0] == ID_OPEN_CONNECTION_REQUEST_2)
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
	}
}