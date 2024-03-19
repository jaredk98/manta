#pragma once

#include <types.hpp>

#include <build/list.hpp>
#include <build/string.hpp>
#include <build/buffer.hpp>

#include <build/assets/textures.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Material
{
	TextureID textureIDColor;
	TextureID textureIDNormal;
	String name;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Materials
{
	List<Material> materials;

	void make_new( const Material &material );
	void gather( const char *path, const bool recurse = true );
	void load( const char *path );
	void write();

	inline Material &operator[]( const u32 materialID ) { return materials[materialID]; }
};