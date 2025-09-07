workspace "EnigmaMDOC"
    architecture "x64"

    configurations {
        "Debug",
        "Release"
    }

    startproject "SpaceSim"

-- Directory final files will be placed into
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Libraries
include "vendor/CommandLineTools/CommandLineTools/CommandLineTools"

-- Main Project
include "EnigmaMDOC"