#include "cpu.h"
#include "raylib.h"
#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MENU_SIZE 20
#define ROMS 5
#define FONT_SIZE 25

typedef enum {
  MENU,
  CONTROLS,
  EMUL,
} Screen;

typedef enum {
  QWERTZ,
  QWERTY,
} KeyLayout;

typedef struct {
  struct ROM {
    char *romName;
    int isSelected;
  } roms[MENU_SIZE];
  int pointer;
} MenuState;

typedef struct {
  MenuState menu;
  Screen currScreen;
  KeyLayout currLayout;
} EmulState;

void initMenuState(MenuState *menu) {
  struct dirent *de;
  DIR *dr = opendir("./roms/");
  if (dr == NULL) {
    printf("Could not open directory\n");
    exit(1);
  }
  size_t i = 0;
  size_t j = 0;
  while ((de = readdir(dr)) != NULL) {

    if (i < 2) {
      i++;
      continue;
    }

    menu->roms[j].romName = strdup(de->d_name);
    menu->roms[j].isSelected = 0;
    printf("Loaded %s into MenuState\n", de->d_name);
    i++;
    j++;
  }

  menu->roms[0].isSelected = 1;

  closedir(dr);
}

void drawMenuScreen(MenuState *menu) {
  size_t i;
  int posY = FONT_SIZE;
  for (i = 0; i < ROMS; i++) {
    if (menu->roms[i].romName != NULL) {
      if (menu->roms[i].isSelected) {
        DrawText(menu->roms[i].romName, 50, posY * (i + 1), FONT_SIZE, ORANGE);
      } else {
        DrawText(menu->roms[i].romName, 50, posY * (i + 1), FONT_SIZE,
                 RAYWHITE);
      }
    }
  }
}

void processInputMenu(MenuState *menu, Cpu *cpu, Screen *screen) {
  if (IsKeyPressed(KEY_DOWN)) {
    if (menu->pointer >= 0 && menu->pointer < ROMS - 1) {
      menu->roms[menu->pointer].isSelected = 0;
      menu->roms[++menu->pointer].isSelected = 1;
    }
  } else if (IsKeyPressed(KEY_UP)) {
    if (menu->pointer >= 1 && menu->pointer < ROMS) {
      menu->roms[menu->pointer].isSelected = 0;
      menu->roms[--menu->pointer].isSelected = 1;
    }
  } else if (IsKeyPressed(KEY_ENTER)) {
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "./roms/%s",
             menu->roms[menu->pointer].romName);

    resetCpuMemory(cpu);
    loadRom(cpu, buffer);
    menu->roms[menu->pointer].isSelected = 0;
    menu->roms[0].isSelected = 1;
    menu->pointer = 0;
    *screen = EMUL;
  }
}

void processInput(Cpu *cpu, EmulState *emul) {

  for (size_t i = 0; i < KEYS; i++) {
    switch (emul->currLayout) {
    case QWERTZ:
      if (IsKeyDown(KEY_CODES_QWERTZ[i])) {
        cpu->keypad[i] = 1;
      }
      break;
    case QWERTY:
      if (IsKeyDown(KEY_CODES_QWERTY[i])) {
        cpu->keypad[i] = 1;
      }
      break;
    default:
      printf("No such Keyboard layout\n");
      break;
    }
  }

  for (size_t i = 0; i < KEYS; i++) {
    switch (emul->currLayout) {
    case QWERTZ:
      if (IsKeyUp(KEY_CODES_QWERTZ[i])) {
        cpu->keypad[i] = 0;
      }
      break;
    case QWERTY:
      if (IsKeyUp(KEY_CODES_QWERTY[i])) {
        cpu->keypad[i] = 0;
      }
      break;
    default:
      printf("No such Keyboard layout\n");
      break;
    }
  }

  if (IsKeyPressed(KEY_M)) {
    emul->currScreen = MENU;
  }
}

void drawEmulScreen(Cpu *cpu, int scale) {
  size_t y, x;
  for (y = 0; y < VIDEO_HEIGHT; y++) {
    for (x = 0; x < VIDEO_WIDTH; x++) {
      if (cpu->video[y * VIDEO_WIDTH + x]) {
        DrawRectangle(x * scale, y * scale, scale, scale, RAYWHITE);
      }
    }
  }
}

int main(int argc, char **argv) {

  if (argc != 3) {
    printf("Program must have 3 arguments\n");
    exit(1);
  }

  printf("Starting the application\n");
  int scale = atoi(argv[1]);
  int delay = atoi(argv[2]);
  // char const *romName = argv[3];

  InitWindow(VIDEO_WIDTH * scale, VIDEO_HEIGHT * scale, "Chip8");

  Cpu cpu = {0};
  cpu.pc = START_ADDRESS;
  int cyclePerFrame = 10;

  loadFont(&cpu);
  // loadRom(&cpu);

  Screen currScreen = MENU;

  int currentFps = 60;
  int lastFrameTime = GetTime();

  MenuState menu = {0};
  initMenuState(&menu);
  KeyLayout currLayout = QWERTY;

  EmulState emul = (EmulState){
      .menu = menu, .currScreen = currScreen, .currLayout = currLayout};
  printf("Menu state %s\n", menu.roms[2].romName);

  SetTargetFPS(currentFps);

  while (!WindowShouldClose()) {

    switch (currScreen) {
    case EMUL:
      double currTime = GetTime();
      double deltaTime = currTime - lastFrameTime;
      processInput(&cpu, &emul);

      static double timerAccum = 0.0;
      timerAccum += deltaTime;

      while (timerAccum >= (1.0 / 60.0)) {
        if (cpu.delayTimer > 0)
          cpu.delayTimer--;
        if (cpu.soundTimer > 0)
          cpu.soundTimer--;
        timerAccum -= (1.0 / 60.0);
      }

      size_t i;
      for (i = 0; i < cyclePerFrame; i++) {
        cpuCycle(&cpu);
      }

      BeginDrawing();
      ClearBackground(BLACK);
      drawEmulScreen(&cpu, scale);
      EndDrawing();
      break;
    case CONTROLS:

      break;

    case MENU:

      processInputMenu(&menu, &cpu, &currScreen);

      BeginDrawing();
      ClearBackground(BLACK);
      drawMenuScreen(&menu);
      EndDrawing();
      break;
    }
  }

  CloseWindow();
  return 0;
}
