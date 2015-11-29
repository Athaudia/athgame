#include "process.h"

#include <stdio.h>
#include <stdlib.h>

char* ag_system(char* cmd)
{
	FILE* p = popen(cmd, "r");
	int buf_size = 17;
	char* buf = (char*)malloc(sizeof(char)*buf_size);
	int write_pos = 0; //can't use straight up pointer, because memory position of buf may change because of realloc
	
	int rlen;
	while((rlen = fread(buf+write_pos, 1, 16, p)) > 0)
	{
		write_pos += rlen;
		buf_size += 16;
		buf = realloc(buf, sizeof(char)*buf_size);
	}
	buf[write_pos] = 0;
	pclose(p);
	return buf;
}
