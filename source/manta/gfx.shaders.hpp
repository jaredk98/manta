#pragma once

#include <shaders.generated.hpp>

#include <types.hpp>
#include <config.hpp>
#include <debug.hpp>
#include <pipeline.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// TODO: Refactor this

#if GRAPHICS_D3D11
	#include <manta/backend/gfx/d3d11/d3d11.hpp>
#elif GRAPHICS_D3D12
	#include <manta/backend/gfx/d3d12/d3d12.hpp>
#elif GRAPHICS_OPENGL
	#include <manta/backend/gfx/opengl/opengl.hpp>
#elif GRAPHICS_METAL
	#include <manta/backend/gfx/metal/metal.hpp>
#elif GRAPHICS_VULKAN
	#include <manta/backend/gfx/vulkan/vulkan.hpp>
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Shader
{
#if GRAPHICS_D3D11
	struct ID3D11VertexShader *vs;
	struct ID3D11PixelShader *ps;
	struct ID3D11InputLayout *il; // TODO
#endif

#if GRAPHICS_D3D12
	// ...
#endif

#if GRAPHICS_OPENGL
	GLuint shaderVertex;
	GLuint shaderFragment;
	GLuint shaderCompute;
	GLuint program;
#endif

#if GRAPHICS_METAL
	// ...
#endif

#if GRAPHICS_VULKAN
	// ...
#endif
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct GfxBuffer
{
#if GRAPHICS_D3D11
	ID3D11Buffer *gfxbuffer;
	UINT slot;
#endif

#if GRAPHICS_D3D12
	// ...
#endif

#if GRAPHICS_OPENGL
	GLuint location;
#endif

#if GRAPHICS_METAL
	// ...
#endif

#if GRAPHICS_VULKAN
	// ...
#endif
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace shaders
{
	extern Shader shaders[shadersCount];

	extern bool init();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////