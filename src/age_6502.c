#include "age_6502.h"
#include <stdio.h>
#include <stdlib.h>

struct age_6502* age_6502_new(void* system, uint8_t (*read_mem)(void* sys, uint16_t pos), void (*write_mem)(void* sys, uint16_t pos, uint8_t data))
{
	struct age_6502* cpu = (struct age_6502*)malloc(sizeof(struct age_6502));
	cpu->system = system;
	cpu->read_mem = read_mem;
	cpu->write_mem = write_mem;
	return cpu;
}

void age_6502_destroy(struct age_6502* cpu)
{
	free(cpu);
}

void age_6502_init(struct age_6502* cpu)
{
	cpu->cycle = 0;
	cpu->sp = 0xff;
	cpu->pc = cpu->read_mem(cpu->system, 0xfffc) | ((uint16_t)cpu->read_mem(cpu->system, 0xfffd) << 8);
}

uint16_t age_6502_read16(struct age_6502* cpu, uint16_t addr)
{
	return cpu->read_mem(cpu->system, addr) | ((uint16_t)cpu->read_mem(cpu->system, addr+1) << 8);
}

uint8_t age_6502_get_p(struct age_6502* cpu)
{
	return (cpu->f_c) | (cpu->f_z << 1) | (cpu->f_i << 2) | (cpu->f_d << 3) | (cpu->f_b << 4) | (1 << 5) | (cpu->f_v << 6) | (cpu->f_n << 7);
}

void age_6502_set_p(struct age_6502* cpu, uint8_t val)
{
	cpu->f_c = (val & 0x01);
	cpu->f_z = (val & 0x02) >> 1;
	cpu->f_i = (val & 0x04) >> 2;
	cpu->f_d = (val & 0x08) >> 3;
	cpu->f_b = (val & 0x10) >> 4;
	cpu->f_v = (val & 0x40) >> 6;
	cpu->f_n = (val & 0x80) >> 7;
}

#define D_ACC  printf(" A\n");
#define D_IMM  printf(" #%02X\n", cpu->read_mem(cpu->system, cpu->pc+1));
#define D_ZP   printf(" $%02X\n", cpu->read_mem(cpu->system, cpu->pc+1));
#define D_ZPX  printf(" $%02X,X\n", cpu->read_mem(cpu->system, cpu->pc+1));
#define D_ZPY  printf(" $%02X,Y\n", cpu->read_mem(cpu->system, cpu->pc+1));
#define D_REL  printf(" $%04X\n", (int8_t)cpu->read_mem(cpu->system, cpu->pc+1)+(int)cpu->pc+2);
#define D_ABS  printf(" $%04X\n", age_6502_read16(cpu, cpu->pc+1));
#define D_ABSX printf(" $%04X,X\n", age_6502_read16(cpu, cpu->pc+1));
#define D_ABSY printf(" $%04X,Y\n", age_6502_read16(cpu, cpu->pc+1));
#define D_INDX printf(" ($%02X,X)\n", cpu->read_mem(cpu->system, cpu->pc+1));
#define D_INDY printf(" ($%02X),Y\n", cpu->read_mem(cpu->system, cpu->pc+1));
void age_6502_print_next_instruction(struct age_6502* cpu)
{
	printf("%04x:", cpu->pc);
	for(int i = 0; i < 4; ++i)
		printf("%02x:", cpu->read_mem(cpu->system, cpu->pc+i));
	printf("a=%02x:x=%02x:y=%02x:sp=%02x:p=%02x:  ", cpu->a, cpu->x, cpu->y, cpu->sp, age_6502_get_p(cpu));
	switch(cpu->read_mem(cpu->system, cpu->pc))
	{
	case 0x69: printf("ADC"); D_IMM  break;
	case 0x65: printf("ADC"); D_ZP   break;
	case 0x75: printf("ADC"); D_ZPX  break;
	case 0x6D: printf("ADC"); D_ABS  break;
	case 0x7D: printf("ADC"); D_ABSX break;
	case 0x79: printf("ADC"); D_ABSY break;
	case 0x61: printf("ADC"); D_INDX break;
	case 0x71: printf("ADC"); D_INDY break;

	case 0x29: printf("AND"); D_IMM  break;
	case 0x25: printf("AND"); D_ZP   break;
	case 0x35: printf("AND"); D_ZPX  break;
	case 0x2D: printf("AND"); D_ABS  break;
	case 0x3D: printf("AND"); D_ABSX break;
	case 0x39: printf("AND"); D_ABSY break;
	case 0x21: printf("AND"); D_INDX break;
	case 0x31: printf("AND"); D_INDY break;

	case 0x0A: printf("ASL"); D_ACC  break;
	case 0x06: printf("ASL"); D_ZP   break;
	case 0x16: printf("ASL"); D_ZPX  break;
	case 0x0E: printf("ASL"); D_ABS  break;
	case 0x1E: printf("ASL"); D_ABSX break;

	case 0x90: printf("BCC"); D_REL  break;
	case 0xB0: printf("BCS"); D_REL  break;
	case 0xF0: printf("BEQ"); D_REL  break;

	case 0x24: printf("BIT"); D_ZP   break;
	case 0x2C: printf("BIT"); D_ABS  break;
		
	case 0x30: printf("BMI"); D_REL  break;
	case 0xD0: printf("BNE"); D_REL  break;
	case 0x10: printf("BPL"); D_REL  break;

	case 0x00: printf("BRK\n");      break;

	case 0x50: printf("BVC"); D_REL  break;
	case 0x70: printf("BVS"); D_REL  break;

	case 0x18: printf("CLC\n");      break;
	case 0xD8: printf("CLD\n");      break;
	case 0x58: printf("CLI\n");      break;
	case 0xB8: printf("CLV\n");      break;

	case 0xC9: printf("CMP"); D_IMM  break;
	case 0xC5: printf("CMP"); D_ZP   break;
	case 0xD5: printf("CMP"); D_ZPX  break;
	case 0xCD: printf("CMP"); D_ABS  break;
	case 0xDD: printf("CMP"); D_ABSX break;
	case 0xD9: printf("CMP"); D_ABSY break;
	case 0xC1: printf("CMP"); D_INDX break;
	case 0xD1: printf("CMP"); D_INDY break;

	case 0xE0: printf("CPX"); D_IMM  break;
	case 0xE4: printf("CPX"); D_ZP   break;
	case 0xEC: printf("CPX"); D_ABS  break;
		
	case 0xC0: printf("CPY"); D_IMM  break;
	case 0xC4: printf("CPY"); D_ZP   break;
	case 0xCC: printf("CPY"); D_ABS  break;
		
	case 0xC6: printf("DEC"); D_ZP   break;
	case 0xD6: printf("DEC"); D_ZPX  break;
	case 0xCE: printf("DEC"); D_ABS  break;
	case 0xDE: printf("DEC"); D_ABSX break;

	case 0xCA: printf("DEX\n");      break;
	case 0x88: printf("DEY\n");      break;

	case 0x49: printf("EOR"); D_IMM  break;
	case 0x45: printf("EOR"); D_ZP   break;
	case 0x55: printf("EOR"); D_ZPX  break;
	case 0x4D: printf("EOR"); D_ABS  break;
	case 0x5D: printf("EOR"); D_ABSX break;
	case 0x59: printf("EOR"); D_ABSY break;
	case 0x41: printf("EOR"); D_INDX break;
	case 0x51: printf("EOR"); D_INDY break;

	case 0xE6: printf("INC"); D_ZP   break;
	case 0xF6: printf("INC"); D_ZPX  break;
	case 0xEE: printf("INC"); D_ABS  break;
	case 0xFE: printf("INC"); D_ABSX break;

	case 0xE8: printf("INX\n");      break;
	case 0xC8: printf("INY\n");      break;

	case 0x4C: printf("JMP"); D_ABS  break;
	case 0x6C: printf("JMP ($%04x)\n", age_6502_read16(cpu, cpu->pc+1)); break;

	case 0x20: printf("JSR"); D_ABS  break;

	case 0xA9: printf("LDA"); D_IMM  break;
	case 0xA5: printf("LDA"); D_ZP   break;
	case 0xB5: printf("LDA"); D_ZPX  break;
	case 0xAD: printf("LDA"); D_ABS  break;
	case 0xBD: printf("LDA"); D_ABSX break;
	case 0xB9: printf("LDA"); D_ABSY break;
	case 0xA1: printf("LDA"); D_INDX break;
	case 0xB1: printf("LDA"); D_INDY break;

	case 0xA2: printf("LDX"); D_IMM  break;
	case 0xA6: printf("LDX"); D_ZP   break;
	case 0xB6: printf("LDX"); D_ZPY  break;
	case 0xAE: printf("LDX"); D_ABS  break;
	case 0xBE: printf("LDX"); D_ABSY break;

	case 0xA0: printf("LDY"); D_IMM  break;
	case 0xA4: printf("LDY"); D_ZP   break;
	case 0xB4: printf("LDY"); D_ZPX  break;
	case 0xAC: printf("LDY"); D_ABS  break;
	case 0xBC: printf("LDY"); D_ABSX break;

	case 0x4A: printf("LSR"); D_ACC  break;
	case 0x46: printf("LSR"); D_ZP   break;
	case 0x56: printf("LSR"); D_ZPX  break;
	case 0x4E: printf("LSR"); D_ABS  break;
	case 0x5E: printf("LSR"); D_ABSX break;

	case 0xEA: printf("NOP\n");      break;

	case 0x09: printf("ORA"); D_IMM  break;
	case 0x05: printf("ORA"); D_ZP   break;
	case 0x15: printf("ORA"); D_ZPX  break;
	case 0x0D: printf("ORA"); D_ABS  break;
	case 0x1D: printf("ORA"); D_ABSX break;
	case 0x19: printf("ORA"); D_ABSY break;
	case 0x01: printf("ORA"); D_INDX break;
	case 0x11: printf("ORA"); D_INDY break;

	case 0x48: printf("PHA\n");      break;
	case 0x08: printf("PHP\n");      break;
	case 0x68: printf("PLA\n");      break;
	case 0x28: printf("PLP\n");      break;

	case 0x2A: printf("ROL"); D_ACC  break;
	case 0x26: printf("ROL"); D_ZP   break;
	case 0x36: printf("ROL"); D_ZPX  break;
	case 0x2E: printf("ROL"); D_ABS  break;
	case 0x3E: printf("ROL"); D_ABSX break;

	case 0x6A: printf("ROR"); D_ACC  break;
	case 0x66: printf("ROR"); D_ZP   break;
	case 0x76: printf("ROR"); D_ZPX  break;
	case 0x6E: printf("ROR"); D_ABS  break;
	case 0x7E: printf("ROR"); D_ABSX break;

	case 0x40: printf("RTI\n");      break;
	case 0x60: printf("RTS\n");      break;
	
	case 0xE9: printf("SBC"); D_IMM  break;
	case 0xE5: printf("SBC"); D_ZP   break;
	case 0xF5: printf("SBC"); D_ZPX  break;
	case 0xED: printf("SBC"); D_ABS  break;
	case 0xFD: printf("SBC"); D_ABSX break;
	case 0xF9: printf("SBC"); D_ABSY break;
	case 0xE1: printf("SBC"); D_INDX break;
	case 0xF1: printf("SBC"); D_INDY break;

	case 0x38: printf("SEC\n");      break;
	case 0xF8: printf("SED\n");      break;
	case 0x78: printf("SEI\n");      break;

	case 0x85: printf("STA"); D_ZP   break;
	case 0x95: printf("STA"); D_ZPX  break;
	case 0x8D: printf("STA"); D_ABS  break;
	case 0x9D: printf("STA"); D_ABSX break;
	case 0x99: printf("STA"); D_ABSY break;
	case 0x81: printf("STA"); D_INDX break;
	case 0x91: printf("STA"); D_INDY break;

	case 0x86: printf("STX"); D_ZP   break;
	case 0x96: printf("STX"); D_ZPY  break;
	case 0x8E: printf("STX"); D_ABS  break;

	case 0x84: printf("STY"); D_ZP   break;
	case 0x94: printf("STY"); D_ZPX  break;
	case 0x8C: printf("STY"); D_ABS  break;

	case 0xAA: printf("TAX\n");      break;
	case 0xA8: printf("TAY\n");      break;
	case 0xBA: printf("TSX\n");      break;
	case 0x8A: printf("TXA\n");      break;
	case 0x9A: printf("TXS\n");      break;
	case 0x98: printf("TYA\n");      break;

		//illegals:

	case 0x04: case 0x14: case 0x34: case 0x44: case 0x54: case 0x64: case 0x74:
	case 0x80: case 0x82: case 0x89: case 0xC2: case 0xD4: case 0xE2: case 0xF4:
		printf("DOP (illegal double NOP)\n"); break;

	default:
		printf("UNKNOWN OPCODE 0x%02X\n", cpu->read_mem(cpu, cpu->pc));
	}	
}

void age_6502_print_page(struct age_6502* cpu, uint16_t page)
{
	for(int l = 0; l < 4; ++l)
	{
		for(int i = 0; i < 64; ++i)
		{
			printf("%02X ", cpu->read_mem(cpu->system, page + l*64 + i));
		}
		printf("\n");
	}
}

uint8_t age_6502_fetch_next(struct age_6502* cpu)
{
	return cpu->read_mem(cpu->system, cpu->pc++);
}


uint16_t age_6502_fetch_next16(struct age_6502* cpu)
{
	cpu->pc += 2;
	return age_6502_read16(cpu, cpu->pc-2);
}

void age_6502_stack_push(struct age_6502* cpu, uint8_t val)
{
	cpu->write_mem(cpu->system, cpu->sp-- + 0x100, val);
}

void age_6502_stack_push16(struct age_6502* cpu, uint16_t val)
{
	cpu->write_mem(cpu->system, cpu->sp-- + 0x100, val>>8);
	cpu->write_mem(cpu->system, cpu->sp-- + 0x100, val&0xFF);
}

uint8_t age_6502_stack_pop(struct age_6502* cpu)
{
	return cpu->read_mem(cpu->system, ++cpu->sp);
}

uint16_t age_6502_stack_pop16(struct age_6502* cpu)
{
	cpu->sp += 2;
	return cpu->read_mem(cpu->system, cpu->sp-1+0x100) | ((uint16_t)cpu->read_mem(cpu->system, cpu->sp-0+0x100) << 8);
}


#define SET_N cpu->f_n = (res8&0x80) >> 7
#define SET_V cpu->f_v = (((cpu->a^val)&0x80) == 0) && (((cpu->a^(res8))&0x80) != 0)
#define SET_Z cpu->f_z = res8 == 0
#define SET_C cpu->f_c = res > 0xff

#define IMM    cpu->cycle += 2; val = age_6502_fetch_next(cpu);
#define ZP     cpu->cycle += 3; addr = age_6502_fetch_next(cpu); val = cpu->read_mem(cpu->system, addr);
#define ZPX    cpu->cycle += 4; addr = (age_6502_fetch_next(cpu)+cpu->x)&0xFF; val = cpu->read_mem(cpu->system, addr);
#define ZPY    cpu->cycle += 4; addr = (age_6502_fetch_next(cpu)+cpu->y)&0xFF; val = cpu->read_mem(cpu->system, addr);
#define ABS    cpu->cycle += 4; addr = age_6502_fetch_next16(cpu); val = cpu->read_mem(cpu->system, addr);
#define ABSX   cpu->cycle += 5; addr = age_6502_fetch_next16(cpu)+cpu->x; val = cpu->read_mem(cpu->system, addr);
#define ABSY   cpu->cycle += 5; addr = age_6502_fetch_next16(cpu)+cpu->y; val = cpu->read_mem(cpu->system, addr);
#define ABSX_B taddr = age_6502_fetch_next16(cpu);	  \
	addr = taddr + cpu->x; \
	cpu->cycle += 4 + (((taddr%0x100)+cpu->x) >> 8); /*grab lower byte of address, add register, then check if byte overflows*/ \
	val = cpu->read_mem(cpu->system, addr);

#define ABSY_B taddr = age_6502_fetch_next16(cpu);	  \
	addr = taddr + cpu->y; \
	cpu->cycle += 4 + (((taddr%0x100)+cpu->y) >> 8); \
	val = cpu->read_mem(cpu->system, addr);

//todo: zero page wraparound?
#define INDX   cpu->cycle += 6; addr = age_6502_read16(cpu, age_6502_fetch_next(cpu)+cpu->x); val = cpu->read_mem(cpu->system, addr);
#define INDY   cpu->cycle += 6; addr = age_6502_read16(cpu, age_6502_fetch_next(cpu))+cpu->y; val = cpu->read_mem(cpu->system, addr);
#define INDY_B taddr = age_6502_read16(cpu, age_6502_fetch_next(cpu));  \
	addr = taddr + cpu->y; \
	cpu->cycle += 5 + (((taddr%0x100)+cpu->y) >> 8); \
	val = cpu->read_mem(cpu->system, addr);

#define BRANCH(A) if(A){res=(int8_t)age_6502_fetch_next(cpu) + cpu->pc; cpu->cycle += 3 + ((res>>8) != (cpu->pc>>8)); cpu->pc = res;} else {cpu->cycle += 2; cpu->pc++;}

void age_6502_exec(struct age_6502* cpu)
{
	uint8_t val, res8;
	uint16_t res, taddr, addr;
	int16_t ress;
	switch(age_6502_fetch_next(cpu))
	{
		//ADC
	case 0x69: IMM    goto adc;
	case 0x65: ZP     goto adc;
	case 0x75: ZPX    goto adc;
	case 0x6D: ABS    goto adc;
	case 0x7D: ABSX_B goto adc;
	case 0x79: ABSY_B goto adc;
	case 0x61: INDX   goto adc;
	case 0x71: INDY_B //goto adc
	adc:
		res = cpu->a + val + cpu->f_c;
		res8 = res%0x100;
		SET_N;SET_V;SET_Z;SET_C;
		cpu->a = res8;
		break;
		
		//AND
	case 0x29: IMM    goto and;
	case 0x25: ZP     goto and;
	case 0x35: ZPX    goto and;
	case 0x2D: ABS    goto and;
	case 0x3D: ABSX_B goto and;
	case 0x39: ABSX_B goto and;
	case 0x21: INDX   goto and;
	case 0x31: INDY_B //goto and
	and:
		res8 = cpu->a & val;
		SET_N;SET_Z;
		cpu->a = res8;
		break;
		
		//ASL
	case 0x0A: //ACC
		cpu->f_c = (cpu->a&80)>>7;
		cpu->a <<= 1;
		cpu->f_z = cpu->a == 0;
		cpu->f_n = (cpu->a&80)>>7;
		cpu->cycle += 2;
		break;
	case 0x06: ZP     goto asl;
	case 0x16: ZPX    goto asl;
    case 0x0E: ABS    goto asl;
	case 0x1E: ABSX   //goto asl
	asl:
		cpu->cycle += 2;
		res = val << 1;
		res8 = res%0x100;
		cpu->write_mem(cpu->system, addr, res8);
		SET_C;SET_N;SET_Z;
		break;
		
		//BCC
	case 0x90: BRANCH(!cpu->f_c) break;
		
		//BCS
	case 0xB0: BRANCH(cpu->f_c) break;
		
		//BEQ
	case 0xF0: BRANCH(cpu->f_z) break;

		//BIT
	case 0x24: ZP  goto bit;
	case 0x2C: ABS //goto bit
	bit:
		res8 = val & cpu->a;
		cpu->f_z = res8 == 0;
		cpu->f_n = val >> 7;
		cpu->f_v = (val&0x40) >> 6;
		break;

		//BMI
	case 0x30: BRANCH(cpu->f_n) break;

		//BNE
	case 0xD0: BRANCH(!cpu->f_z) break;

		//BPL
	case 0x10: BRANCH(!cpu->f_n) break;

		//BRK
	case 0x00:
		cpu->cycle += 7;
		age_6502_stack_push16(cpu, cpu->pc);
		age_6502_stack_push(cpu, age_6502_get_p(cpu));
		cpu->pc = age_6502_read16(cpu, 0xFFFE);
		cpu->f_b = 1;
		break;

		//BVC
	case 0x50: BRANCH(!cpu->f_v) break;

		//BVS
	case 0x70: BRANCH(cpu->f_v) break;

		//CLC
	case 0x18: cpu->cycle += 2; cpu->f_c = 0; break;

		//CLD
	case 0xD8: cpu->cycle += 2; cpu->f_d = 0; break;

		//CLI
	case 0x58: cpu->cycle += 2; cpu->f_i = 0; break;

		//CLV
	case 0xB8: cpu->cycle += 2; cpu->f_v = 0; break;

		//CMP
	case 0xC9: IMM    goto cmp;
	case 0xC5: ZP     goto cmp;
	case 0xD5: ZPX    goto cmp;
	case 0xCD: ABS    goto cmp;
	case 0xDD: ABSX_B goto cmp;
	case 0xD9: ABSY_B goto cmp;
	case 0xC1: INDX   goto cmp;
	case 0xD1: INDY_B //goto cmp
	cmp:
		cpu->f_c = cpu->a >= val;
		cpu->f_z = cpu->a == val;
		cpu->f_n = (cpu->a - val) >> 7;
		break;

		//CPX
	case 0xE0: IMM goto cpx;
	case 0xE4: ZP  goto cpx;
	case 0xEC: ABS //goto cpx
	cpx:
		cpu->f_c = cpu->x >= val;
		cpu->f_z = cpu->x == val;
		cpu->f_n = (cpu->x - val) >> 7;
		break;

		//CPY
	case 0xC0: IMM goto cpy;
	case 0xC4: ZP  goto cpy;
	case 0xCC: ABS //goto cpx
	cpy:
		cpu->f_c = cpu->y >= val;
		cpu->f_z = cpu->y == val;
		cpu->f_n = (cpu->y - val) >> 7;
		break;

		//DEC
	case 0xC6: ZP   goto dec;
	case 0xD6: ZPX  goto dec;
	case 0xCE: ABS  goto dec;
	case 0xDE: ABSX //goto dec
	dec:
		cpu->cycle += 2;
		res8 = val - 1;
		cpu->f_z = res8 == 0;
		cpu->f_n = res8 >> 7;
		cpu->write_mem(cpu->system, addr, res8);
		break;

		//DEX
	case 0xCA:
		cpu->cycle += 2;
		cpu->x--;
		cpu->f_z = cpu->x == 0;
		cpu->f_n = cpu->x >> 7;
		break;

		//DEY
	case 0x88:
		cpu->cycle += 2;
		cpu->y--;
		cpu->f_z = cpu->y == 0;
		cpu->f_n = cpu->y >> 7;
		break;

		//EOR
	case 0x49: IMM    goto eor;
	case 0x45: ZP     goto eor;
	case 0x55: ZPX    goto eor;
	case 0x4D: ABS    goto eor;
	case 0x5D: ABSX_B goto eor;
	case 0x59: ABSY_B goto eor;
	case 0x41: INDX   goto eor;
	case 0x51: INDY_B //goto eor
	eor:
		cpu->a ^= val;
		cpu->f_z = cpu->a == 0;
		cpu->f_n = cpu->a >> 7;
		break;

		//INC
	case 0xE6: ZP   goto inc;
	case 0xF6: ZPX  goto inc;
	case 0xEE: ABS  goto inc;
	case 0xFE: ABSX //goto inc
	inc:
		cpu->cycle += 2;
		res8 = val + 1;
		cpu->f_z = res8 == 0;
		cpu->f_n = res8 >> 7;
		cpu->write_mem(cpu->system, addr, res8);
		break;

		//INX
	case 0xE8:
		cpu->cycle += 2;
		cpu->x++;
		cpu->f_z = cpu->x == 0;
		cpu->f_n = cpu->x >> 7;
		break;

		//INY
	case 0xC8:
		cpu->cycle += 2;
		cpu->y++;
		cpu->f_z = cpu->y == 0;
		cpu->f_n = cpu->y >> 7;
		break;

		//JMP
	case 0x4C: //absolute
		cpu->cycle += 3;
		cpu->pc = age_6502_fetch_next16(cpu);
		break;
	case 0x6C: //indirect
		cpu->cycle += 5;
		addr = age_6502_fetch_next16(cpu);
		//bug in 6502, if address crosses page boundary, read from beginning of first page, instead of second page (todo: diverge on other variations)
		cpu->pc = cpu->read_mem(cpu->system, addr) | (cpu->read_mem(cpu->system, (addr&0xFF00)|(((addr&0xFF)+1)%0x100)) << 8);
		break;

		//JSR
	case 0x20:
		cpu->cycle += 6;
		addr = age_6502_fetch_next16(cpu);
		age_6502_stack_push16(cpu, cpu->pc - 1);
		cpu->pc = addr;
		break;

		//LDA
	case 0xA9: IMM    goto lda;
	case 0xA5: ZP     goto lda;
	case 0xB5: ZPX    goto lda;
	case 0xAD: ABS    goto lda;
	case 0xBD: ABSX_B goto lda;
	case 0xB9: ABSY_B goto lda;
	case 0xA1: INDX   goto lda;
	case 0xB1: INDY_B //goto lda
	lda:
		cpu->a = val;
		cpu->f_z = cpu->a == 0;
		cpu->f_n = cpu->a >> 7;
		break;

		//LDX
	case 0xA2: IMM    goto ldx;
	case 0xA6: ZP     goto ldx;
	case 0xB6: ZPY    goto ldx;
	case 0xAE: ABS    goto ldx;
	case 0xBE: ABSY_B //goto ldx
	ldx:
		cpu->x = val;
		cpu->f_z = cpu->x == 0;
		cpu->f_n = cpu->x >> 7;
		break;

		//LDY
	case 0xA0: IMM    goto ldy;
	case 0xA4: ZP     goto ldy;
	case 0xB4: ZPX    goto ldy;
	case 0xAC: ABS    goto ldy;
	case 0xBC: ABSX_B //goto ldy
	ldy:
		cpu->y = val;
		cpu->f_z = cpu->y == 0;
		cpu->f_n = cpu->y >> 7;
		break;

		//LSR
	case 0x4A:
		cpu->cycle += 2;
		cpu->f_c = cpu->a & 0x01;
		cpu->a >>= 1;
		cpu->f_z = cpu->a == 0;
		cpu->f_n = cpu->a >> 7;
		break;
	case 0x46: ZP   goto lsr;
	case 0x56: ZPX  goto lsr;
	case 0x4E: ABS  goto lsr;
	case 0x5E: ABSX //goto lsr
	lsr:
		cpu->cycle += 2;
		cpu->f_c = val & 0x01;
		res8 = val >> 1;
		cpu->f_z = res8 == 0;
		cpu->f_n = res8 >> 7;
		cpu->write_mem(cpu->system, addr, res8);
		break;

		//NOP
	case 0xEA: cpu->cycle += 2; break;

		//ORA
	case 0x09: IMM    goto ora;
	case 0x05: ZP     goto ora;
	case 0x15: ZPX    goto ora;
	case 0x0D: ABS    goto ora;
	case 0x1D: ABSX_B goto ora;
	case 0x19: ABSY_B goto ora;
	case 0x01: INDX   goto ora;
	case 0x11: INDY_B //goto ora
	ora:
		cpu->a |= val;
		cpu->f_z = cpu->a == 0;
		cpu->f_n = cpu->a >> 7;
		break;

		//PHA
	case 0x48:
		cpu->cycle += 3;
		age_6502_stack_push(cpu, cpu->a);
		break;

		//PHP
	case 0x08:
		cpu->cycle += 3;
		age_6502_stack_push(cpu, age_6502_get_p(cpu));
		break;

		//PLA
	case 0x68:
		cpu->cycle += 4;
		cpu->a = age_6502_stack_pop(cpu);
		cpu->f_z = cpu->a == 0;
		cpu->f_n = cpu->a >> 7;
		break;

		//PLP
	case 0x28:
		age_6502_set_p(cpu, age_6502_stack_pop(cpu));
		break;

		//ROL
	case 0x2A:
		cpu->cycle += 2;
		val = (cpu->a&0x80)>>7;
		cpu->a = (cpu->a<<1) | cpu->f_c;
		cpu->f_c = val;
		cpu->f_z = cpu->a == 0;
		cpu->f_n = cpu->a >> 7;
		break;
	case 0x26: ZP   goto rol;
	case 0x36: ZPX  goto rol;
	case 0x2E: ABS  goto rol;
	case 0x3E: ABSX //goto rol
	rol:
		cpu->cycle += 2;
		res8 = (cpu->a<<1) | cpu->f_c;
		cpu->f_c = (val&0x80)>>7;
		cpu->f_z = cpu->a == 0;
		cpu->f_n = cpu->a >> 7;
		cpu->a = res8;
		break;

		//ROR
	case 0x6A:
		cpu->cycle += 2;
		val = cpu->a&0x01;
		cpu->a = (cpu->a>>1) | (cpu->f_c<<7);
		cpu->f_c = val;
		cpu->f_z = cpu->a == 0;
		cpu->f_n = cpu->a >> 7;
		break;
	case 0x66: ZP   goto ror;
	case 0x76: ZPX  goto ror;
	case 0x6E: ABS  goto ror;
	case 0x7E: ABSX //goto ror
	ror:
		cpu->cycle += 2;
		res8 = (cpu->a>>1) | (cpu->f_c<<7);
		cpu->f_c = val&0x01;
		cpu->f_z = cpu->a == 0;
		cpu->f_n = cpu->a >> 7;
		cpu->a = res8;
		break;
		
		//RTI
	case 0x40:
		cpu->cycle += 6;
		age_6502_set_p(cpu, age_6502_stack_pop(cpu));
		cpu->pc = age_6502_stack_pop16(cpu);
		break;

		//RTS
	case 0x60:
		cpu->cycle += 6;
		cpu->pc = age_6502_stack_pop16(cpu) + 1;
		break;

		//SBC
	case 0xE9: IMM    goto sbc;
	case 0xE5: ZP     goto sbc;
	case 0xF5: ZPX    goto sbc;
	case 0xED: ABS    goto sbc;
	case 0xFD: ABSX_B goto sbc;
	case 0xF9: ABSY_B goto sbc;
	case 0xE1: INDX   goto sbc;
	case 0xF1: INDY_B //goto sbc
	sbc:
		ress = cpu->a - val - !cpu->f_c;
		cpu->f_v = (ress > 127 || ress < -128);
		cpu->f_c = ress >= 0;
		cpu->a = ress & 0xFF;
		cpu->f_z = cpu->a == 0;
		cpu->f_n = cpu->a >> 7;
		break;

		//SEC
	case 0x38:
		cpu->cycle += 2;
		cpu->f_c = 1;
		break;

		//SED
	case 0xF8:
		cpu->cycle += 2;
		cpu->f_d = 1;
		break;

		//SEI
	case 0x78:
		cpu->cycle += 2;
		cpu->f_i = 1;
		break;

		//STA
	case 0x85: ZP   goto sta;
	case 0x95: ZPX  goto sta;
	case 0x8D: ABS  goto sta;
	case 0x9D: ABSX goto sta;
	case 0x99: ABSY goto sta;
	case 0x81: INDX goto sta;
	case 0x91: INDY //goto sta
	sta:
		cpu->write_mem(cpu->system, addr, cpu->a);
		break;

		//STX
	case 0x86: ZP  goto stx;
	case 0x96: ZPY goto stx;
	case 0x8E: ABS //goto stx
	stx:
		cpu->write_mem(cpu->system, addr, cpu->x);
		break;

		//STY
	case 0x84: ZP  goto sty;
	case 0x94: ZPX goto sty;
	case 0x8C: ABS //goto sty
	sty:
		cpu->write_mem(cpu->system, addr, cpu->y);
		break;

		//TAX
	case 0xAA:
		cpu->cycle += 2;
		cpu->x = cpu->a;
		cpu->f_z = cpu->x == 0;
		cpu->f_n = cpu->x >> 7;
		break;

		//TAY
	case 0xA8:
		cpu->cycle += 2;
		cpu->y = cpu->a;
		cpu->f_z = cpu->y == 0;
		cpu->f_n = cpu->y >> 7;
		break;

		//TSX
	case 0xBA:
		cpu->cycle += 2;
		cpu->x = cpu->sp;
		cpu->f_z = cpu->x == 0;
		cpu->f_n = cpu->x >> 7;
		break;
		
		//TXA
	case 0x8A:
		cpu->cycle += 2;
		cpu->a = cpu->x;
		cpu->f_z = cpu->a == 0;
		cpu->f_n = cpu->a >> 7;
		break;

		//TXS
	case 0x9A:
		cpu->cycle += 2;
		cpu->sp = cpu->x;
		break;

		//TYA
	case 0x98:
		cpu->cycle += 2;
		cpu->a = cpu->y;
		cpu->f_z = cpu->a == 0;
		cpu->f_n = cpu->a >> 7;
		break;

		//----illegals----
		
	
		//DOP (double nop)
	case 0x80: case 0x82: case 0x89: case 0xC2: case 0xE2:
		cpu->cycle += 2;
		cpu->pc++;
		break;
	case 0x04: case 0x44: case 0x64:
		cpu->cycle += 3;
		cpu->pc++;
		break;
	case 0x14: case 0x34: case 0x54: case 0x74: case 0xD4: case 0xF4:
		cpu->cycle += 4;
		cpu->pc++;
		break;

	


	}
}

void age_6502_print_status(struct age_6502* cpu)
{
	printf("pc=%#06x sp=%#04x a=%#04x x=%#04x y=%#04x", cpu->pc, cpu->sp, cpu->a, cpu->x, cpu->y);
}
