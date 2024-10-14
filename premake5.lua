-- Ascii-GL (workspace)

require "premake/workspace-files"

workspace "Ascii-GL"
  architecture("x86_64")
  startproject("ascii-gl-tests")

  configurations {
    "Release",
    "Debug"
  }

IncludeDir = {}
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Enables OpenMP API for shared-memory parallel programming
--openmp "On"

--vectorextensions "SSE"
--vectorextensions "SSE2"
--vectorextensions "SSE3"
--vectorextensions "SSE4.1"
--vectorextensions "SSE4.2"

defines {
  "",
}

workspace_files {
  "premake5.lua",
  ".gitignore"
}

filter "configurations:windows"
  defines "_WIN32"

filter "configurations:Debug"
  defines "_DEBUG"
  runtime "Debug"
  symbols "On"
  
filter "configurations:Release"
  runtime "Release"
  optimize "On"

-- dependencies compiled from source
group "Dependencies"
  include("libs/glm")
  include("libs/frozen")

group ""
  include("ascii-gl")
  include("tests")
