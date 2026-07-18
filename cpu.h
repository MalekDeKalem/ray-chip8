

#ifndef __CPU_H__
#define __CPU_H__

#include <raylib.h>
#include <stdint.h>

#define REGISTER_SIZE 16
#define MEMORY_SIZE 4096
#define STACK_SIZE 16
#define KEYS 16

#define START_ADDRESS 0x200
#define FONT_START_ADDRESS 0x50
#define FONTSET_SIZE 80
#define VIDEO_WIDTH 64
#define VIDEO_HEIGHT 32

typedef uint16_t u16;
typedef uint8_t u8;
typedef uint32_t u32;

typedef struct {
  u8 regs[REGISTER_SIZE];
  u8 memory[MEMORY_SIZE];
  u16 stack[STACK_SIZE];
  u32 video[VIDEO_WIDTH * VIDEO_HEIGHT];
  u8 delayTimer;
  u8 soundTimer;
  u8 keypad[KEYS];
  u16 index;
  u8 sp;
  u16 pc;
  u16 opcode;
} Cpu;

/*
 * |1| |2| |3| |4|
 * |q| |w| |e| |r|
 * |a| |s| |d| |f|
 * |z| |x| |c| |v|
 */
static const int KEY_CODES_QWERTY[KEYS] = {
    KEY_ONE, KEY_TWO, KEY_THREE, KEY_FOUR, KEY_Q, KEY_W, KEY_E, KEY_R,
    KEY_A,   KEY_S,   KEY_D,     KEY_F,    KEY_Z, KEY_X, KEY_C, KEY_V};

/*
 * |1| |2| |3| |4|
 * |q| |w| |e| |r|
 * |a| |s| |d| |f|
 * |y| |x| |c| |v|
 */
static const int KEY_CODES_QWERTZ[KEYS] = {
    KEY_ONE, KEY_TWO, KEY_THREE, KEY_FOUR, KEY_Q, KEY_W, KEY_E, KEY_R,
    KEY_A,   KEY_S,   KEY_D,     KEY_F,    KEY_Y, KEY_X, KEY_C, KEY_V};

static const u8 FONTSET[FONTSET_SIZE] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, 0x20, 0x60, 0x20, 0x20, 0x70, 0xF0, 0x10,
    0xF0, 0x80, 0xF0, 0xF0, 0x10, 0xF0, 0x10, 0xF0, 0x90, 0x90, 0xF0, 0x10,
    0x10, 0xF0, 0x80, 0xF0, 0x10, 0xF0, 0xF0, 0x80, 0xF0, 0x90, 0xF0, 0xF0,
    0x10, 0x20, 0x40, 0x40, 0xF0, 0x90, 0xF0, 0x90, 0xF0, 0xF0, 0x90, 0xF0,
    0x10, 0x10, 0xF0, 0x90, 0xF0, 0x90, 0x90, 0xE0, 0x90, 0xE0, 0x90, 0xE0,
    0xF0, 0x80, 0x80, 0x80, 0xF0, 0xE0, 0x90, 0x90, 0x90, 0xE0, 0xF0, 0x80,
    0xF0, 0x80, 0xF0, 0xF0, 0x80, 0xF0, 0x80, 0x80};

void loadRom(Cpu *cpu, const char *filename);
void loadFont(Cpu *cpu);
void cpuCycle(Cpu *cpu);
void resetCpuMemory(Cpu *cpu);

void op_00e0(Cpu *cpu);
void op_00ee(Cpu *cpu);
void op_1nnn(Cpu *cpu);
void op_2nnn(Cpu *cpu);
void op_3xkk(Cpu *cpu);
void op_4xkk(Cpu *cpu);
void op_5xy0(Cpu *cpu);
void op_6xkk(Cpu *cpu);
void op_7xkk(Cpu *cpu);
void op_8xy0(Cpu *cpu);
void op_8xy1(Cpu *cpu);
void op_8xy2(Cpu *cpu);
void op_8xy3(Cpu *cpu);
void op_8xy4(Cpu *cpu);
void op_8xy5(Cpu *cpu);
void op_8xy6(Cpu *cpu);
void op_8xy7(Cpu *cpu);
void op_8xye(Cpu *cpu);
void op_9xy0(Cpu *cpu);
void op_annn(Cpu *cpu);
void op_bnnn(Cpu *cpu);
void op_cxkk(Cpu *cpu);
void op_dxyn(Cpu *cpu);
void op_ex9e(Cpu *cpu);
void op_exa1(Cpu *cpu);
void op_fx07(Cpu *cpu);
void op_fx07(Cpu *cpu);
void op_fx0a(Cpu *cpu);
void op_fx15(Cpu *cpu);
void op_fx18(Cpu *cpu);
void op_fx1e(Cpu *cpu);
void op_fx29(Cpu *cpu);
void op_fx33(Cpu *cpu);
void op_fx55(Cpu *cpu);
void op_fx65(Cpu *cpu);
#endif // __CPU_H__
