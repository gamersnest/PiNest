/*
    ____  _ _   _           _   
   |  _ \(_) \ | | ___  ___| |_ 
   | |_) | |  \| |/ _ \/ __| __|
   |  __/| | |\  |  __/\__ \ |_ 
   |_|   |_|_| \_|\___||___/\__|

   Copyright Alexander Argentakis & gamersnest.org

   This file is licensed under the GPL v2.0 license

 */

#include "net/raknet/packets.h"
#include "net/raknet/message_identifiers.h"
#include "net/socket.h"
#include <arpa/inet.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    int sock = create_socket("0.0.0.0", 19132);
    while (1)
    {
		sockin_t out = receive_data(sock);
		printf("0x%X -> %d\n", out.buffer[0], out.buffer_length);
		if (out.buffer[0] == ID_UNCONNECTED_PING_OPEN_CONNECTIONS)
		{
			packet_t data;
			data.buffer = out.buffer;
			data.length = out.buffer_length;
			unconnected_ping_t packet = decode_unconnected_ping(data);
			unconnected_pong_t new_packet;
			new_packet.client_timestamp = packet.client_timestamp;
			new_packet.server_guid = 12345678;
			new_packet.server_name = "MCCPP;Demo;Hello world";
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
			printf("RakNet Protocol Version -> %d\n", packet.protocol_version);
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
			printf("Server address -> %s:%d\n", packet.server_address, packet.server_port);
		}
	}
}