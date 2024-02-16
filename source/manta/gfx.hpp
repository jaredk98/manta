#pragma once

#include <config.hpp>
#include <types.hpp>
#include <debug.hpp>

#include <manta/matrix.hpp>
#include <manta/color.hpp>
#include <manta/assets.hpp>

#include <manta/gfx.shaders.hpp>

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

enum_type( GfxFilteringMode, u8 )
{
	GfxFilteringMode_NEAREST = 0,
	GfxFilteringMode_LINEAR,
	GfxFilteringMode_ANISOTROPIC,

	GFXFILTERINGMODE_COUNT,
};

enum_type( GfxDepthTestMode, u8 )
{
	GfxDepthTestMode_NONE = 0,

	GfxDepthTestMode_LESS,
	GfxDepthTestMode_LESS_EQUALS,
	GfxDepthTestMode_ALWAYS,

	GFXDEPTHTESTMODE_COUNT,
};

enum_type( GfxColorFormat, u8 )
{
	GfxColorFormat_NONE = 0,

	GfxColorFormat_R8G8B8A8,
	GfxColorFormat_R10G10B10A2,
	GfxColorFormat_R8,
	GfxColorFormat_R8G8,
	GfxColorFormat_R16,
	GfxColorFormat_R16G16,
	GfxColorFormat_R32,

	GFXCOLORFORMAT_COUNT,
};

enum_type( GfxDepthFormat, u8 )
{
	GfxDepthFormat_NONE = 0,

	GfxDepthFormat_R16_FLOAT,
	GfxDepthFormat_R16_UINT,
	GfxDepthFormat_R32_FLOAT,
	GfxDepthFormat_R32_UINT,
	GfxDepthFormat_R24_UINT_G8_UINT,

	GFXDEPTHFORMAT_COUNT,
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct GfxVertexBuffer
{
#if GRAPHICS_D3D11
	struct ID3D11Buffer *buffer;
#endif

#if GRAPHICS_D3D12
	// ...
#endif

#if GRAPHICS_OPENGL
	GLuint vbo;
	GLuint vao;
#endif

#if GRAPHICS_METAL
	// ...
#endif

#if GRAPHICS_VULKAN
	// ...
#endif
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct GfxIndexBuffer
{
#if GRAPHICS_OPENGL
	// ...
#endif

#if GRAPHICS_D3D11
	// ...
#endif

#if GRAPHICS_METAL
	//...
#endif

#if GRAPHICS_VULKAN
	//...
#endif
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct GfxVertex // TODO: Generate These
{
	float x, y, z;
	u16 u, v;
	Color rgba;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
struct GfxQuad
{
	T v1, v2, v3, v4;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct GfxTexture2DDescription
{
	GfxColorFormat colorFormat = GfxColorFormat_R8G8B8A8;
};

struct GfxTexture2D
{
	bool exists = false;

#if GRAPHICS_D3D11
	struct ID3D11ShaderResourceView *view;
#endif

#if GRAPHICS_D3D12
	// ...
#endif

#if GRAPHICS_OPENGL
	GLuint texture;
#endif

#if GRAPHICS_METAL
	// ...
#endif

#if GRAPHICS_VULKAN
	// ...
#endif
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct GfxSurface2DDescription
{
	GfxColorFormat colorFormat = GfxColorFormat_R8G8B8A8;
	GfxDepthFormat depthFormat = GfxDepthFormat_NONE;
};

struct GfxSurface2D
{
	bool exists = false;
	u16 width;
	u16 height;
	GfxSurface2DDescription desc;

	GfxTexture2D textureColor;
	GfxTexture2D textureDepth;

#if GRAPHICS_D3D11
	struct ID3D11RenderTargetView *targetColor = nullptr;
	struct ID3D11DepthStencilView *targetDepth = nullptr;
#endif

#if GRAPHICS_D3D12
	// ...
#endif

#if GRAPHICS_OPENGL
	GLuint fbo;
	//GLuint rbo;
#endif

#if GRAPHICS_METAL
	// ...
#endif

#if GRAPHICS_VULKAN
	// ...
#endif

	void init( const u16 width, const u16 height, const GfxSurface2DDescription &desc = { } );
	void free();
	void bind();
	void release();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct RenderState
{
	RenderState() = default;

	bool dirty = false;

	u32 shader = SHADER_DEFAULT;
	GfxTexture2D const * texture2D = nullptr;
	GfxFilteringMode filteringMode = GfxFilteringMode_NEAREST;
	GfxDepthTestMode depthTestMode = GfxDepthTestMode_NONE;
	Matrix matrixMVP;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace bGfx
{
	extern bool rendering;

	extern RenderState state;
	extern RenderState statePrevious;

	extern i32 viewportWidth;
	extern i32 viewportHeight;
	extern bool viewportFullscreen;

	extern struct GfxQuad<GfxVertex> *quadBatchStart;
	extern struct GfxQuad<GfxVertex> *quadBatchPointer;
	extern struct GfxTexture2D *quadBatchTexture;

	extern GfxTexture2D textures2D[]; // TODO: make this a List
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace bGfx
{
	extern bool init();
	extern bool free();

	extern bool shader_compile( Shader &shader, const struct DiskShader &diskShader );
	extern void shader_free( Shader &shader );

	extern void rb_shader_bind( Shader &shader );
	extern int rb_shader_get_uniform( Shader &shader, const char *name );
	extern void rb_shader_set_uniform_matrix( const int location, const Matrix &matrix );

	extern void state_reset();
	extern void state_update();

	extern void viewport_set_size( const i32 width, const i32 height, const bool fullscreen );
	extern void swapchain_resize( const i32 width, const i32 height, const bool fullscreen );

	extern bool rb_texture2d_load( GfxTexture2D &texture, const void *pixels, const u16 width, const u16 height );
	extern void rb_texture2d_free( GfxTexture2D &texture );
	extern void rb_texture2d_bind( int slot, const GfxTexture2D &texture );
	extern void rb_texture2d_unbind( int slot );

	extern void rb_set_matrix_mvp( const Matrix &matrix );
	extern void rb_set_filtering_mode( GfxFilteringMode mode );
	extern void rb_set_depthtest_mode( GfxDepthTestMode mode );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Gfx
{
	extern void frame_begin();
	extern void frame_end();

	extern void clear_color( Color color );
	extern void clear_depth();

	extern void shader_bind( const u32 shader );
	extern void shader_release();

	extern void viewport_set_size( const i32 width, const i32 height, const bool fullscreen );
	extern void viewport_update();

	extern void texture2d_bind( const GfxTexture2D &texture );

	extern void set_matrix_mvp( const Matrix &matrix );
	extern void set_filtering_mode( GfxFilteringMode mode );
	extern void set_depthtest_mode( GfxDepthTestMode mode );

	////////////////////////////////////////////////////////////////////////////////////////////////////

	extern void quad_batch_build_indices( u16 *buffer );
	extern void quad_batch_begin();
	extern void quad_batch_end();
	extern void quad_batch_break();

	static inline void quad_batch_break_check()
	{
		if( UNLIKELY( bGfx::quadBatchPointer - bGfx::quadBatchStart >= RENDER_QUAD_BATCH_SIZE ) )
		{
			Gfx::quad_batch_break();
		}
	}

	extern void quad_batch_write( const float x1, const float y1, const float x2, const float y2,
	                              const u16 u1, const u16 v1, const u16 u2, const u16 v2,
	                              const Color c1, const Color c2, const Color c3, const Color c4, const GfxTexture2D *texture, const float depth = 0.0f );

	extern void quad_batch_write( const float x1, const float y1, const float x2, const float y2, const float x3, const float y3, const float x4, const float y4,
	                              const u16 u1, const u16 v1, const u16 u2, const u16 v2,
	                              const Color c1, const Color c2, const Color c3, const Color c4, const GfxTexture2D *texture = nullptr, const float depth = 0.0f );

	extern void quad_batch_write( const float x1, const float y1, const float x2, const float y2,
	                              const u16 u1, const u16 v1, const u16 u2, const u16 v2,
	                              const Color color, const GfxTexture2D *texture = nullptr, const float depth = 0.0f );

	extern void quad_batch_write( const float x1, const float y1, const float x2, const float y2, const float x3, const float y3, const float x4, const float y4,
	                              const u16 u1, const u16 v1, const u16 u2, const u16 v2,
	                              const Color color, const GfxTexture2D *texture = nullptr, const float depth = 0.0f );
}