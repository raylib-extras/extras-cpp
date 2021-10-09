workspace "extras"
	configurations { "Debug","Debug.DLL", "Release", "Release.DLL" }
	platforms { "x64"}
	defaultplatform "x64"
	
	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"
		
	filter "configurations:Debug.DLL"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"	
		
	filter "configurations:Release.DLL"
		defines { "NDEBUG" }
		optimize "On"	
		
	filter { "platforms:x64" }
		architecture "x86_64"
		
	targetdir "bin/%{cfg.buildcfg}/"
		
project "raylib"
	filter "configurations:Debug.DLL OR Release.DLL"
		kind "SharedLib"
		defines {"BUILD_LIBTYPE_SHARED"}
		
	filter "configurations:Debug OR Release"
		kind "StaticLib"
		
	filter "action:vs*"
		defines{"_WINSOCK_DEPRECATED_NO_WARNINGS", "_CRT_SECURE_NO_WARNINGS", "_WIN32"}
		links {"winmm"}
				
	filter{}
	
	location "build"
	language "C"
	targetdir "bin/%{cfg.buildcfg}"
	
	includedirs { "raylib/src", "raylib/src/external/glfw/include"}
	vpaths 
	{
		["Header Files"] = { "raylib/src/**.h"},
		["Source Files/*"] = {"raylib/src/**.c"},
	}
	files {"raylib/src/*.h", "raylib/src/*.c"}
	
	defines{"PLATFORM_DESKTOP", "GRAPHICS_API_OPENGL_33"}

project "rlFPCamera"
	kind "StaticLib"
	
	location "build"
	language "C++"
	targetdir "bin/%{cfg.buildcfg}"
	
	includedirs { "raylib/src"}
	vpaths 
	{
		["Header Files"] = { "cameras/rlFPCamera/*.h"},
		["Source Files"] = {"cameras/rlFPCamera/*.cpp"},
	}
	files {"cameras/rlFPCamera/*.cpp","cameras/rlFPCamera/*.h"}

project "rlTPCamera"
	kind "StaticLib"
	
	location "build"
	language "C++"
	targetdir "bin/%{cfg.buildcfg}"
	
	includedirs { "raylib/src"}
	vpaths 
	{
		["Header Files"] = { "cameras/rlTPCamera/*.h"},
		["Source Files"] = {"cameras/rlTPCamera/*.cpp"},
	}
	files {"cameras/rlTPCamera/*.cpp","cameras/rlTPCamera/*.h"}

group "Examples"
project "rlFPCamera_sample"
	kind "ConsoleApp"
	location "cameras/rlFPCamera/samples"
	language "C++"
	targetdir "bin/%{cfg.buildcfg}"
	
	vpaths 
	{
		["Header Files"] = { "cameras/rlFPCamera/samples/*.h"},
		["Source Files"] = {"cameras/rlFPCamera/samples/*.cpp" },
	}
	files {"cameras/rlFPCamera/samples/*.cpp"}

	links {"raylib", "rlFPCamera"}
	
	includedirs {"./", "raylib/src", "cameras/rlFPCamera" }
	
	filter "action:vs*"
		defines{"_WINSOCK_DEPRECATED_NO_WARNINGS", "_CRT_SECURE_NO_WARNINGS", "_WIN32"}
		links {"winmm"}
		
project "rlTPCamera_sample"
	kind "ConsoleApp"
	location "cameras/rlTPCamera/samples"
	language "C++"
	targetdir "bin/%{cfg.buildcfg}"
	
	vpaths 
	{
		["Header Files"] = { "cameras/rlTPCamera/samples/*.h"},
		["Source Files"] = {"cameras/rlTPCamera/samples/*.cpp" },
	}
	files {"cameras/rlTPCamera/samples/*.cpp"}

	links {"raylib", "rlTPCamera"}
	
	includedirs {"./", "raylib/src", "cameras/rlTPCamera" }
	
	filter "action:vs*"
		defines{"_WINSOCK_DEPRECATED_NO_WARNINGS", "_CRT_SECURE_NO_WARNINGS", "_WIN32"}
		links {"winmm"}

