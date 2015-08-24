#pragma once
#include "age_6502.h"
#include "error.h"

struct age_famicom
{
	struct age_6502* cpu;
	uint8_t* work_ram; //2KB
	uint8_t* ppu_reg_ram; //8 bytes
	uint8_t* apu_reg_ram; //0x20 bytes

	//cartridge
	uint8_t* prg_rom; //16 or 32 KB
	uint8_t* prg_ram;
};

struct age_famicom* age_famicom_new();
void age_famicom_destroy(struct age_famicom* fami);

//uint8_t age_famicom_cpu_read_mem(void* sys, uint16_t pos);
//void age_famicom_cpu_write_mem(void* sys, uint16_t pos, uint8_t val);

ag_error age_famicom_load_rom(struct age_famicom* fami, char* path);
