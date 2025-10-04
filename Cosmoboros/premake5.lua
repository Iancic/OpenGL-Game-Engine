workspace "MyGame"
  location "Build"                         
  startproject "Game"
  configurations { "Debug", "Release" }
  platforms { "x64" }
  architecture "x86_64"
  cppdialect "C++20"
  staticruntime "Off"
  characterset "Unicode"

newoption {
  trigger     = "engine-root",
  value       = "PATH",
  description = "Path to Engine repo root (contains Headers/, Libraries/, libraries/, Build/)"
}

-- Engine root (absolute)
local ENGINE = _OPTIONS["engine-root"] or "../OpenGL-Game-Engine"
ENGINE = path.getabsolute(ENGINE)
if not os.isdir(ENGINE) then
  error("Engine directory not found at: " .. ENGINE .. "\nUse --engine-root=PATH to specify location")
end

-- Engine outputs
local ENGINE_INCLUDE = ENGINE .. "/Headers"
local ENGINE_BIN     = ENGINE .. "/Build/bin/%{cfg.buildcfg}"
local ENGINE_LIB     = ENGINE .. "/Build/bin/%{cfg.buildcfg}"

-- Vendor roots
local VENDORS_BIG   = ENGINE .. "/Libraries"
local VENDORS_SMALL = ENGINE .. "/libraries"

-- Pick SDL3 vendor path
local SDL3 = VENDORS_BIG .. "/SDL3"

-- Include directories
local includeList = {
  "Headers",
  ENGINE_INCLUDE,

  -- Vendors (adjust to your tree)
  VENDORS_BIG .. "/yaml/include",
  SDL3 .. "/include",                    -- SDL3 headers
  VENDORS_BIG .. "/glm-master",
  VENDORS_BIG .. "/imgui-master",
  VENDORS_BIG .. "/entt/single_include",
  VENDORS_BIG .. "/sol",

  VENDORS_SMALL .. "/glad/include",
  VENDORS_SMALL .. "/stb",

  VENDORS_BIG,
  VENDORS_SMALL,
}
for i, p in ipairs(includeList) do
  includeList[i] = path.getabsolute(p)
end

-- YAML debug vs release name helper
local function yaml_lib_name(isDebug)
  local d = isDebug and "/Debug" or "/Release"
  local base = VENDORS_BIG .. "/yaml/lib" .. d
  if os.isfile(base .. "/yaml-cppd.lib") then
    return "yaml-cppd"
  else
    return "yaml-cpp"
  end
end

project "Game"
  kind "ConsoleApp"
  language "C++"
  targetname "Game"
  targetdir "%{wks.location}/bin/%{cfg.buildcfg}"
  objdir    "%{wks.location}/obj/%{prj.name}/%{cfg.buildcfg}"
  debugdir  "%{cfg.targetdir}"

  files {
    "Source/**.cpp", "Source/**.cc", "Source/**.c", "Source/**.cxx",
    "Headers/**.hpp","Headers/**.h", "Headers/**.inl",
  }

  vpaths {
    ["Header Files"] = { "Headers/**.h", "Headers/**.hpp", "Headers/**.inl" },
    ["Source Files"] = { "Source/**.cpp", "Source/**.cc", "Source/**.c", "Source/**.cxx" },
  }

  includedirs(includeList)

  filter "configurations:Debug"
    libdirs {
      ENGINE .. "/Build/bin/Debug",
      SDL3   .. "/lib",                   -- put SDL3.lib / SDL3.dll here
      VENDORS_BIG .. "/lua/lib",
      VENDORS_BIG .. "/yaml/lib/Debug",
    }
    links {
      "Engine",
      "SDL3",                             -- SDL3 (shared import lib)
      "lua54",
      yaml_lib_name(true),
    }

  filter "configurations:Release"
    libdirs {
      ENGINE .. "/Build/bin/Release",
      SDL3   .. "/lib",
      VENDORS_BIG .. "/lua/lib",
      VENDORS_BIG .. "/yaml/lib/Release",
    }
    links {
      "Engine",
      "SDL3",
      "lua54",
      yaml_lib_name(false),
    }

  -- System libs
  filter "system:windows"
    systemversion "latest"
    links { "opengl32", "Comdlg32", "winmm", "imm32", "version", "setupapi" }
    defines { "WIN32", "_WINDOWS", "NOMINMAX" }

    -- Post-build: ensure bin has needed runtime DLLs + assets
    postbuildcommands {
      '{MKDIR} "%{cfg.targetdir}"',

      -- SDL3.dll
      'if exist "' .. SDL3 .. '/lib/SDL3.dll" {COPYFILE} "' .. SDL3 .. '/lib/SDL3.dll" "%{cfg.targetdir}"',

      -- lua54.dll
      'if exist "' .. VENDORS_BIG .. '/lua/lib/lua54.dll" {COPYFILE} "' .. VENDORS_BIG .. '/lua/lib/lua54.dll" "%{cfg.targetdir}"',

      -- yaml-cpp DLLs
    }

  filter { "system:windows", "configurations:Debug" }
    postbuildcommands {
      'if exist "' .. VENDORS_BIG .. '/yaml/lib/Debug/yaml-cppd.dll" {COPYFILE} "' .. VENDORS_BIG .. '/yaml/lib/Debug/yaml-cppd.dll" "%{cfg.targetdir}"',
      'if exist "' .. VENDORS_BIG .. '/yaml/lib/Debug/yaml-cpp.dll"  {COPYFILE} "' .. VENDORS_BIG .. '/yaml/lib/Debug/yaml-cpp.dll"  "%{cfg.targetdir}"',
    }

  filter { "system:windows", "configurations:Release" }
    postbuildcommands {
      'if exist "' .. VENDORS_BIG .. '/yaml/lib/Release/yaml-cpp.dll" {COPYFILE} "' .. VENDORS_BIG .. '/yaml/lib/Release/yaml-cpp.dll" "%{cfg.targetdir}"',
    }

  -- Copy assets from Game and Engine
  filter {}
    postbuildcommands {
      'if exist "Assets" {COPYDIR} "Assets" "%{cfg.targetdir}/Assets"',
      'if exist "Resources" {COPYDIR} "Resources" "%{cfg.targetdir}/Resources"',
      'if exist "' .. ENGINE .. '/Assets" {COPYDIR} "' .. ENGINE .. '/Assets" "%{cfg.targetdir}/Assets"',
      'if exist "Assets/imgui.ini" {COPYFILE} "Assets/imgui.ini" "%{cfg.targetdir}"',
      'if exist "' .. ENGINE .. '/Assets/imgui.ini" {COPYFILE} "' .. ENGINE .. '/Assets/imgui.ini" "%{cfg.targetdir}"',
    }

  -- Config flags
  filter "configurations:Debug"
    symbols "On"
    runtime "Debug"
    optimize "Off"
    staticruntime "Off"
    defines { "DEBUG", "_DEBUG", "GAME_DEBUG" }

  filter "configurations:Release"
    symbols "Off"
    runtime "Release"
    optimize "Speed"
    staticruntime "Off"
    defines { "NDEBUG", "GAME_RELEASE" }

-- Console summary
print("===========================================")
print("Game Configuration:")
print("  Engine Path: " .. ENGINE)
print("  Workspace Location: Build/")
print("  SDL3 Path: " .. SDL3)
print("===========================================")

-- Check Engine.lib
if os.host() == "windows" then
  local libPaths = {
    ENGINE .. "/Build/bin/Debug/Engine.lib",
    ENGINE .. "/Build/bin/Release/Engine.lib",
    ENGINE .. "/Build/lib/Debug/Engine.lib",
    ENGINE .. "/Build/lib/Release/Engine.lib",
  }
  print("Checking for Engine.lib (static library):")
  local found = false
  for _, p in ipairs(libPaths) do
    if os.isfile(p) then print("  ✓ Found: " .. p) found = true end
  end
  if not found then
    print("  ✗ Engine.lib not found!")
    print("  Build Engine as kind \"StaticLib\" and make sure outputs go to Build/bin/<cfg>")
  end
end

print("===========================================")
print("Generate with: premake5 vs2022 --engine-root=PATH")
print("===========================================")
