#pragma once

#include <types.hpp>

#include <build/list.hpp>
#include <build/string.hpp>
#include <build/buffer.hpp>
#include <build/objloader.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Mesh
{
	MeshObj meshFile;
	usize vertexBufferOffset = 0;
	usize indexBufferOffset = 0;
	float minX, minY, minZ;
	float maxX, maxY, maxZ;

	String filepath;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Meshes
{
	List<Mesh> meshes;

	void make_new( const Mesh &material );
	void gather( const char *path, const bool recurse = true );
	void load( const char *path );
	void write();

	inline Mesh &operator[]( const u32 meshID ) { return meshes[meshID]; }
};