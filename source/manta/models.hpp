#pragma once

#include <types.hpp>

#include <manta/gfx.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Model
{
	// Mesh mesh;
	GfxVertexBuffer<GfxVertex::BuiltinVertex> vertexBuffer;
	u16 material = 0;

	bool init( const u32 meshID, const u16 materialID );
	bool free();
	void draw( float x, float y, float z, float scale, float rotation );
};