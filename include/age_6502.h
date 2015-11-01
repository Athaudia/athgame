#pragma once

#include <stdint.h>

struct age_6502
{
	uint16_t pc; //program counter
	uint8_t  sp; //stack pointer
	uint8_t  a;  //accumulator
	uint8_t  x;  //index register x
	uint8_t  y;  //index register y
	//	uint8_t  p;  //processor status
	uint8_t f_n, f_v, f_b, f_d, f_i, f_z, f_c; //processorr status
	void* system; //parent system this instance is part of
	uint8_t (*read_mem)(void* sys, uint16_t pos);
	void (*write_mem)(void* sys, uint16_t pos, uint8_t data);
	int cycle;
};

struct age_6502* age_6502__new(void* system, uint8_t (*read_mem)(void* sys, uint16_t pos), void (*write_mem)(void* sys, uint16_t pos, uint8_t data));
void age_6502__destroy(struct age_6502* cpu);

void age_6502__init(struct age_6502* cpu); //can be called again to reset
void age_6502__print_status(struct age_6502* cpu);
void age_6502__print_next_instruction(struct age_6502* cpu);
void age_6502__print_page(struct age_6502* cpu, uint16_t page);
void age_6502__exec(struct age_6502* cpu);
