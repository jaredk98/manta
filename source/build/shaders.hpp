#pragma once

#include <types.hpp>
#include <debug.hpp>
#include <config.hpp>

#include <build/build.hpp>
#include <build/fileio.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Shader
{
	Shader() = default;
	Shader( const char *name ) : name{ name } { }

	String name;
	String codeVertex;
	String codeFragment;
	String codeCompute;

	u32 offsetVertex = 0;
	u32 sizeVertex = 0;
	u32 offsetFragment = 0;
	u32 sizeFragment = 0;
	u32 offsetCompute = 0;
	u32 sizeCompute = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace shaders
{
	// Output Paths
	extern char pathHeader[PATH_SIZE];
	extern char pathSource[PATH_SIZE];

	// Output contents
	extern String source;
	extern String header;

	// Cache
	extern usize shaderFileCount;
	extern FileTime timeCache;

	// Shaders
	extern List<FileInfo> shaderFiles;
	extern List<Shader> shaders;

	// Stages
	extern void begin();
	extern u32 gather( const char *path, const bool recurse );
	extern void build();
	extern void write();

	extern void parse_shader_glsl( const FileInfo &file );
	extern void parse_shader_hlsl( const FileInfo &file );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////