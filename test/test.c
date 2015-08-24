#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <athgame.h>
#include <age_famicom.h>



int main()
{
	struct age_famicom* fami = age_famicom_new();
	//	ag_error err = age_famicom_load_rom(fami, "/home/athaudia/Super Mario Bros. (Japan, USA).nes");
	//ag_error err = age_famicom_load_rom(fami, "/home/athaudia/instr_test-v4/rom_singles/01-basics.nes");
	ag_error err = age_famicom_load_rom(fami, "/home/athaudia/nestest.nes");
	printf("%s\n", ag_error_readable(err));
	printf("pc: %04X\n", fami->cpu->pc);
	fami->cpu->pc = 0xc000;
	//age_6502_print_status(fami->cpu);
	for(int i = 0; i < 1000000; ++i)
	{
		age_6502_print_page(fami->cpu, 0x100);
		age_6502_print_next_instruction(fami->cpu);
		age_6502_exec(fami->cpu);
		//getchar();
		//		if(fami->cpu->pc == 0xe86b) break;
	}

	age_6502_print_next_instruction(fami->cpu);
	printf("a: %04X\n", *fami->prg_ram);
	printf("b: %s\n", fami->prg_ram+6);
	age_famicom_destroy(fami);
	return 0;
}
