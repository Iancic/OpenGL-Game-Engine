local TP       = "Libraries"
local SDL3     = TP .. "/SDL3"          -- include/, lib/ (SDL3.lib, SDL3.dll)
local LUA      = TP .. "/lua"           -- include/, lib/ (lua54.lib, lua54.dll)
local YAML     = TP .. "/yaml"          -- include/, lib/Debug|Release (yaml-cpp(d).lib/.dll)
local ENTT     = TP .. "/entt"          -- header-only
local SOL2     = TP .. "/sol"           -- header-only
local IMGUI    = TP .. "/imgui-master"  -- sources + backends
local GLADH    = "libraries/glad"       -- glad.h
local STB      = "libraries/stb"        -- stb_image.cpp
local BOX2D    = TP .. "/box2d"         -- <-- Box2D **2.4.x** (C++) here: include/, src/

-- DIR macro like your original .vcxproj (absolute repo path)
local ENGINE_ROOT = path.translate(path.getabsolute(path.getdirectory(_SCRIPT)), "/")
local DIR_DEFINE  = 'DIR=R\\"(' .. ENGINE_ROOT .. ')\\"'

-- helpers
local function exists(p)    return os.isfile(path.translate(p, "\\")) end
local function existsdir(p) return os.isdir(path.translate(p, "\\")) end
local function join(a,b)    return a .. "/" .. b end

-- --- YAML auto-detect (handles setups that only have non-d libs) ---
local YAML_LIB_DEBUG    = exists(YAML.."/lib/Debug/yaml-cppd.lib") and "yaml-cppd" or "yaml-cpp"
local YAML_DLL_DEBUG    = exists(YAML.."/lib/Debug/yaml-cppd.dll") and "yaml-cppd.dll" or "yaml-cpp.dll"
local YAML_LIB_RELEASE  = exists(YAML.."/lib/Release/yaml-cppd.lib") and "yaml-cppd" or "yaml-cpp"
local YAML_DLL_RELEASE  = exists(YAML.."/lib/Release/yaml-cppd.dll") and "yaml-cppd.dll" or "yaml-cpp.dll"

-- --- SDL3 auto-detect libdir + flavor (shared vs static) ---
local SDL3_LIBDIRS_CANDIDATES = {
  join(SDL3, "lib"),
  join(SDL3, "lib/x64"),
  join(SDL3, "lib/Release"),
  join(SDL3, "lib/Debug"),
}
local function first_existing_dir(cands)
  for _,d in ipairs(cands) do if existsdir(d) then return d end end
  return nil
end
local SDL3_LIBDIR = first_existing_dir(SDL3_LIBDIRS_CANDIDATES) or (SDL3 .. "/lib")
local SDL3_SHARED_LIB = exists(SDL3_LIBDIR .. "/SDL3.lib")
local SDL3_STATIC_LIB = exists(SDL3_LIBDIR .. "/SDL3-static.lib")
local USE_SDL3_STATIC = (not SDL3_SHARED_LIB) and SDL3_STATIC_LIB

project "Engine"
  kind "StaticLib"
  language "C++"
  cppdialect "C++20"
  characterset "Unicode"
  staticruntime "Off"          -- /MDd Debug, /MD Release
  conformancemode "On"
  rtti "On"
  exceptionhandling "On"

  targetname "Engine"
  targetdir ("%{wks.location}/bin/%{cfg.buildcfg}")
  objdir   ("%{wks.location}/obj/%{prj.name}/%{cfg.buildcfg}")
  debugdir "%{cfg.targetdir}"

  files {
    -- Engine
    "Source/**.cpp",
    "Headers/**.h",
    "Headers/**.hpp",

    -- ImGui core
    IMGUI .. "/imgui.cpp",
    IMGUI .. "/imgui_demo.cpp",
    IMGUI .. "/imgui_draw.cpp",
    IMGUI .. "/imgui_tables.cpp",
    IMGUI .. "/imgui_widgets.cpp",

    -- ImGui backends (SDL3 + OpenGL3)
    IMGUI .. "/backends/imgui_impl_opengl3.cpp",
    IMGUI .. "/backends/imgui_impl_sdl3.cpp",

    -- ImGuizmo (if present)
    IMGUI .. "/ImGuizmo.cpp",

    -- GLAD C source
    "Source/glad.c",

    -- STB implementation
    STB .. "/stb_image.cpp",

    -- Box2D 2.4.x (C++) sources
    BOX2D .. "/src/**.cpp",
    BOX2D .. "/src/**.cc",
    BOX2D .. "/src/**.cxx",
  }

  -- Exclude EXE entry point from the library
  removefiles { "Source/main.cpp" }

  includedirs {
    "Headers",
    "Libraries",
    YAML .. "/include",
    YAML .. "/include/yaml-cpp",
    ENTT,
    SOL2,
    GLADH,
    IMGUI,
    IMGUI .. "/backends",
    BOX2D .. "/include",           -- Box2D public headers (enables #include <box2d/box2d.h>)
    SDL3 .. "/include",            -- SDL3 headers
  }

  defines {
    "ENGINE_BUILD",
    "SDL_MAIN_HANDLED",            -- avoid SDL entry point in static lib
    "B2_STATIC",                   -- Box2D static (optional, harmless)
    DIR_DEFINE,
    "_CONSOLE",
  }

  filter "system:windows"
    systemversion "latest"
    links { "Comdlg32", "opengl32" }
    if USE_SDL3_STATIC then
      defines { "SDL_BUILD_STATIC", "SDL_DISABLE_WINDOWS_IME=0" }
      links { "Version", "Imm32", "Winmm", "Setupapi" }
    end
  filter {}

  ----------------------------------------------------------------
  -- Debug
  ----------------------------------------------------------------
  filter "configurations:Debug"
    symbols "On"
    runtime "Debug"
    defines { "DEBUG", "_DEBUG" }
    warnings "Off"

    libdirs {
      SDL3_LIBDIR,
      LUA  .. "/lib",
      YAML .. "/lib/Debug",
    }
    links {
      (USE_SDL3_STATIC and "SDL3-static" or "SDL3"),
      "lua54",
      YAML_LIB_DEBUG,
    }
    if not USE_SDL3_STATIC then
      postbuildcommands {
        ('cmd /c if exist "%s\\SDL3.dll" copy /Y "%s\\SDL3.dll" "%%{cfg.targetdir}"'):format(SDL3_LIBDIR, SDL3_LIBDIR),
      }
    end
    postbuildcommands {
      'cmd /c if exist "Libraries\\lua\\lib\\lua54.dll" copy /Y "Libraries\\lua\\lib\\lua54.dll" "%{cfg.targetdir}"',
      'cmd /c if exist "Libraries\\yaml\\lib\\Debug\\' .. YAML_DLL_DEBUG .. '" copy /Y "Libraries\\yaml\\lib\\Debug\\' .. YAML_DLL_DEBUG .. '" "%{cfg.targetdir}"'
    }
  filter {}

  ----------------------------------------------------------------
  -- Release
  ----------------------------------------------------------------
  filter "configurations:Release"
    optimize "Speed"
    runtime "Release"
    defines { "NDEBUG" }
    warnings "Default"

    libdirs {
      SDL3_LIBDIR,
      LUA  .. "/lib",
      YAML .. "/lib/Release",
    }
    links {
      (USE_SDL3_STATIC and "SDL3-static" or "SDL3"),
      "lua54",
      YAML_LIB_RELEASE,
    }
    if not USE_SDL3_STATIC then
      postbuildcommands {
        ('cmd /c if exist "%s\\SDL3.dll" copy /Y "%s\\SDL3.dll" "%%{cfg.targetdir}"'):format(SDL3_LIBDIR, SDL3_LIBDIR),
      }
    end
    postbuildcommands {
      'cmd /c if exist "Libraries\\lua\\lib\\lua54.dll" copy /Y "Libraries\\lua\\lib\\lua54.dll" "%{cfg.targetdir}"',
      'cmd /c if exist "Libraries\\yaml\\lib\\Release\\' .. YAML_DLL_RELEASE .. '" copy /Y "Libraries\\yaml\\lib\\Release\\' .. YAML_DLL_RELEASE .. '" "%{cfg.targetdir}"'
    }
  filter {}

  ----------------------------------------------------------------
  -- Precompiled headers (PCH) + exclusions
  ----------------------------------------------------------------
  pchheader "precomp.h"
  pchsource "Source/precomp.cpp"

  -- Force PCH only for your engine C++ sources
  filter "files:Source/**.cpp"
    forceincludes { "precomp.h" }
  filter {}

  -- Disable PCH for 3rd-party sources
  filter { "files:" .. IMGUI .. "/**.cpp" }
    flags { "NoPCH" }
    externalwarnings "Off"
    warnings "Off"
  filter {}

  filter { "files:" .. STB .. "/stb_image.cpp" }
    flags { "NoPCH" }
    externalwarnings "Off"
    warnings "Off"
  filter {}

  filter "files:Source/glad.c"
    flags { "NoPCH" }
    externalwarnings "Off"
    warnings "Off"
  filter {}

  -- Disable PCH for Box2D C++ sources as well (avoid precomp.h inclusion)
  filter { "files:" .. BOX2D .. "/src/**.cpp" }
    flags { "NoPCH" }
    externalwarnings "Off"
    warnings "Off"
  filter {}