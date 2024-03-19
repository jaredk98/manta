#pragma once

#include <types.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// TODO: Do this differently?

struct GfxBuiltInVertex
{
	float x, y, z;
	// TODO: normals
	u16 u, v;
	u8 r, g, b, a;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct MeshObj
{
	bool load( const char *path );
	bool free();

	GfxBuiltInVertex *vertexBufferData = nullptr;
	usize vertexBufferSize = 0;
	usize vertexCount = 0;

	u32 *indexBufferData = nullptr;
	usize indexBufferSize = 0;
	usize indexCount = 0;
};