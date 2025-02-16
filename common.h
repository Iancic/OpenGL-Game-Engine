#pragma once

// #define DEBUGMODE
// #define FULLSCREEN

constexpr bool UNCAPPED = true;
constexpr int FPS = 120;
constexpr int MIL_PER_FRAME = 1000 / FPS; // 1 second = 1000 ms // How much do we expect each frame to last

constexpr int SCREEN_WIDTH = 1920;
constexpr int SCREEN_HEIGHT = 1080;

constexpr int SCREEN_LOGICAL_WIDTH = 640;
constexpr int SCREEN_LOGICAL_HEIGHT = 360;

constexpr int SECTOR_WIDTH = 32;
constexpr int SECTOR_HEIGHT = 18;

const float CAMERA_SHIFT_INCREMENT_WIDTH = SCREEN_WIDTH;
const float CAMERA_SHIFT_INCREMENT_HEIGHT = SCREEN_HEIGHT;

// 3840×2160 (4K UHD)
// 2560×1440
// 5 // Last Stage // 1920×1080 (full HD)
// 4 // 1280×720 (HD)
// 3 // 640 x 360
// 2 // 320 x 180
// 1 // 160 x 90
// TO DO: When cellulla zooms out change logical scale by zooming out with lerp.