#include "cpu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void loadRom(Cpu *cpu, const char *filename) {
  FILE *f = fopen(filename, "rb");

  if (f == NULL) {
    fprintf(stderr, "Failed to load ROM file %s \n", filename);
  }

  long i;

  fseek(f, 0L, SEEK_END);
  long size = ftell(f);
  fseek(f, 0, SEEK_SET);

  char buffer[size];
  fread(buffer, 1, size, f);

  for (i = 0; i < size; i++) {
    cpu->memory[START_ADDRESS + i] = buffer[i];
  }

  fclose(f);
}

void resetCpuMemory(Cpu *cpu) {
  size_t i;

  for (i = START_ADDRESS; i < MEMORY_SIZE; i++) {
    cpu->memory[i] = 0;
  }

  for (i = 0; i < VIDEO_WIDTH * VIDEO_HEIGHT; i++) {
    cpu->video[i] = 0;
  }

  for (i = 0; i < REGISTER_SIZE; i++) {
    cpu->regs[i] = 0;
  }

  for (i = 0; i < STACK_SIZE; i++) {
    cpu->stack[i] = 0;
  }

  cpu->index = 0;
  cpu->sp = 0;
  cpu->pc = START_ADDRESS;
}

void loadFont(Cpu *cpu) {
  size_t i;
  for (i = 0; i < FONTSET_SIZE; i++) {
    cpu->memory[FONT_START_ADDRESS + i] = FONTSET[i];
  }
}

void cpuCycle(Cpu *cpu) {
  cpu->opcode = (cpu->memory[cpu->pc] << 8) | cpu->memory[cpu->pc + 1];
  cpu->pc += 2;

  switch (cpu->opcode & 0xF000) {

  case 0x0000:
    switch (cpu->opcode & 0x00FF) {
    case 0x00E0:
      op_00e0(cpu);
      break;
    case 0x00ee:
      op_00ee(cpu);
      break;
    }
    break;

  case 0x1000:
    op_1nnn(cpu);
    break;
  case 0x2000:
    op_2nnn(cpu);
    break;
  case 0x3000:
    op_3xkk(cpu);
    break;
  case 0x4000:
    op_4xkk(cpu);
    break;
  case 0x5000:
    op_5xy0(cpu);
    break;
  case 0x6000:
    op_6xkk(cpu);
    break;
  case 0x7000:
    op_7xkk(cpu);
    break;

  case 0x8000:
    switch (cpu->opcode & 0x000F) {
    case 0x0000:
      op_8xy0(cpu);
      break;
    case 0x0001:
      op_8xy1(cpu);
      break;
    case 0x0002:
      op_8xy2(cpu);
      break;
    case 0x0003:
      op_8xy3(cpu);
      break;
    case 0x0004:
      op_8xy4(cpu);
      break;
    case 0x0005:
      op_8xy5(cpu);
      break;
    case 0x0006:
      op_8xy6(cpu);
      break;
    case 0x0007:
      op_8xy7(cpu);
      break;
    case 0x000E:
      op_8xye(cpu);
      break;
    default:
      printf("Opcode: 0x%X not implemented\n", cpu->opcode);
    }
    break;

  case 0x9000:
    op_9xy0(cpu);
    break;
  case 0xA000:
    op_annn(cpu);
    break;
  case 0xB000:
    op_bnnn(cpu);
    break;
  case 0xC000:
    op_cxkk(cpu);
    break;
  case 0xD000:
    op_dxyn(cpu);
    break;

  case 0xE000:
    switch (cpu->opcode & 0x00FF) {
    case 0x009E:
      op_ex9e(cpu);
      break;
    case 0x00A1:
      op_exa1(cpu);
      break;
    }
    break;

  case 0xF000:
    switch (cpu->opcode & 0x00FF) {
    case 0x0007:
      op_fx07(cpu);
      break;
    case 0x000A:
      op_fx0a(cpu);
      break;
    case 0x0015:
      op_fx15(cpu);
      break;
    case 0x0018:
      op_fx18(cpu);
      break;
    case 0x001E:
      op_fx1e(cpu);
      break;
    case 0x0029:
      op_fx29(cpu);
      break;
    case 0x0033:
      op_fx33(cpu);
      break;
    case 0x0055:
      op_fx55(cpu);
      break;
    case 0x0065:
      op_fx65(cpu);
      break;
    }
    break;

  default:
    printf("Unknown opcode 0x%X\n", cpu->opcode);
  }

  if (cpu->delayTimer > 0)
    --cpu->delayTimer;

  if (cpu->soundTimer > 0)
    --cpu->soundTimer;
}

// Instructions

void op_00e0(Cpu *cpu) { memset(cpu->video, 0, sizeof(cpu->video)); }
void op_00ee(Cpu *cpu) {
  cpu->sp--;
  cpu->pc = cpu->stack[cpu->sp];
}

void op_1nnn(Cpu *cpu) {
  u16 addr = cpu->opcode & 0x0FFFu;
  cpu->pc = addr;
}

void op_2nnn(Cpu *cpu) {
  u16 addr = cpu->opcode & 0x0FFFu;
  cpu->stack[cpu->sp] = cpu->pc;
  cpu->sp++;
  cpu->pc = addr;
}

void op_3xkk(Cpu *cpu) {
  u8 Vx = (cpu->opcode & 0x0F00u) >> 8u;
  u8 byte = cpu->opcode & 0x00FFu;
  if (cpu->regs[Vx] == byte)
    cpu->pc += 2;
}

void op_4xkk(Cpu *cpu) {
  u8 Vx = (cpu->opcode & 0x0F00u) >> 8u;
  u8 byte = cpu->opcode & 0x00FFu;
  if (cpu->regs[Vx] != byte)
    cpu->pc += 2;
}

void op_5xy0(Cpu *cpu) {
  u8 Vx = (cpu->opcode & 0x0F00u) >> 8u;
  u8 Vy = (cpu->opcode & 0x00F0u) >> 4u;

  if (cpu->regs[Vx] == cpu->regs[Vy])
    cpu->pc += 2;
}

void op_6xkk(Cpu *cpu) {
  u8 Vx = (cpu->opcode & 0x0F00u) >> 8u;
  u8 byte = cpu->opcode & 0x00FFu;

  cpu->regs[Vx] = byte;
}

void op_7xkk(Cpu *cpu) {
  u8 Vx = (cpu->opcode & 0x0F00u) >> 8u;
  u8 byte = cpu->opcode & 0x00FFu;

  cpu->regs[Vx] += byte;
}

void op_8xy0(Cpu *cpu) {
  u8 Vx = (cpu->opcode & 0x0F00u) >> 8u;
  u8 Vy = (cpu->opcode & 0x00F0u) >> 4u;
  cpu->regs[Vx] = cpu->regs[Vy];
}

void op_8xy1(Cpu *cpu) {
  u8 Vx = (cpu->opcode & 0x0F00u) >> 8u;
  u8 Vy = (cpu->opcode & 0x00F0u) >> 4u;
  cpu->regs[Vx] |= cpu->regs[Vy];
}

void op_8xy2(Cpu *cpu) {
  u8 Vx = (cpu->opcode & 0x0F00u) >> 8u;
  u8 Vy = (cpu->opcode & 0x00F0u) >> 4u;
  cpu->regs[Vx] &= cpu->regs[Vy];
}

void op_8xy3(Cpu *cpu) {
  u8 Vx = (cpu->opcode & 0x0F00u) >> 8u;
  u8 Vy = (cpu->opcode & 0x00F0u) >> 4u;
  cpu->regs[Vx] ^= cpu->regs[Vy];
}

void op_8xy4(Cpu *cpu) {
  u8 Vx = (cpu->opcode & 0x0F00u) >> 8u;
  u8 Vy = (cpu->opcode & 0x00F0u) >> 4u;
  u16 sum = cpu->regs[Vx] + cpu->regs[Vy];
  cpu->regs[0xF] = sum > 255U ? 1 : 0;
  cpu->regs[Vx] = sum & 0xFFu;
}

void op_8xy5(Cpu *cpu) {
  u8 Vx = (cpu->opcode & 0x0F00u) >> 8u;
  u8 Vy = (cpu->opcode & 0x00F0u) >> 4u;
  cpu->regs[0xF] = cpu->regs[Vx] > cpu->regs[Vy] ? 1 : 0;
  cpu->regs[Vx] -= cpu->regs[Vy];
}

void op_8xy6(Cpu *cpu) {
  u8 Vx = (cpu->opcode & 0x0F00u) >> 8u;
  u8 Vy = (cpu->opcode & 0x00F0u) >> 4u;
  cpu->regs[0xF] = cpu->regs[Vy] & 0x1;
  cpu->regs[Vx] = cpu->regs[Vy] >> 1;
}

void op_8xy7(Cpu *cpu) {
  u8 Vx = (cpu->opcode & 0x0F00u) >> 8u;
  u8 Vy = (cpu->opcode & 0x00F0u) >> 4u;
  cpu->regs[0xF] = cpu->regs[Vy] > cpu->regs[Vx] ? 1 : 0;
  u8 sub = cpu->regs[Vy] - cpu->regs[Vx];
  cpu->regs[Vx] = sub;
}

void op_8xye(Cpu *cpu) {
  u8 Vx = (cpu->opcode & 0x0F00u) >> 8u;
  u8 Vy = (cpu->opcode & 0x00F0u) >> 4u;
  cpu->regs[0xF] = (cpu->regs[Vy] >> 7) & 0x1u;
  cpu->regs[Vx] = cpu->regs[Vy] << 1;
}

void op_9xy0(Cpu *cpu) {
  u8 Vx = (cpu->opcode & 0x0F00u) >> 8u;
  u8 Vy = (cpu->opcode & 0x00F0u) >> 4u;
  cpu->pc += cpu->regs[Vx] != cpu->regs[Vy] ? 2 : 0;
}

void op_annn(Cpu *cpu) {
  u16 addr = cpu->opcode & 0x0FFFu;
  cpu->index = addr;
}

void op_bnnn(Cpu *cpu) {
  u16 addr = cpu->opcode & 0x0FFFu;
  cpu->pc = addr + cpu->regs[0x0];
}

void op_cxkk(Cpu *cpu) {
  u8 Vx = (cpu->opcode & 0x0F00u) >> 8u;
  u8 byte = cpu->opcode & 0x00FFu;
  u8 x = rand() >> 7;
  cpu->regs[Vx] = (rand() % 256) & byte;
}

void op_dxyn(Cpu *cpu) {
  u8 Vx = (cpu->opcode & 0x0F00u) >> 8u;
  u8 Vy = (cpu->opcode & 0x00F0u) >> 4u;
  u8 height = cpu->opcode & 0x000Fu;

  u8 xPos = cpu->regs[Vx] % VIDEO_WIDTH;
  u8 yPos = cpu->regs[Vy] % VIDEO_HEIGHT;

  cpu->regs[0xF] = 0;
  int row, col;
  for (row = 0; row < height; row++) {
    u8 sprite = cpu->memory[cpu->index + row];
    for (col = 0; col < 8; col++) {
      u8 spritePixel = sprite & (0x80u >> col);

      if (spritePixel) {
        // Stack Overflow V
        u32 *screenPixel =
            &cpu->video[((yPos + row) % VIDEO_HEIGHT) * VIDEO_WIDTH +
                        ((xPos + col) % VIDEO_WIDTH)];
        if (*screenPixel == 0xFFFFFFFF) {
          cpu->regs[0xF] = 1;
        }

        *screenPixel ^= 0xFFFFFFFF;
      }
    }
  }
}

void op_ex9e(Cpu *cpu) {
  u8 Vx = (cpu->opcode & 0x0F00u) >> 8u;
  u8 key = cpu->regs[Vx];
  cpu->pc += cpu->keypad[key] ? 2 : 0;
}

void op_exa1(Cpu *cpu) {
  u8 Vx = (cpu->opcode & 0x0F00u) >> 8u;
  u8 key = cpu->regs[Vx];
  cpu->pc += !cpu->keypad[key] ? 2 : 0;
}

void op_fx07(Cpu *cpu) {
  u8 Vx = (cpu->opcode & 0x0F00u) >> 8u;
  cpu->regs[Vx] = cpu->delayTimer;
}

void op_fx0a(Cpu *cpu) {
  u8 Vx = (cpu->opcode & 0x0F00u) >> 8u;
  u8 found = 0;

  size_t i;
  for (i = 0; i < KEYS; i++) {
    if (cpu->keypad[i]) {
      cpu->regs[Vx] = i;
      found = 1;
    }
  }

  if (!found)
    cpu->pc -= 2;
}

void op_fx15(Cpu *cpu) {
  u8 Vx = (cpu->opcode & 0x0F00u) >> 8u;
  cpu->delayTimer = cpu->regs[Vx];
}

void op_fx18(Cpu *cpu) {
  u8 Vx = (cpu->opcode & 0x0F00u) >> 8u;
  cpu->soundTimer = cpu->regs[Vx];
}

void op_fx1e(Cpu *cpu) {
  u8 Vx = (cpu->opcode & 0x0F00u) >> 8u;
  cpu->index += cpu->regs[Vx];
}

void op_fx29(Cpu *cpu) {
  u8 Vx = (cpu->opcode & 0x0F00u) >> 8u;
  u8 val = cpu->regs[Vx];
  cpu->index = FONT_START_ADDRESS + (5 * val);
}

void op_fx33(Cpu *cpu) {
  u8 Vx = (cpu->opcode & 0x0F00u) >> 8u;
  u8 val = cpu->regs[Vx];
  cpu->memory[cpu->index + 2] = val % 10;
  val /= 10;
  cpu->memory[cpu->index + 1] = val % 10;
  val /= 10;
  cpu->memory[cpu->index] = val % 10;
}

void op_fx55(Cpu *cpu) {
  u8 Vx = (cpu->opcode & 0x0F00u) >> 8u;
  size_t i;
  for (i = 0; i <= Vx; i++) {
    cpu->memory[cpu->index + i] = cpu->regs[i];
  }
}

void op_fx65(Cpu *cpu) {
  u8 Vx = (cpu->opcode & 0x0F00u) >> 8u;
  size_t i;
  for (i = 0; i <= Vx; i++) {
    cpu->regs[i] = cpu->memory[cpu->index + i];
  }
}
