#pragma once
// #define DEBUGMODE
// #define FULLSCREEN

constexpr bool UNCAPPED = true; 
constexpr int FPS = 120;
constexpr int MIL_PER_FRAME = 1000 / FPS; // 1 second = 1000 ms // How much do we expect each frame to last

constexpr int SCREEN_WIDTH = 1680;
constexpr int SCREEN_HEIGHT = 850;

constexpr int VIEWPORT_WIDTH = 960;
constexpr int VIEWPORT_HEIGHT = 540;

constexpr int SCREEN_LOGICAL_WIDTH = 640;
constexpr int SCREEN_LOGICAL_HEIGHT = 360;

const float CAMERA_SHIFT_INCREMENT_WIDTH = SCREEN_WIDTH;
const float CAMERA_SHIFT_INCREMENT_HEIGHT = SCREEN_HEIGHT;