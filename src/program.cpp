#include "program.h"
#include <stdio.h>
#include "dynamic_libs/os_functions.h"
#include "dynamic_libs/vpad_functions.h"
#include "draw.h"
#include "level.h"

const int SCREEN_WIDTH = 854;
const int SCREEN_HEIGHT = 480;
const float DEG2RAD = 0.01745329251f;

u64 lastFrame = 0;
bool isPaused = false;
bool darkOverlay = false;
float score = 0;
GameState state = GameState::PreGame;
Level level;

void changeState(GameState s);

int start() {
    InitOSFunctionPointers();
    InitVPadFunctionPointers();

	//Call the Screen initilzation function.
	OSScreenInit();
	//Grab the buffer size for each screen (TV and gamepad)
	int buf0_size = OSScreenGetBufferSizeEx(0);
	// int buf1_size = OSScreenGetBufferSizeEx(1);
	//Set the buffer area.
	OSScreenSetBufferEx(0, (void*) 0xF4000000);
	OSScreenSetBufferEx(1, (void*) 0xF4000000 + buf0_size);

	int ii;
	for (ii = 0; ii < 2; ii++) {
		fillScreen(0x2C, 0x2C, 0x2C, 0xFF);
		flipBuffers();
	}

    OSScreenEnableEx(0, 1); // TV
    OSScreenEnableEx(1, 1); // Pad

	level.init();
	changeState(GameState::PreGame);
	level.spawnWave();

	int error;
	VPADData vpad_data;
	while (true) {
		u64 t = OSGetTick();
		u64 elapsed = t - lastFrame;
        float dt = elapsed / BUS_SPEED / 1000000000.0f; // t is in nanoseconds, convert to seconds
        if(isPaused) dt = 0;
        lastFrame = t;

        // Get the status of the gamepad and check button presses
		VPADRead(0, &vpad_data, 1, &error);
		u32 buttonDown = vpad_data.btns_d;
        if(buttonDown & BUTTON_HOME) break;
		if(buttonDown & BUTTON_PLUS && state == GameState::InGame) isPaused = !isPaused;
		if(buttonDown & BUTTON_MINUS) changeState(GameState::PreGame);
		if(buttonDown & BUTTON_A || buttonDown & BUTTON_B || buttonDown & BUTTON_X || buttonDown & BUTTON_Y) {
			if(state == GameState::InGame) {
				level.player->flip();
			} else {
				changeState(GameState::InGame);
			}
		}

		char scoreChars[64];
		snprintf(scoreChars, sizeof(scoreChars), "%08d", (int) score);

		fillScreen(0x2C, 0x2C, 0x2C, 0xFF);

		level.update(dt);
		level.render();

		// Render text
		switch(state) {
			case GameState::PreGame:
				drawString(25, 7, "F L U C T U S");
				drawString(22, 9, "Any button to start");
				break;
			case GameState::InGame:
				drawString(57, 0, scoreChars);
				break;
			case GameState::PostGame:
				drawString(28, 7, scoreChars);
				drawString(21, 9, "Any button to restart");
				break;
		}

		if(isPaused) {
			drawString(-2, 0, "Paused");
		} else {
			char fpsChars[64];
			snprintf(fpsChars, sizeof(fpsChars), "fps: %.1f", 1.0f / dt);
			drawString(-2, 0, fpsChars);
		}

		flipBuffers();
	}

	level.cleanup();

	//WARNING: DO NOT CHANGE THIS. YOU MUST CLEAR THE FRAMEBUFFERS AND IMMEDIATELY CALL EXIT FROM THIS FUNCTION. RETURNING TO LOADER CAUSES FREEZE.
	for(ii = 0; ii < 2 ;ii++) {
		fillScreen(0, 0, 0, 0);
		flipBuffers();
	}

	return 0;
}

void changeState(GameState s) {
	state = s;
	level.onStateChange(state);
	if(state == GameState::InGame) {
		score = 0;
	} else {
		isPaused = false;
	}
}