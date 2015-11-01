#include "age_ehbasic.h"
#include "error.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <stropts.h>
//#include <ncurses.h>

uint8_t age_ehbasic_cpu_read_mem(void* s, uint16_t pos)
{
	struct age_ehbasic* sys = (struct age_ehbasic*)s;
	if(pos == 0xF004)
	{
		//todo: make independent of ncurses
		/*int ch = getch();
		if(ch == ERR)
			{
			return 0;
			}
		else
			return ch;
		*/
		return 0;
	}
	else
		return sys->ram[pos];
}

void age_ehbasic_cpu_write_mem(void* s, uint16_t pos, uint8_t val)
{
	struct age_ehbasic* sys = (struct age_ehbasic*)s;
	if(pos == 0xF001)
	{
		printf("%c", val);
		fflush(stdout);
	}
	else
		sys->ram[pos] = val;
}

struct age_ehbasic* age_ehbasic_new()
{
	//initscr();
	//nocbreak();
	//nodelay(stdscr, TRUE);
	struct age_ehbasic* sys = (struct age_ehbasic*)malloc(sizeof(struct age_ehbasic));
	sys->ram = (uint8_t*)malloc(0xffff);
	sys->cpu = age_6502_new(sys, age_ehbasic_cpu_read_mem, age_ehbasic_cpu_write_mem);
	sys->str = 0;
	sys->strpos = 0;
	return sys;
}

ag_error age_ehbasic_load_rom(struct age_ehbasic* sys, char* path)
{
	FILE* fil = fopen(path, "rb");
	if(fil == 0)
		return AG_ERR_INVALID_PATH;
	fread(sys->ram+0xC000, 1, 0x4000, fil);
	age_6502_init(sys->cpu);
	return AG_ERR_SUCCESS;
}

void age_ehbasic_destroy(struct age_ehbasic* sys)
{
	age_6502_destroy(sys->cpu);
	free(sys->ram);
	free(sys);
}
