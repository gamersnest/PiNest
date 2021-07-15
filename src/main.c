/*
    ____  _ _   _           _   
   |  _ \(_) \ | | ___  ___| |_ 
   | |_) | |  \| |/ _ \/ __| __|
   |  __/| | |\  |  __/\__ \ |_ 
   |_|   |_|_| \_|\___||___/\__|

   Copyright Alexander Argentakis & gamersnest.org

   This file is licensed under the GPL v2.0 license

 */

#include "./binary_stream/binary_stream.h"
#include "./net/raknet/packets.h"
#include "./net/raknet/message_identifiers.h"
#include "./net/socket.h"
#include "./logger.h"
#ifdef _WIN32

#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")

#else

#include <arpa/inet.h>

#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct
{
	frame_set_t *queue;
	size_t size;
} recovery_queue_t;

typedef struct
{
	char *address;
	unsigned short port;
	unsigned long long guid;
	unsigned short mtu_size;
	unsigned int last_sequence_number;
	unsigned int send_sequence_number;
	unsigned char has_connected;
	recovery_queue_t recovery_queue;
	frame_set_t queue;
	char *username;
} connection_t;

connection_t *connections;
unsigned int connection_count;

void send_queue(connection_t *connection, int sock)
{
	if (connection->queue.frame_count > 0)
	{
		connection->queue.sequence_number = connection->send_sequence_number;
		++connection->send_sequence_number;
		binary_stream_t stream = encode_frame_set(connection->queue);
		sockin_t st;
		st.buffer = stream.buffer;
		st.buffer_length = stream.size;
		st.address = connection->address;
		st.port = connection->port;
		send_data(sock, st);
		++connection->recovery_queue.size;
		connection->recovery_queue.queue = realloc(connection->recovery_queue.queue, connection->recovery_queue.size * sizeof(frame_set_t));
		connection->recovery_queue.queue[connection->recovery_queue.size - 1] = connection->queue;
		connection->queue.sequence_number = 0;
		connection->queue.frame_count = 0;
		connection->queue.frames = malloc(0);
	}
}

void add_to_queue(frame_t frame, unsigned char is_immediate, connection_t *connection, int sock)
{
	++connection->queue.frame_count;
	connection->queue.frames = realloc(connection->queue.frames, connection->queue.frame_count * sizeof(frame_t));
	connection->queue.frames[connection->queue.frame_count - 1] = frame;
	if (is_immediate == 1)
	{
		send_queue(connection, sock);
	}
	else
	{
		// Todo non immediate
	}
}

void init_connections()
{
	connections = malloc(0);
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
			connection_t *temp = malloc(0);
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

char *concate(char* a, char *b)
{
	size_t size = snprintf(NULL, 0, "%s%s", a, b);
	char *result = malloc(size + 1);
	sprintf(result, "%s%s", a, b);
	result[size] = 0x00;
	return result;
}

int has_put_input()
{
  struct timeval tv;
  fd_set fds;
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  FD_ZERO(&fds);
  FD_SET(fileno(stdin), &fds);
  select(fileno(stdin) + 1, &fds, NULL, NULL, &tv);
  return (FD_ISSET(0, &fds));
}

int main(int argc, char *argv[])
{
	log_info("Starting up...");
	init_connections();
    int sock = create_socket("0.0.0.0", 19132);
	char *command = malloc(1);
	command[0] = 0x00;
	log_success("Done. Type \"help\" or \"?\" to view all available commands.");
    while (1)
    {
		if (has_put_input() == 1)
		{
			int line_size = 1000;
			char* input = malloc(line_size);
			fgets(input, line_size, stdin);
			input[strlen(input) - 1] = 0x00;
			if (strcmp(input, "stop") == 0)
			{
				exit(0);
			}
			else if (strcmp(input, "help") == 0 || strcmp(input, "?") == 0)
			{
				log_info("--- Showing help ---");
				log_info("help | Shows all available commands");
				log_info("stop | Stops the server");
				log_info("version | Shows PiNest's version info");
			}
			else if (strcmp(input, "version") == 0 || strcmp(input, "ver") == 0 || strcmp(input, "about") == 0)
			{
				log_info("This server is running PiNest version 0.1-alpha for mcpi 0.1.1");
			}
			else
			{
				log_info("Command not found.");
			}
		}
		sockin_t out = receive_data(sock);
		if (out.buffer_length > 0)
		{
			if ((out.buffer[0] & 0xff) == ID_UNCONNECTED_PING || out.buffer[0] == ID_UNCONNECTED_PING_OPEN_CONNECTIONS)
			{
				binary_stream_t stream;
    			stream.buffer = out.buffer;
    			stream.offset = 0;
    			stream.size = out.buffer_length;
				unconnected_ping_t packet = decode_unconnected_ping(&stream);
				unconnected_pong_t new_packet;
				new_packet.client_timestamp = packet.client_timestamp;
				new_packet.server_guid = 12345678;
				new_packet.server_name = "MCCPP;MINECON;Dedicated Server";
				binary_stream_t new_stream = encode_unconnected_pong(new_packet);
				sockin_t st;
				st.buffer = new_stream.buffer;
				st.buffer_length = new_stream.size;
				st.address = out.address;
				st.port = out.port;
				send_data(sock, st);
			}
			else if ((out.buffer[0] & 0xff) == ID_OPEN_CONNECTION_REQUEST_1)
			{
				binary_stream_t stream;
    			stream.buffer = out.buffer;
    			stream.offset = 0;
    			stream.size = out.buffer_length;
				open_connection_request_1_t packet = decode_open_connection_request_1(&stream);
				open_connection_reply_1_t new_packet;
				new_packet.server_guid = 12345678;
				new_packet.use_security = 0;
				new_packet.mtu_size = packet.mtu_size;
				binary_stream_t new_stream = encode_open_connection_reply_1(new_packet);
				sockin_t st;
				st.buffer = new_stream.buffer;
				st.buffer_length = new_stream.size;
				st.address = out.address;
				st.port = out.port;
				send_data(sock, st);
			}
			else if ((out.buffer[0] & 0xff) == ID_OPEN_CONNECTION_REQUEST_2)
			{
				binary_stream_t stream;
    			stream.buffer = out.buffer;
    			stream.offset = 0;
    			stream.size = out.buffer_length;
				open_connection_request_2_t packet = decode_open_connection_request_2(&stream);
				open_connection_reply_2_t new_packet;
				new_packet.server_guid = 12345678;
				new_packet.client_address.hostname = out.address;
				new_packet.client_address.port = out.port;
				new_packet.client_address.version = 4;
				new_packet.mtu_size = packet.mtu_size;
				new_packet.use_encryption = 0;
				binary_stream_t new_stream = encode_open_connection_reply_2(new_packet);
				sockin_t st;
				st.buffer = new_stream.buffer;
				st.buffer_length = new_stream.size;
				st.address = out.address;
				st.port = out.port;
				send_data(sock, st);
				connection_t new_connection;
				new_connection.address = out.address;
				new_connection.port = out.port;
				new_connection.guid = packet.client_guid;
				new_connection.mtu_size = packet.mtu_size;
				new_connection.last_sequence_number = 0;
				new_connection.has_connected = 0;
				new_connection.send_sequence_number = 0;
				new_connection.queue.sequence_number = 0;
				new_connection.queue.frame_count = 0;
				new_connection.queue.frames = malloc(0);
				new_connection.recovery_queue.queue = malloc(0);
				new_connection.recovery_queue.size = 0;
				add_connection(new_connection);
			}
			else if ((out.buffer[0] & 0xff) == 0xa0)
			{
				binary_stream_t stream;
    			stream.buffer = out.buffer;
    			stream.offset = 0;
    			stream.size = out.buffer_length;
				acknowledgement_t nack = decode_acknowledgement(&stream);
				connection_t *connection = get_connection(out.address, out.port);
				recovery_queue_t recovery_queue;
				recovery_queue.queue = malloc(0);
				recovery_queue.size = 0;
				int i;
				for (i = 0; i < connection->recovery_queue.size; ++i)
				{
					int j;
					int none = 1;
					for (j = 0; j < nack.sequence_numbers_count; ++j)
					{
						if (connection->recovery_queue.queue[i].sequence_number == nack.sequence_numbers[j])
						{
							int k;
							for (k = 0; k < connection->recovery_queue.queue[i].frame_count; ++k)
							{
								add_to_queue(connection->recovery_queue.queue[i].frames[k], 1, connection, sock);
							}
							none = 0;
						}
					}
					if (none == 1)
					{
						++recovery_queue.size;
						recovery_queue.queue = realloc(recovery_queue.queue, recovery_queue.size * sizeof(frame_set_t));
						recovery_queue.queue[recovery_queue.size - 1] = connection->recovery_queue.queue[i];
					}
				}
				connection->recovery_queue = recovery_queue;
			}
			else if ((out.buffer[0] & 0xff) == 0xc0)
			{
				binary_stream_t stream;
    			stream.buffer = out.buffer;
    			stream.offset = 0;
    			stream.size = out.buffer_length;
				acknowledgement_t ack = decode_acknowledgement(&stream);
				connection_t *connection = get_connection(out.address, out.port);
				recovery_queue_t recovery_queue;
				recovery_queue.queue = malloc(0);
				recovery_queue.size = 0;
				int i;
				for (i = 0; i < connection->recovery_queue.size; ++i)
				{
					int j;
					int none = 1;
					for (j = 0; j < ack.sequence_numbers_count; ++j)
					{
						if (connection->recovery_queue.queue[i].sequence_number == ack.sequence_numbers[j])
						{
							none = 0;
						}
					}
					if (none == 1)
					{
						++recovery_queue.size;
						recovery_queue.queue = realloc(recovery_queue.queue, recovery_queue.size * sizeof(frame_set_t));
						recovery_queue.queue[recovery_queue.size - 1] = connection->recovery_queue.queue[i];
					}
				}
				connection->recovery_queue = recovery_queue;
			}
			else if ((out.buffer[0] & 0xff) >= 0x80 && (out.buffer[0] & 0xff) <= 0x8f)
			{
				connection_t *connection = get_connection(out.address, out.port);
				sockin_t st;
				binary_stream_t stream;
    			stream.buffer = out.buffer;
    			stream.offset = 0;
    			stream.size = out.buffer_length;
				frame_set_t packet = decode_frame_set(&stream);
				if (connection->last_sequence_number < packet.sequence_number)
				{
					acknowledgement_t nack;
					nack.sequence_numbers_count = packet.sequence_number - connection->last_sequence_number;
					nack.sequence_numbers = malloc(nack.sequence_numbers_count * sizeof(unsigned int));
					int i;
					int ii = 0;
					for (i = 0; i < nack.sequence_numbers_count; ++i)
					{
						nack.sequence_numbers[i] = i + connection->last_sequence_number;
					}
					binary_stream_t nack_stream = encode_acknowledgement(nack, 0);
					st.buffer = nack_stream.buffer;
					st.buffer_length = nack_stream.size;
					st.address = out.address;
					st.port = out.port;
					send_data(sock, st);
				}
				acknowledgement_t ack;
				ack.sequence_numbers = malloc(1 * sizeof(unsigned int));
				ack.sequence_numbers[0] = packet.sequence_number;
				ack.sequence_numbers_count = 1;
				binary_stream_t ack_stream = encode_acknowledgement(ack, 1);
				st.buffer = ack_stream.buffer;
				st.buffer_length = ack_stream.size;
				st.address = out.address;
				st.port = out.port;
				send_data(sock, st);
				connection->last_sequence_number = packet.sequence_number + 1;
				int i;
				for (i = 0; i < packet.frame_count; ++i)
				{
					binary_stream_t stream;
    				stream.buffer = packet.frames[i].body;
    				stream.offset = 0;
    				stream.size = packet.frames[i].body_length;
					if (connection->has_connected == 0)
					{
						if ((stream.buffer[0] & 0xff) == ID_CONNECTION_REQUEST)
						{
							connection_request_t connection_request = decode_connection_request(&stream);
							connection_request_accepted_t connection_request_accepted;
							connection_request_accepted.client_address.hostname = out.address;
							connection_request_accepted.client_address.port = out.port;
							connection_request_accepted.client_address.version = 4;
							connection_request_accepted.request_timestamp = connection_request.request_timestamp;
							connection_request_accepted.accepted_timestamp = time(NULL);
							binary_stream_t result = encode_connection_request_accepted(connection_request_accepted);
							frame_t frame;
							frame.body = result.buffer;
							frame.body_length = result.size;
							frame.is_fragmented = 0;
							frame.reliability = 0;
							add_to_queue(frame, 1, connection, sock);
						}
						else if (stream.buffer[0] == ID_NEW_INCOMING_CONNECTION)
						{
							connection->has_connected = 1;
						}
					}
					if ((stream.buffer[0] & 0xff) == ID_CONNECTED_PING)
					{
						connected_ping_t connected_ping = decode_connected_ping(&stream);
						connected_pong_t connected_pong;
						connected_pong.client_timestamp = connected_ping.client_timestamp;
						binary_stream_t connected_pong_stream = encode_connected_pong(connected_pong);
						frame_t frame;
						frame.body = connected_pong_stream.buffer;
						frame.body_length = connected_pong_stream.size;
						frame.is_fragmented = 0;
						frame.reliability = 0;
						add_to_queue(frame, 1, connection, sock);
					}
					else if ((stream.buffer[0] & 0xff) == ID_DISCONNECTION_NOTIFICATION)
					{
						remove_connection(out.address, out.port);
						
						log_info(concate(connection->username, " left the server."));
					}
					else if ((stream.buffer[0] & 0xff) == 0x82)
					{
						++stream.offset;
						connection->username = get_string(&stream);
						binary_stream_t status_stream;
						status_stream.buffer = malloc(0);
    					status_stream.offset = 0;
    					status_stream.size = 0;
						put_unsigned_byte(0x83, &status_stream);
						put_unsigned_int_be(0, &status_stream);
						binary_stream_t start_stream;
						start_stream.buffer = malloc(0);
    					start_stream.offset = 0;
    					start_stream.size = 0;
						put_unsigned_byte(0x87, &start_stream);
						put_unsigned_int_be(0, &start_stream);
						put_unsigned_int_be(0, &start_stream);
						put_unsigned_int_be(1, &start_stream);
						put_unsigned_int_be(1, &start_stream);
						put_float_be(0 + 128, &start_stream);
						put_float_be(9 + 64, &start_stream);
						put_float_be(0 + 128, &start_stream);
						frame_t frame;
						frame.body = status_stream.buffer;
						frame.body_length = status_stream.size;
						frame.is_fragmented = 0;
						frame.reliability = 0;
						add_to_queue(frame, 1, connection, sock);
						frame.body = start_stream.buffer;
						frame.body_length = start_stream.size;
						frame.is_fragmented = 0;
						frame.reliability = 0;
						add_to_queue(frame, 1, connection, sock);
					}
					else if ((stream.buffer[0] & 0xff) == 0x84)
					{
						log_info(concate(connection->username, " joined the server!"));
					}
				}
			}
		}
	}
}