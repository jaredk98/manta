#include <manta/gfx.hpp>

#include <manta/window.hpp>

#include <debug.hpp>
#include <config.hpp>
#include <types.hpp>

#include <vendor/vendor.hpp>
#include <vendor/string.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace bGfx
{
	bool rendering = false;

	RenderState state;
	RenderState statePrevious;

	i32 viewportWidth = WINDOW_WIDTH_DEFAULT; // TODO: Refactor
	i32 viewportHeight = WINDOW_HEIGHT_DEFAULT;
	bool viewportFullscreen = false;

	GfxQuad<GfxVertex> *quadBatchStart = nullptr; // TODO: Refactor
	GfxQuad<GfxVertex> *quadBatchPointer = nullptr;
	GfxTexture2D *quadBatchTexture = nullptr;

	GfxTexture2D textures2D[::assets::texturesCount]; // TODO: Refactor
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void bGfx::state_reset()
{
	bGfx::state = { };
	bGfx::statePrevious = { };

	// Default Shader (TODO: Refactor)
	bGfx::rb_shader_bind( ::shaders::shaders[SHADER_DEFAULT] );
}

void bGfx::state_update()
{
	#define CHECK_STATE(condition) UNLIKELY( current.dirty || ( condition ) )

	// This function runs at every draw call
	// It is responsible for making sure that the GPU render state reflects the CPU render state
	RenderState &current = bGfx::state;
	RenderState &previous = bGfx::statePrevious;

	// Texture Bindings
	if( CHECK_STATE( current.texture2D != previous.texture2D && current.texture2D != nullptr ) )
		{ bGfx::rb_texture2d_bind( 0, *current.texture2D ); }

	// Texture Filtering Mode
	if( CHECK_STATE( current.filteringMode != previous.filteringMode ) )
		{ bGfx::rb_set_filtering_mode( current.filteringMode ); }

	// Depth Test Mode
	if( CHECK_STATE( current.depthTestMode != previous.depthTestMode ) )
		{ bGfx::rb_set_depthtest_mode( current.depthTestMode ); }

	// Cache RenderState
	previous = current;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Gfx::viewport_set_size( const i32 width, const i32 height, const bool fullscreen )
{
	bGfx::viewportWidth = width;
	bGfx::viewportHeight = height;
	bGfx::viewportFullscreen = fullscreen;
	bGfx::viewport_set_size( width, height, fullscreen );
	//render_state_dirty( RenderStateFlag_Viewport, true );
}


void Gfx::viewport_update()
{
	// Update Viewport
	const i32 viewportWidth = static_cast<i32>( window::width * window::scale );
	const i32 viewportHeight = static_cast<i32>( window::height * window::scale );
	viewport_set_size( viewportWidth, viewportHeight, window::fullscreen );

	// Resize Swapchain
	bGfx::swapchain_resize( window::width, window::height, window::fullscreen );

	// Update Scissor
	//render_reset_scissor_rectangle();

	// Update Abstracted Viewports
	//viewports_resize();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static inline void draw_call()
{
	// TODO: Refactor this
	Gfx::quad_batch_break(); // Break the quad batch
	// TODO ... break any other batch?
}


void Gfx::texture2d_bind( const GfxTexture2D &texture )
{
	// Texture binding forces a batch break
	if( bGfx::state.texture2D == &texture ) { return; }
	draw_call();

	bGfx::state.texture2D = &texture;
	bGfx::rb_texture2d_bind( 0, texture );
}


void Gfx::shader_bind( const u32 shader )
{
	// Shader binding forces a batch break
	if( bGfx::state.shader == shader ) { return; }
	draw_call();

	bGfx::state.shader = shader;
	bGfx::rb_shader_bind( shaders::shaders[shader] );
}


void Gfx::shader_release()
{
	if( bGfx::state.shader == SHADER_DEFAULT ) { return; }
	Gfx::shader_bind( SHADER_DEFAULT );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Gfx::set_matrix_mvp( const Matrix &matrix )
{
	if( matrix_equal( bGfx::state.matrixMVP, matrix ) ) { return; }
	draw_call();

	bGfx::state.matrixMVP = matrix;
	bGfx::rb_set_matrix_mvp( matrix );
}

void Gfx::set_filtering_mode( GfxFilteringMode mode )
{
	// Texture filtering mode changes force a batch break
	if( bGfx::state.filteringMode == mode ) { return; }
	draw_call();

	bGfx::state.filteringMode = mode;
}

void Gfx::set_depthtest_mode( GfxDepthTestMode mode )
{
	// Depth test mode changes force a batch break
	if( bGfx::state.depthTestMode == mode ) { return; }
	draw_call();

	bGfx::state.depthTestMode = mode;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Gfx::quad_batch_build_indices( u16 *buffer )
{
	int i = 0;
	int j = 0;

	do
	{
		buffer[j + 0] = i;
		buffer[j + 1] = i + 1;
		buffer[j + 2] = i + 2;
		buffer[j + 3] = i + 3;
		buffer[j + 4] = i + 2;
		buffer[j + 5] = i + 1;

		i += 4;
		j += 6;
	}
	while ( j < RENDER_QUAD_BATCH_SIZE * 6 );
}


void Gfx::quad_batch_break()
{
	if( bGfx::quadBatchPointer - bGfx::quadBatchStart > 0 )
	{
		quad_batch_end();
		quad_batch_begin();
	}
}


void Gfx::quad_batch_write( const float x1, const float y1, const float x2, const float y2,
                               const u16 u1, const u16 v1, const u16 u2, const u16 v2,
                               const Color c1, const Color c2, const Color c3, const Color c4, const GfxTexture2D *texture, const float depth )
{
	// Bind Texture
	if( LIKELY( texture != nullptr ) ) { Gfx::texture2d_bind( *texture ); }

	// Break Batch
	quad_batch_break_check();

	// Write Vertex Data
	*bGfx::quadBatchPointer++ =
	{
		{ x1, y1, depth, u1, v1, c1 },
		{ x2, y1, depth, u2, v1, c2 },
		{ x1, y2, depth, u1, v2, c3 },
		{ x2, y2, depth, u2, v2, c4 },
	};
	//DEBUG_COUNTER_INCREMENT( DebugCounter_Render_RenderedQuads )
}


void Gfx::quad_batch_write( const float x1, const float y1, const float x2, const float y2, const float x3, const float y3, const float x4, const float y4,
                               const u16 u1, const u16 v1, const u16 u2, const u16 v2,
                               const Color c1, const Color c2, const Color c3, const Color c4, const GfxTexture2D *texture, const float depth )
{
	// Bind Texture
	if( LIKELY( texture != nullptr ) ) { Gfx::texture2d_bind( *texture ); }

	// Break Batch
	quad_batch_break_check();

	// Write Vertex Data
	*bGfx::quadBatchPointer++ =
	{
		{ x1, y1, depth, u1, v1, c1 },
		{ x2, y2, depth, u2, v1, c2 },
		{ x3, y3, depth, u1, v2, c3 },
		{ x4, y4, depth, u2, v2, c4 },
	};
	//DEBUG_COUNTER_INCREMENT( DebugCounter_Render_RenderedQuads )
}


void Gfx::quad_batch_write( const float x1, const float y1, const float x2, const float y2,
                               const u16 u1, const u16 v1, const u16 u2, const u16 v2,
                               const Color color, const GfxTexture2D *texture, const float depth )
{
	// Bind Texture
	if( LIKELY( texture != nullptr ) ) { Gfx::texture2d_bind( *texture ); }

	// Break Batch
	quad_batch_break_check();

	// Write Vertex Data
	*bGfx::quadBatchPointer++ =
	{
		{ x1, y1, depth, u1, v1, color },
		{ x2, y1, depth, u2, v1, color },
		{ x1, y2, depth, u1, v2, color },
		{ x2, y2, depth, u2, v2, color },
	};
	//DEBUG_COUNTER_INCREMENT( DebugCounter_Render_RenderedQuads )
}


void Gfx::quad_batch_write( const float x1, const float y1, const float x2, const float y2, const float x3, const float y3, const float x4, const float y4,
                               const u16 u1, const u16 v1, const u16 u2, const u16 v2,
                               const Color color, const GfxTexture2D *texture, const float depth )
{
	// Bind Texture
	if( LIKELY( texture != nullptr ) ) { Gfx::texture2d_bind( *texture ); }

	// Break Batch
	quad_batch_break_check();

	// Write Vertex Data
	*bGfx::quadBatchPointer++ =
	{
		{ x1, y1, depth, u1, v1, color },
		{ x2, y2, depth, u2, v1, color },
		{ x3, y3, depth, u1, v2, color },
		{ x4, y4, depth, u2, v2, color },
	};
	//DEBUG_COUNTER_INCREMENT( DebugCounter_Render_RenderedQuads )
}