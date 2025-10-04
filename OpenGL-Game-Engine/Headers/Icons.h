#pragma once
#include "precomp.h"

// ===== ICON DEFINES =====
#define ICON_UPLOAD        u8"\ue800" // 59392
#define ICON_ARROWS_CW     u8"\ue801" // 59393
#define ICON_PLAY          u8"\ue802" // 59394
#define ICON_STOP          u8"\ue803" // 59395
#define ICON_PAUSE         u8"\ue804" // 59396
#define ICON_EJECT         u8"\ue805" // 59397
#define ICON_ZOOM_IN       u8"\ue806" // 59398
#define ICON_ZOOM_OUT      u8"\ue807" // 59399
#define ICON_FOLDER        u8"\ue808" // 59400
#define ICON_LIST          u8"\ue809" // 59401
#define ICON_DESKTOP       u8"\uf108" // 61704
#define ICON_TABLET        u8"\uf10a" // 61706
#define ICON_HELP_CIRCLED  u8"\ue80a" // 59402
#define ICON_HELP          u8"\uf128" // 61736
#define ICON_INFO_CIRCLED  u8"\ue80b" // 59403
#define ICON_INFO          u8"\uf129" // 61737
#define ICON_COG           u8"\ue80c" // 59404
#define ICON_WRENCH        u8"\ue80d" // 59405
#define ICON_SLIDERS       u8"\uf1de" // 61918
#define ICON_MENU          u8"\uf0c9" // 61641
#define ICON_WARNING       u8"\ue80e" // 59406
#define ICON_LIST_NESTED   u8"\ue80f" // 59407

// ===== ICON GLYPH RANGES =====
static const ImWchar ICON_RANGES[] = 
{
    0xE800, 0xE80F, // continuous block from 59392 to 59407
    0xF0B7, 0xF0B7, // ICON_MOVE
    0xF0C9, 0xF0C9, // ICON_MENU
    0xF108, 0xF108, // ICON_DESKTOP
    0xF10A, 0xF10A, // ICON_TABLET
    0xF128, 0xF129, // ICON_HELP & ICON_INFO
    0xF12A, 0xF12B, // ICON_CUBE & ICON_CUBES
    0xF1DE, 0xF1DE, // ICON_SLIDERS
    0,              // terminator
};

// ===== FONT LOADER =====
inline void LoadIconFont(ImGuiIO& io, const char* icons_ttf_path, float size_pixels = 16.0f)
{
    ImFontConfig config;
    config.MergeMode = true;      // merge into current font
    config.PixelSnapH = true;
    config.GlyphMinAdvanceX = size_pixels; // Optional: adjust spacing

    io.Fonts->AddFontFromFileTTF(icons_ttf_path, size_pixels, &config, ICON_RANGES);
}