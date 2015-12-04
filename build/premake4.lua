--[[
 To compile the project files, install
 premake4 ( http://industriousone.com/premake ), then:

    For windows.
        $ premake4 vs2010

    For linux.
        $ premake4 gmake

    For macosx.
        $ premake4 xcode3

]]--

--===========================================================================--
-- Globals
--===========================================================================--

local projectname = "waterspout"
local projectkind = "ConsoleApp"


--===========================================================================--
-- Common functions
--===========================================================================--
function setup_solution(projectname, projectkind, platformname)

  -- solution
  solution(projectname)
  platforms { "x32", "x64" }

  if platformname == "Linux" then
    configurations { "debug", "release" }
  elseif platformname == "Windows" then
    configurations { "debug", "release" }
  elseif platformname == "MacOSX" then
    configurations { "debug", "release" }
  end

  -- project
  project(projectname)
  kind(projectkind)
  language "C++"

  -- build/link options
  flags {
    "EnableSSE",
    "EnableSSE2",
    "ExtraWarnings",
    "FatalWarnings",
    "OptimizeSpeed",
    "NoFramePointer",
    "NoImportLib"
  }

  -- target dirs
  configuration { "debug", "x32" }
    targetdir("../bin/" .. platformname .. "/debug32")
    objdir("../bin/" .. platformname .. "/debug32/obj")
  configuration {}
  configuration { "debug", "x64" }
    targetdir("../bin/" .. platformname .. "/debug64")
    objdir("../bin/" .. platformname .. "/debug64/obj")
  configuration {}
  configuration { "release", "x32" }
    targetdir("../bin/" .. platformname .. "/release32")
    objdir("../bin/" .. platformname .. "/release32/obj")
  configuration {}
  configuration { "release", "x64" }
    targetdir("../bin/" .. platformname .. "/release64")
    objdir("../bin/" .. platformname .. "/release64/obj")
  configuration {}

  -- specific configurations
  configuration { "debug*" }
    defines { "DEBUG=1" }
    flags { "Symbols" }
  configuration {}

  configuration { "release*" }
    defines { "NDEBUG=1" }
    flags { "OptimizeSpeed" }
  configuration {}

  -- global defines
  --defines {
  --}

  -- include directories
  includedirs {
    "../include",
    "../tests"
  }

  -- project files
  files {
    "../src/*.h",
    "../src/*.cpp",
    "../tests/*.h",
    "../tests/*.cpp"
  }

end


--===========================================================================--
-- begin specific configurations
--===========================================================================--

--===========================================================================--
if _ACTION == "gmake" then
  setup_solution(projectname, projectkind, "Linux")

  defines {
    "LINUX=1"
  }

  buildoptions {
    "-march=native",
    "-std=c++0x", -- should be -std=c++11 (this is needed as we build for old gcc in travis)
    "-fPIC",
    "-Wno-error"
  }

  includedirs {
    "/usr/include"
  }

  links {
    "m"
  }

--===========================================================================--
elseif _ACTION == "vs2010" then
  setup_solution(projectname, projectkind, "Windows")

  buildoptions {
  	"/wd4127",
  	"/wd4146",
  	"/wd4244",
  	"/wd4305",
  	"/wd4310"
  }

  defines {
    "WIN32=1",
    "_SCL_SECURE_NO_WARNINGS=1",
    "_CRT_SECURE_NO_WARNINGS=1",
    "inline=__inline"
  }

--===========================================================================--
elseif _ACTION == "xcode3" then
  setup_solution(projectname, projectkind, "MacOSX")

  defines {
  	"MACOSX=1",
  }

  buildoptions {
  	"-march=native",
  	"-fPIC",
    "-Wno-error"
  }

  --[[
  links {
    "AudioToolbox.framework",
    "AudioUnit.framework",
    "Carbon.framework",
    "Cocoa.framework",
    "CoreAudio.framework",
    "CoreAudioKit.framework",
    "CoreMIDI.framework",
    "DiscRecording.framework",
    "IOKit.framework",
    "OpenGL.framework",
    "QTKit.framework",
    "QuartzCore.framework",
    "QuickTime.framework",
    "WebKit.framework"
  }
  ]]--

end
