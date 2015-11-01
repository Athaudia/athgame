#pragma once

struct ag_tcp_con
{
	int sock;
};

struct ag_tcp_con_buffered
{
	struct ag_tcp_con* con;
	int buffer_size;
	char* buffer;
	int buffer_i;
	char* delim;
	char* return_line;
};

struct ag_tcp_con* ag_tcp_con__new(char* addr, char* port);
void ag_tcp_con__destroy(struct ag_tcp_con* con);

void ag_tcp_con__send(struct ag_tcp_con* con, void* data, int len);
void ag_tcp_con__send_str(struct ag_tcp_con* con, char* str);
int ag_tcp_con__recv(struct ag_tcp_con* con, void* buf, int buf_len);

struct ag_tcp_con_buffered* ag_tcp_con_buffered__new(char* addr, char* port, int buffer_size, char* delim);
void ag_tcp_con_buffered__destroy(struct ag_tcp_con_buffered* con);

char* ag_tcp_con_buffered__recv(struct ag_tcp_con_buffered* con);
void ag_tcp_con_buffered__send_str(struct ag_tcp_con_buffered* con, char* str);
