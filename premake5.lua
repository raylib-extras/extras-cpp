
newoption
{
	trigger = "graphics",
	value = "OPENGL_VERSION",
	description = "version of OpenGL to build raylib against",
	allowed = {
		{ "opengl11", "OpenGL 1.1"},
		{ "opengl21", "OpenGL 2.1"},
		{ "opengl33", "OpenGL 3.3"},
		{ "opengl43", "OpenGL 4.3"}
	},
	default = "opengl33"
}

function define_C()
	language "C"
end

function define_Cpp()
	language "C++"
end

function string.starts(String,Start)
	return string.sub(String,1,string.len(Start))==Start
end

function link_to(lib)
	links (lib)
	includedirs ("../"..lib.."/include", "../"..lib.."/" )
end

function download_progress(total, current)
	local ratio = current / total;
	ratio = math.min(math.max(ratio, 0), 1);
	local percent = math.floor(ratio * 100);
	print("Download progress (" .. percent .. "%/100%)")
end

function check_raylib()
	if(os.isdir("raylib") == false and os.isdir("raylib-master") == false) then
		if(not os.isfile("raylib-master.zip")) then
			print("Raylib not found, downloading from github")
			local result_str, response_code = http.download("https://github.com/raysan5/raylib/archive/refs/heads/master.zip", "raylib-master.zip", {
				progress = download_progress,
				headers = { "From: Premake", "Referer: Premake" }
			})
		end
		print("Unzipping to " ..  os.getcwd())
		zip.extract("raylib-master.zip", os.getcwd())
		os.remove("raylib-master.zip")
	end
end

workspace "raylib-extras-cpp"
	configurations { "Debug", "Release"}
	platforms { "x64", "x86"}

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"

	filter { "platforms:x64" }
		architecture "x86_64"

	filter {}

	targetdir "_bin/%{cfg.buildcfg}/"
	
	startproject('rlFPCamera_sample')
	
	cdialect "C99"
	cppdialect "C++11"
	check_raylib()
	
	include ("raylib_premake5.lua")

project "rlFPCamera"
	kind "StaticLib"
	
	location "_build"
	targetdir "_bin/%{cfg.buildcfg}"
	language "C++"
	
	includedirs { "raylib/src"}
	vpaths 
	{
		["Header Files"] = { "cameras/rlFPCamera/*.h"},
		["Source Files"] = {"cameras/rlFPCamera/*.cpp"},
	}
	files {"cameras/rlFPCamera/*.cpp","cameras/rlFPCamera/*.h"}
	include_raylib()

project "rlTPCamera"
	kind "StaticLib"
	
	location "_build"
	targetdir "_bin/%{cfg.buildcfg}"
	language "C++"
	
	includedirs { "raylib/src"}
	vpaths 
	{
		["Header Files"] = { "cameras/rlTPCamera/*.h"},
		["Source Files"] = {"cameras/rlTPCamera/*.cpp"},
	}
	files {"cameras/rlTPCamera/*.cpp","cameras/rlTPCamera/*.h"}
	include_raylib()
	
group "Examples"
project "rlFPCamera_sample"
	kind "ConsoleApp"
	location "_build"
	targetdir "_bin/%{cfg.buildcfg}"
	language "C++"

	vpaths 
	{
		["Header Files"] = { "cameras/rlFPCamera/samples/*.h"},
		["Source Files"] = {"cameras/rlFPCamera/samples/*.cpp" },
	}
	files {"cameras/rlFPCamera/samples/*.cpp"}

	links {"rlFPCamera"}
	
	includedirs {"./", "cameras/rlFPCamera" }
	
	link_raylib()
	
project "rlTPCamera_sample"
	kind "ConsoleApp"
	location "_build"
	targetdir "_bin/%{cfg.buildcfg}"
	language "C++"
	
	vpaths 
	{
		["Header Files"] = { "cameras/rlTPCamera/samples/*.h"},
		["Source Files"] = {"cameras/rlTPCamera/samples/*.cpp" },
	}
	files {"cameras/rlTPCamera/samples/*.cpp"}

	links {"rlTPCamera"}
	
	includedirs {"./", "cameras/rlTPCamera" }
	
	link_raylib()
