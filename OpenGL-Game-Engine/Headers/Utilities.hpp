#pragma once
#include "precomp.h"

#include "Input.hpp"
#include "Line.hpp"
#include "Common.hpp"
#include "Creature.hpp"
#include "Camera.hpp"
#include "ResourceManager.hpp"
#include "FrameBuffer.hpp"
#include "UserInterface.hpp"
#include "IKChain.hpp"
#include "Logger.hpp"
#include "BoxCollider.hpp"
#include "CollisionSolver.hpp"
#include "Emitter.hpp"

static inline std::string OpenFileDialog()
{
    char filename[MAX_PATH] = "";

    OPENFILENAMEA ofn = { 0 };
    ofn.lStructSize = sizeof(OPENFILENAMEA);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFile = filename;
    ofn.nMaxFile = sizeof(filename);
    ofn.lpstrFilter = "Image Files\0*.png;*.jpg;*.jpeg\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameA(&ofn))
        return std::string(filename);
    return ""; // Cancelled or failed
}