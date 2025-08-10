workspace "SCOL"
  architecture "x64"
  startproject "SCOL"

  configurations { "Release" }

  outputdir = "%{cfg.buildcfg}"
  
  IncludeDir = {}
  IncludeDir["MinHook"] = "vendor/MinHook/include"
  
  CppVersion = "C++20"
  MsvcToolset = "v143"
  WindowsSdkVersion = "10.0"

  function DeclareMSVCOptions()
    filter "system:windows"
      staticruntime "Off"
      floatingpoint "Fast"
      vectorextensions "AVX2"
      systemversion (WindowsSdkVersion)
      toolset (MsvcToolset)
      cppdialect (CppVersion)

      defines {
        "_CRT_SECURE_NO_WARNINGS",
        "NOMINMAX",
        "WIN32_LEAN_AND_MEAN"
      }

      disablewarnings {
        "4100", "4201", "4307", "4996"
      }
  end

  project "MinHook"
    location "vendor/%{prj.name}"
    kind "StaticLib"
    language "C"

    targetdir ("bin/lib/" .. outputdir)
    objdir ("bin/lib/int/" .. outputdir .. "/%{prj.name}")

    files
    {
      "vendor/%{prj.name}/include/**.h",
      "vendor/%{prj.name}/src/**.h",
      "vendor/%{prj.name}/src/**.c"
    }

    DeclareMSVCOptions()

  project "SCOL"
    kind "SharedLib"
    language "C++"
    targetextension ".asi"

    targetdir ("bin/" .. outputdir)
    objdir ("bin/int/" .. outputdir .. "/%{prj.name}")

    PrecompiledHeaderInclude = "Common.hpp"
    PrecompiledHeaderSource = "src/Common.cpp"

    files { "src/**.hpp", "src/**.cpp" }
    includedirs { "%{IncludeDir.MinHook}", "src" }

    libdirs { "bin/lib" }
    links { "MinHook" }

    pchheader "%{PrecompiledHeaderInclude}"
    pchsource "%{PrecompiledHeaderSource}"
    forceincludes { "%{PrecompiledHeaderInclude}" }

    DeclareMSVCOptions()

    flags { "NoImportLib" }

    filter "configurations:Release"
      flags { "linktimeoptimization", "NoManifest", "MultiProcessorCompile" }
      defines { "SCOL_RELEASE" }
      optimize "Speed"