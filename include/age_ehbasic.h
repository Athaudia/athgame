#pragma once
#include "age_6502.h"
#include "error.h"

struct age_ehbasic
{
	struct age_6502* cpu;
	uint8_t* ram; //64KB
	char* str;
	int strpos;
};

struct age_ehbasic* age_ehbasic__new();
void age_ehbasic__destroy(struct age_ehbasic* sys);
ag_error age_ehbasic__load_rom(struct age_ehbasic* sys, char* path);
