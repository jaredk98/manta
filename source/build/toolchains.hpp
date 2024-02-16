#pragma once

#include <debug.hpp>
#include <vendor/string.hpp>

#include <build/arguments.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Toolchain
{
	void detect( Arguments &args )
	{
		// MSVC
		if( strcmp( args.toolchain, "msvc" ) == 0 )
		{
			compilerName = "cl";
			compilerFlags = "-c -showIncludes -nologo -std:c++20 -EHsc -DUNICODE";
			compilerFlagsIncludes = "-I\"%s\"";
			compilerFlagsWarnings = "-wd4100 -wd4101 -wd4189 -wd4201 -wd4244 -wd4456 -wd4458 -wd4459 -wd4505 -wd4702";
			if( strcmp( args.architecture,   "x64" ) == 0 ) { compilerFlagsArchitecture = ""; } else
			if( strcmp( args.architecture, "arm64" ) == 0 ) { compilerFlagsArchitecture = "-arch:ARM64"; } else
															{ Error( "Unsupported target architecture '%s' for compiler '%s'", args.architecture, args.toolchain ); }
			compilerOutput = "-Fo:";

			linkerName = "link";
			linkerFlags = "-nologo";
			linkerOutput = "-out:";

			linkerExtensionExe = PIPELINE_OS_WINDOWS ? ".exe" : "";
			linkerExtensionObj = ".obj";
			linkerExtensionLibrary = ".lib";
			linkerPrefixLibrary = "";
		}
		// LLVM
		else if( strcmp( args.toolchain, "llvm" ) == 0 )
		{
			compilerName = "clang";
			compilerFlags = PIPELINE_OS_MACOS ? "-x objective-c++ -c -MD -MF $out.d -std=c++20 -fno-exceptions -DUNICODE" :
			                                    "-c -MD -MF $out.d -std=c++20 -fno-exceptions -DUNICODE";
			compilerFlagsIncludes = "-I%s";
			compilerFlagsWarnings = "-Wno-unused-variable -Wno-unused-function -Wno-unused-private-field";
			if( strcmp( args.architecture,   "x64" ) == 0 ) { compilerFlagsArchitecture = "-m64"; } else
			if( strcmp( args.architecture, "arm64" ) == 0 ) { compilerFlagsArchitecture = "-target aarch64-linux-gnu"; } else
															{ Error( "Unsupported target architecture '%s' for compiler '%s'", args.architecture, args.toolchain ); }
			compilerOutput = "-o ";

			linkerName = "clang++";
			linkerFlags = "";
			linkerOutput = "-o ";

			linkerExtensionExe = PIPELINE_OS_WINDOWS ? ".exe" : "";
			linkerExtensionObj = ".o";
			linkerExtensionLibrary = "";
			linkerPrefixLibrary = "-l";
		}
		// GNU
		else if( strcmp( args.toolchain, "gnu" ) == 0 )
		{
			compilerName = "gcc";
			compilerFlags = PIPELINE_OS_MACOS ? "-x objective-c++ -c -MD -MF $out.d -std=c++20 -fno-exceptions -DUNICODE" :
			                                    "-c -MD -MF $out.d -std=c++20 -fno-exceptions -DUNICODE";
			compilerFlagsIncludes = "-I%s";
			compilerFlagsWarnings = "-Wno-unused-variable -Wno-unused-function -Wno-unused-private-field";
			if( strcmp( args.architecture,   "x64" ) == 0 ) { compilerFlagsArchitecture = "-m64"; } else
			if( strcmp( args.architecture, "arm64" ) == 0 ) { compilerFlagsArchitecture = "-march=armv8-a"; } else
															{ Error( "Unsupported target architecture '%s' for compiler '%s'", args.architecture, args.toolchain ); }
			compilerOutput = "-o ";

			linkerName = "g++";
			linkerFlags = "";
			linkerOutput = "-o ";

			linkerExtensionExe = PIPELINE_OS_WINDOWS ? ".exe" : "";
			linkerExtensionObj = ".o";
			linkerExtensionLibrary = "";
			linkerPrefixLibrary = "-l";
		}
		else
		{
			Error( "Unsupported toolchain! '%s'", args.toolchain );
		}
	}

	const char *compilerName;
	const char *compilerFlags;
	const char *compilerFlagsIncludes;
	const char *compilerFlagsWarnings;
	const char *compilerFlagsArchitecture;
	const char *compilerOutput;

	const char *linkerName;
	const char *linkerFlags;
	const char *linkerOutput;

	const char *linkerExtensionExe;
	const char *linkerExtensionObj;
	const char *linkerExtensionLibrary;
	const char *linkerPrefixLibrary;
};