#include "irc.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

struct ag_irc_msg* ag_irc_msg__new(char* prefix, char* command, char** parameters, int parameter_count)
{
	struct ag_irc_msg* msg = (struct ag_irc_msg*)malloc(sizeof(struct ag_irc_msg));
	if(prefix)
		msg->prefix = strdup(prefix);
	else
		msg->prefix = 0;

	msg->command = strdup(command);
	if(parameter_count > 0)
	{
		msg->parameters = (char**)malloc(sizeof(char*)*parameter_count);
		for(int i = 0; i < parameter_count; ++i)
			msg->parameters[i] = strdup(parameters[i]);
	}
	else
		msg->parameters = 0;

	msg->parameter_count = parameter_count;
	return msg;
}

struct ag_irc_msg* ag_irc_msg__new_from_string(char* cstr)
{
	struct ag_irc_msg* msg = (struct ag_irc_msg*)malloc(sizeof(struct ag_irc_msg));
	char* str = strdup(cstr);
	char* strp = str;
	char* init_str = str;
	//TODO: validate input
	if(*strp == ':')
	{
		strp = strstr(strp, " ");
		*strp = 0;
		msg->prefix = strdup(str+1);
		str = ++strp;
	}
	else
		msg->prefix = 0;

	strp = strstr(strp, " ");
	if(strp)
		*strp = 0;
	msg->command = strdup(str);
	if(strp)
		str = ++strp;
	else
		str = 0;
	
	msg->parameter_count = 0;
	msg->parameters = 0;
	bool last = false;
	if(strp)
		while((strp = strstr(strp, " ")))
		{
			msg->parameters = realloc(msg->parameters, sizeof(char*) * ++msg->parameter_count);
			if(*str == ':')
			{
				msg->parameters[msg->parameter_count-1] = strdup(str+1);
				last = true;
				break;
			}
			else
			{
				*strp = 0;
				msg->parameters[msg->parameter_count-1] = strdup(str);
				str = ++strp;
			}
			
		}

	if(!last && str)
	{
		msg->parameters = realloc(msg->parameters, sizeof(char*) * ++msg->parameter_count);
		msg->parameters[msg->parameter_count-1] = strdup(str+(*str == ':'?1:0));
	}
	
	
	free(init_str);
	return msg;
}

void ag_irc_msg__destroy(struct ag_irc_msg* msg)
{
	free(msg->prefix);
	free(msg->command);
	for(int i = 0; i < msg->parameter_count; ++i)
		free(msg->parameters[i]);
	free(msg->parameters);
	free(msg);
}


char* ag_irc_msg__to_string(struct ag_irc_msg* msg)
{
	char* str = (char*)malloc(sizeof(char)*513);
	char* strp = str;
	if(msg->prefix)
		strp += sprintf(strp, ":%s ", msg->prefix);
	strp += sprintf(strp, "%s", msg->command);
	if(msg->parameter_count > 0)
	{
		for(int i = 0; i < msg->parameter_count - 1; ++i)
			strp += sprintf(strp, " %s", msg->parameters[i]);

		strp += sprintf(strp, " :%s", msg->parameters[msg->parameter_count-1]);
	}
	strp += sprintf(strp, "\r\n");
	return str;
}

char* ag_irc_msg__debug_inspect(struct ag_irc_msg* msg)
{
	char* str = (char*)malloc(sizeof(char)*600);
	char* strp = str;
	if(msg->prefix)
		strp += sprintf(strp, "prefix: [[%s]] ", msg->prefix);
	strp += sprintf(strp, "command: [[%s]] ", msg->command);
	if(msg->parameter_count > 0)
	{
		strp += sprintf(strp, "parameters:");
		for(int i = 0; i < msg->parameter_count; ++i)
			strp += sprintf(strp, " [[%s]]", msg->parameters[i]);
	}
	return str;
}



//client

struct ag_irc_client* ag_irc_client__new(char* server, char* port, char* nick)
{
	struct ag_irc_client* client = (struct ag_irc_client*)malloc(sizeof(struct ag_irc_client));
	client->last_received = 0;
	client->con = ag_tcp_con_buffered__new(server, port, 512, "\r\n");
	ag_irc_client__send_nick(client, nick);
	ag_irc_client__send_user(client, nick);
	return client;
}

void ag_irc_client__destroy(struct ag_irc_client* client)
{
	ag_tcp_con_buffered__destroy(client->con);
	if(client->last_received)
		ag_irc_msg__destroy(client->last_received);
	free(client);
}

void ag_irc_client__send(struct ag_irc_client* client, char* prefix, char* command, char** parameters, int parameter_count)
{
	struct ag_irc_msg* msg = ag_irc_msg__new(prefix, command, parameters, parameter_count);
	char* msg_str = ag_irc_msg__to_string(msg);
	ag_tcp_con_buffered__send_str(client->con, msg_str);
	printf(">> %s", msg_str);
	free(msg_str);
	ag_irc_msg__destroy(msg);
}

struct ag_irc_msg* ag_irc_client__recv(struct ag_irc_client* client)
{
	if(client->last_received)
		ag_irc_msg__destroy(client->last_received);
	char* str = ag_tcp_con_buffered__recv(client->con);
	if(str)
	{
		printf("<< %s\n", str);
		client->last_received = ag_irc_msg__new_from_string(str);
	}
	else
		client->last_received = 0;
	if(client->last_received && strcmp(client->last_received->command, "PING") == 0)
	{
		ag_irc_client__send(client, 0, "PONG", client->last_received->parameters, client->last_received->parameter_count);
		//client->last_received = 0;
	}
	return client->last_received;
}

void ag_irc_client__send_nick(struct ag_irc_client* client, char* nick)
{
	ag_irc_client__send(client, 0, "NICK", (char*[]){nick}, 1);
}

void ag_irc_client__send_user(struct ag_irc_client* client, char* user)
{
	ag_irc_client__send(client, 0, "USER", (char*[]){user, user, user, user}, 4);
}
