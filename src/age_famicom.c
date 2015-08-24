#include "age_famicom.h"
#include "error.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#pragma pack(push, 1)
// http://wiki.nesdev.com/w/index.php/INES
struct ines_header
{
	uint32_t magic; //should be 0x1a53454e "NES "
	uint8_t  prg_rom_size; //in 16KB units
	uint8_t  chr_rom_size; //in 8KB units (0 means chr ram)
	uint8_t  flags_6;
	uint8_t  flags_7;
	uint8_t  prg_ram_size; //in 8KB units (0 means 8KB too...?)
	uint8_t  flags_9;
	uint8_t  flags_10;
	uint8_t  zero[5]; //should be zero
};
#pragma pack(pop)

uint8_t age_famicom_cpu_read_mem(void* sys, uint16_t pos)
{
	struct age_famicom* fami = (struct age_famicom*)sys;
	if(pos < 0x2000) //internal ram
		return fami->work_ram[pos%0x800];
	if(pos < 0x4000) //ppu registers
		return fami->ppu_reg_ram[pos%0x8];
	if(pos < 0x4020) //apu registers
		return fami->apu_reg_ram[pos-0x4000];
	//cartridge space
	if(pos >= 0x8000) //rom in mapper 0
		return fami->prg_rom[pos-0x8000];
	if(pos >= 0x6000) //ram in mapper 0
		return fami->prg_ram[pos-0x6000];

	return 0;
}

void age_famicom_cpu_write_mem(void* sys, uint16_t pos, uint8_t val)
{
	struct age_famicom* fami = (struct age_famicom*)sys;
	if(pos < 0x2000) //internal ram
		fami->work_ram[pos%0x800] = val;
	else if(pos < 0x4000) //ppu registers
		fami->ppu_reg_ram[pos%0x8] = val;
	else if(pos < 0x4020) //apu registers
		fami->apu_reg_ram[pos-0x4000] = val;
	else if(pos < 0x8000) //prg ram in mapper 0
		fami->prg_ram[pos-0x6000] = val;

	//cartridge space
}

struct age_famicom* age_famicom_new()
{
	struct age_famicom* famicom = (struct age_famicom*)malloc(sizeof(struct age_famicom));
	famicom->work_ram    = (uint8_t*)malloc(2048);
	famicom->ppu_reg_ram = (uint8_t*)malloc(8);
	famicom->ppu_reg_ram[2] = 0x80;
	famicom->apu_reg_ram = (uint8_t*)malloc(0x20);
	famicom->prg_rom     = 0;
	famicom->prg_ram     = (uint8_t*)malloc(0x2000);
	famicom->cpu = age_6502_new(famicom, age_famicom_cpu_read_mem, age_famicom_cpu_write_mem);
	return famicom;
}

ag_error age_famicom_load_rom(struct age_famicom* fami, char* path)
{
	FILE* fil = fopen(path, "rb");
	if(fil == 0)
		return AG_ERR_INVALID_PATH;
	struct ines_header header;
	fread(&header, sizeof(struct ines_header), 1, fil);
	if(header.magic != 0x1a53454e)
		return AG_ERR_INVALID_FILE;
	printf("prg_rom_size=%i chr_rom_size=%i\n", header.prg_rom_size, header.chr_rom_size);
	uint8_t mapper = ((header.flags_6 & 0xf0) >> 4) | (header.flags_7 & 0xf0);
	if(mapper != 0)
		return AG_ERR_NOT_YET_SUPPORTED;
	fami->prg_rom = (uint8_t*)malloc(0x8000);
	fread(fami->prg_rom, 1, 0x4000*header.prg_rom_size, fil);
	if(header.prg_rom_size == 1) //if only one prg ram bank in mapper 0 used, mirror to upper bank
		memcpy(fami->prg_rom+0x4000, fami->prg_rom, 0x4000);
	age_6502_init(fami->cpu);
	return AG_ERR_SUCCESS;
}

void age_famicom_destroy(struct age_famicom* fami)
{
	age_6502_destroy(fami->cpu);
	free(fami->work_ram);
	free(fami->ppu_reg_ram);
	free(fami->apu_reg_ram);
	free(fami->prg_rom);
	free(fami);
}
