-- premake5.lua (ENGINE REPO)

newoption {
  trigger = "host",
  value = "PATH",
  description = "Path to the EXE that loads Engine.dll (for debugging)"
}

workspace "OpenGL-Game-Engine"
  location "Build"
  configurations { "Debug", "Release" }
  platforms { "x64" }
  architecture "x86_64"
  startproject "OpenGL-Game-Engine"

include "OpenGL-Game-Engine.lua"

-- If a host EXE is provided, run it when debugging the Engine
if _OPTIONS["host"] then
  local exe = _OPTIONS["host"]
  for _, prj in ipairs(premake.api.scope.current.solution.projects) do
    if prj.name == "Engine" then
      prj.debugcommand = exe
      prj.debugdir = path.getdirectory(exe)
    end
  end
end
