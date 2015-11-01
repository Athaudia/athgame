#include "net.h"
#include "platform.h"
#ifdef PLATFORM_LINUX
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#elif PLATFORM_WIN32
#include <ws2tcpip.h>
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

struct ag_tcp_con* ag_tcp_con__new(char* addr, char* port)
{
	struct addrinfo hints;
	struct addrinfo* res;
	struct ag_tcp_con* con = (struct ag_tcp_con*)malloc(sizeof(struct ag_tcp_con));
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	getaddrinfo(addr, port, &hints, &res);
	con->sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	connect(con->sock, res->ai_addr, res->ai_addrlen);

	int f = 1;
	setsockopt(con->sock, IPPROTO_TCP, TCP_NODELAY, (char*)&f, sizeof(int));
#ifdef PLATFORM_WIN32
	u_long mode = 1;
	ioctlsocket(con->sock, FIONBIO, &mode);
#else
	fcntl(con->sock, F_SETFL, O_NONBLOCK);
#endif
	return con;
}

void ag_tcp_con__destroy(struct ag_tcp_con* con)
{
#ifdef PLATFORM_WIN32
	closesocket(con->sock);
#else
	close(con->sock);
#endif
	free(con);
}

void ag_tcp_con__send(struct ag_tcp_con* con, void* data, int len)
{
	int sent = send(con->sock, data, len, 0);
	if(sent < len)
		printf("error, sent less than planned!\n");
}

void ag_tcp_con__send_str(struct ag_tcp_con* con, char* str)
{
	ag_tcp_con__send(con, str, strlen(str));
}

int ag_tcp_con__recv(struct ag_tcp_con* con, void* buf, int buf_len)
{
	int num = recv(con->sock, buf, buf_len, 0);
	if(num == -1)
		return 0;
	return num;
}

struct ag_tcp_con_buffered* ag_tcp_con_buffered__new(char* addr, char* port, int buffer_size, char* delim)
{
	struct ag_tcp_con_buffered* con = (struct ag_tcp_con_buffered*)malloc(sizeof(struct ag_tcp_con_buffered));
	con->con = ag_tcp_con__new(addr, port);
	con->delim = strdup(delim);
	con->buffer_size = buffer_size;
	con->buffer = (char*)malloc(buffer_size+1);
	con->buffer[0] = 0;
	con->return_line = 0;
	con->buffer_i = 0;
	return con;
}

void ag_tcp_con_buffered__destroy(struct ag_tcp_con_buffered* con)
{
	ag_tcp_con__destroy(con->con);
	free(con->buffer);
	free(con->return_line);
	free(con);
}

char* ag_tcp_con_buffered__recv(struct ag_tcp_con_buffered* con)
{
	free(con->return_line);
	con->return_line = 0;

	if(con->buffer_i < con->buffer_size)
		con->buffer_i += ag_tcp_con__recv(con->con, con->buffer+con->buffer_i, con->buffer_size-con->buffer_i);
	con->buffer[con->buffer_i] = 0;
	//printf(con->buffer);
	char* sstr = strstr(con->buffer, con->delim);
	if(sstr)
	{
		*sstr = 0;
		con->return_line = strdup(con->buffer);
		int line_len = sstr+strlen(con->delim)-con->buffer;
		memmove(con->buffer, sstr+strlen(con->delim), con->buffer_size-line_len);
		con->buffer_i -= line_len;
	}
	return con->return_line;
}
                            
void ag_tcp_con_buffered__send_str(struct ag_tcp_con_buffered* con, char* str)
{
	ag_tcp_con__send_str(con->con, str);
}
