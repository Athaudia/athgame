#pragma once
#include "net.h"

struct ag_irc_msg
{
	char* prefix;
	char* command;
	char** parameters;
	int parameter_count;
};

struct ag_irc_client
{
	struct ag_tcp_con_buffered* con;
	struct ag_irc_msg* last_received;
};

struct ag_irc_msg* ag_irc_msg__new(char* prefix, char* command, char** parameters, int paramater_count);
struct ag_irc_msg* ag_irc_msg__new_from_string(char* str);
void ag_irc_msg__destroy(struct ag_irc_msg* msg);

char* ag_irc_msg__to_string(struct ag_irc_msg* msg);
char* ag_irc_msg__debug_inspect(struct ag_irc_msg* msg);

struct ag_irc_client* ag_irc_client__new(char* server, char* port, char* nick);
void ag_irc_client__destroy(struct ag_irc_client* client);
void ag_irc_client__send(struct ag_irc_client* client, char* prefix, char* command, char** parameters, int parameter_count);
struct ag_irc_msg* ag_irc_client__recv(struct ag_irc_client* client);

void ag_irc_client__send_nick(struct ag_irc_client* client, char* nick);
void ag_irc_client__send_user(struct ag_irc_client* client, char* user);

