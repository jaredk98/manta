#include <manta/gfx.hpp>

#include <vendor/vendor.hpp>

#include <config.hpp>
#include <pipeline.generated.hpp>

#include <manta/memory.hpp>
#include <manta/window.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace bGfx
{
	GfxState states[2];
	bool flip = false;
	bool rendering = false;

	GfxTexture2D textures[Assets::texturesCount];
	GfxShader shaders[Gfx::shadersCount];

	GfxSwapChain swapchain;
	GfxViewport viewport;

	Matrix matrixModel;
	Matrix matrixView;
	Matrix matrixPerspective;
	Matrix matrixMVP;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if PROFILING_GFX
#include <manta/draw.hpp>

namespace Gfx
{
	GfxStatistics stats = { };
	GfxStatistics statsPrevious = { };
}


static void format_integer( char *buffer, usize size, int num )
{
	char tempBuffer[32];
	int length, i, j = 0;

	snprintf( tempBuffer, sizeof( tempBuffer ), "%d", num < 0 ? -num : num );
	length = strlen( tempBuffer );

	// Negative
	if( num < 0 )
	{
		buffer[0] = '-';
		j = 1;
	}

	// Commas
	for( i = 0; i < length; i++ )
	{
		buffer[j++] = tempBuffer[i];

		if( ( length - i - 1 ) % 3 == 0 && i != length - 1 && j < size - 1 )
		{
			buffer[j++] = ',';
		}

		if( j >= size - 1 )
		{
			buffer[j] = '\0';
			return;
		}
	}

	buffer[j] = '\0';
}


void debug_overlay_gfx( const float x, const float y )
{
	float drawX = x;
	float drawY = y;
	const GfxStatistics &stats = Gfx::statsPrevious;

	char buffer[32];

	draw_text_f( fnt_consolas, 14, drawX, drawY, c_white, "Draw Calls: %d", stats.frame.drawCalls );
	drawY += 20.0f;

	format_integer( buffer, sizeof( buffer ), stats.frame.vertexCount );
	draw_text_f( fnt_consolas, 14, drawX, drawY, c_white, "Vertices: %s", buffer );
	drawY += 20.0f;

	format_integer( buffer, sizeof( buffer ), stats.frame.vertexCount / 3 );
	draw_text_f( fnt_consolas, 14, drawX, drawY, c_white, "Triangles: %s", buffer );
	drawY += 20.0f;

	draw_text_f( fnt_consolas, 14, drawX, drawY, c_white, "Texture Binds: %d", stats.frame.textureBinds );
	drawY += 20.0f;
	draw_text_f( fnt_consolas, 14, drawX, drawY, c_white, "Shader Binds: %d", stats.frame.shaderBinds );
	drawY += 20.0f;

	drawY += 20.0f;
	draw_text_f( fnt_consolas, 14, drawX, drawY, c_yellow, "GPU Memory Total: %.2f mb", MB( stats.total_memory() ) );
	drawY += 20.0f;
	draw_text_f( fnt_consolas, 14, drawX, drawY, c_white, "  Framebuffer: %.2f mb", MB( stats.gpuMemoryFramebuffer ) );
	drawY += 20.0f;
	draw_text_f( fnt_consolas, 14, drawX, drawY, c_white, "  Textures: %.2f mb", MB( stats.gpuMemoryTextures ) );
	drawY += 20.0f;
	draw_text_f( fnt_consolas, 14, drawX, drawY, c_white, "  Vertex Buffers: %.2f mb", MB( stats.gpuMemoryVertexBuffers ) );
	drawY += 20.0f;
	draw_text_f( fnt_consolas, 14, drawX, drawY, c_white, "  Index Buffers: %.2f mb", MB( stats.gpuMemoryIndexBuffers ) );
	drawY += 20.0f;
	draw_text_f( fnt_consolas, 14, drawX, drawY, c_white, "  Constant Buffers: %.2f mb", MB( stats.gpuMemoryConstantBuffers ) );
	drawY += 20.0f;
	draw_text_f( fnt_consolas, 14, drawX, drawY, c_white, "  Shader Programs: %.2f mb", MB( stats.gpuMemoryShaderPrograms ) );
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static inline void draw_call()
{
	// TODO: Refactor this
	Gfx::quad_batch_break(); // Break the quad batch
	// TODO ... break any other batch?
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool fGfx::init()
{
	// Initialize Backend
	ErrorIf( !bGfx::rb_init(), "%s: Failed to initialize %s backend!", __FUNCTION__, BUILD_GRAPHICS );

	// Initialize Textures
	ErrorIf( !fGfx::init_textures(), "%s: Failed to initialize textures!", __FUNCTION__ );

	// Initialize Shaders
	ErrorIf( !fGfx::init_shaders(), "%s: Failed to initialize shaders!", __FUNCTION__ );

	// Initialize Constant Buffers
 	ErrorIf( !bGfx::rb_init_cbuffers(), "%s: Failed to initialize constant buffers!", __FUNCTION__ );

	// Initialize Quad Batch
	ErrorIf( !fGfx::quad_batch_init(), "%s: Failed to initialize quad batch!", __FUNCTION__ );

	// Success
	return true;
}

bool fGfx::free()
{
	// Free Quad Batch
	ErrorIf( !fGfx::quad_batch_free(), "%s: Failed to free quad batch!", __FUNCTION__ );

	// Free Constant Buffers
	ErrorIf( !bGfx::rb_free_cbuffers(), "%s: Failed to free shaders!", __FUNCTION__ );

	// Free Shaders
	ErrorIf( !fGfx::free_shaders(), "%s: Failed to free shaders!", __FUNCTION__ );

	// Free Textures
	ErrorIf( !fGfx::free_textures(), "%s: Failed to free textures!", __FUNCTION__ );

	// Free Backend
	ErrorIf( !bGfx::rb_free(), "%s: Failed to free %s backend!", __FUNCTION__, BUILD_GRAPHICS );

	// Success
	return true;
}


bool fGfx::init_textures()
{
	// Load Textures
	for( u32 i = 0; i < Assets::texturesCount; i++ )
	{
		const DiskTexture &diskTexture = Assets::textures[i];
		bGfx::textures[i].init( Assets::binary.data + diskTexture.offset, diskTexture.width, diskTexture.height, GfxColorFormat_R8G8B8A8 );
	}

	// Success
	return true;
}


bool fGfx::free_textures()
{
	// Free Textures
	for( u32 i = 0; i < Assets::texturesCount; i++ )
	{
		bGfx::textures[i].free();
	}

	// Success
	return true;
}


bool fGfx::init_shaders()
{
	// Load Shaders
	for( u32 i = 0; i < Gfx::shadersCount; i++ )
	{
		const DiskShader &diskShader = Gfx::diskShaders[i];
		bGfx::shaders[i].init( i, diskShader );
	}

	// Success
	return true;
}


bool fGfx::free_shaders()
{
	// Free Shaders
	for( u32 i = 0; i < Gfx::shadersCount; i++ )
	{
		bGfx::shaders[i].free();
	}

	// Success
	return true;
}


void fGfx::state_reset()
{
	bGfx::states[0] = { };
	bGfx::states[1] = { };
	bGfx::flip = false;

	// Default Shader (TODO: Refactor?)
	Gfx::shader_bind( SHADER_DEFAULT );

	// Update State
	fGfx::state_apply( true );
}


void fGfx::state_apply( const bool dirty )
{
	#define GFX_STATE_CHECK(condition) UNLIKELY( ( condition ) )

	// This function runs at every draw call
	// It is responsible for making sure that the GPU render state reflects the CPU render state
	GfxState &current = bGfx::states[bGfx::flip];
	GfxState &previous = bGfx::states[!bGfx::flip];

	// Raster State
	if( dirty || GFX_STATE_CHECK( current.raster != previous.raster ) )
		{ bGfx::rb_set_raster_state( current.raster ); }

	// Sampler State
	if( dirty || GFX_STATE_CHECK( current.sampler != previous.sampler ) )
		{ bGfx::rb_set_sampler_state( current.sampler ); }

	// Blend State
	if( dirty || GFX_STATE_CHECK( current.blend != previous.blend ) )
		{ bGfx::rb_set_blend_state( current.blend ); }

	// Depth State
	if( dirty || GFX_STATE_CHECK( current.depth != previous.depth ) )
		{ bGfx::rb_set_depth_state( current.depth ); }

	// Texture 2D Binding
	if( GFX_STATE_CHECK( current.textureResource[0] != previous.textureResource[0] && current.textureResource[0] != nullptr ) )
		{ bGfx::rb_texture_2d_bind( current.textureResource[0], 0 ); }

	// Cache State
	previous = current;
	bGfx::flip = !bGfx::flip;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace fGfx
{
	GfxIndexBuffer quadBatchIndexBuffer;
	GfxVertexBuffer<GfxVertex::BuiltinVertex> quadBatchVertexBuffer;
}


bool fGfx::quad_batch_init()
{
	// Init Vertex Buffer
	quadBatchVertexBuffer.init( RENDER_QUAD_BATCH_SIZE * 6, GfxCPUAccessMode_WRITE_DISCARD );

	// Init Index Buffer
	const usize size = RENDER_QUAD_BATCH_SIZE * 6 * sizeof( u16 );
	u16 *indices = reinterpret_cast<u16 *>( memory_alloc( size ) );
	ErrorReturnIf( indices == nullptr, false, "%s: Failed to allocate memory for quad batch index buffer!", __FUNCTION__ );

	int i = 0;
	int j = 0;
	do
	{
		indices[j + 0] = i;
		indices[j + 1] = i + 1;
		indices[j + 2] = i + 2;
		indices[j + 3] = i + 3;
		indices[j + 4] = i + 2;
		indices[j + 5] = i + 1;

		i += 4;
		j += 6;
	}
	while ( j < RENDER_QUAD_BATCH_SIZE * 6 );

	quadBatchIndexBuffer.init( indices, size, 6.0f / 4.0f, GfxIndexBufferFormat_U16 );

	// Success
	return true;
}


bool fGfx::quad_batch_free()
{
	return true;
}


void fGfx::quad_batch_begin()
{
	quadBatchVertexBuffer.write_begin();
}


void fGfx::quad_batch_end()
{
	quadBatchVertexBuffer.write_end();
	quadBatchVertexBuffer.draw( quadBatchIndexBuffer );
}


void Gfx::quad_batch_break()
{
	if( fGfx::quadBatchVertexBuffer.current() > 0 )
	{
		fGfx::quad_batch_end();
		fGfx::quad_batch_begin();
	}
}


bool Gfx::quad_batch_can_break()
{
	return UNLIKELY( fGfx::quadBatchVertexBuffer.current() >= RENDER_QUAD_BATCH_SIZE * sizeof( GfxBuiltInQuad ) );
}


void Gfx::quad_batch_break_check()
{
	if( UNLIKELY( quad_batch_can_break() ) )
	{
		Gfx::quad_batch_break();
	}
}


void Gfx::quad_batch_write( const GfxBuiltInQuad &quad, const GfxTexture2D *const texture )
{
	// Bind Texture
	if( LIKELY( texture != nullptr ) ) { texture->bind( 0 ); }

	// Break Batch
	Gfx::quad_batch_break_check();

	// Write Quad
	fGfx::quadBatchVertexBuffer.write( quad );
}


void Gfx::quad_batch_write( const float x1, const float y1, const float x2, const float y2, const u16 u1, const u16 v1, const u16 u2, const u16 v2,
                            const Color c1, const Color c2, const Color c3, const Color c4, const GfxTexture2D *const texture, const float depth )
{
	// Bind Texture
	if( LIKELY( texture != nullptr ) ) { texture->bind( 0 ); }

	// Break Batch
	Gfx::quad_batch_break_check();

	// Write Quad
	const GfxBuiltInQuad quad =
	{
		{ { x1, y1, depth }, { u1, v1 }, { c1.r, c1.g, c1.b, c1.a } },
		{ { x2, y1, depth }, { u2, v1 }, { c2.r, c2.g, c2.b, c2.a } },
		{ { x1, y2, depth }, { u1, v2 }, { c3.r, c3.g, c3.b, c3.a } },
		{ { x2, y2, depth }, { u2, v2 }, { c4.r, c4.g, c4.b, c4.a } },
	};

	fGfx::quadBatchVertexBuffer.write( quad );
}


void Gfx::quad_batch_write( const float x1, const float y1, const float x2, const float y2, const float x3, const float y3, const float x4, const float y4,
                            const u16 u1, const u16 v1, const u16 u2, const u16 v2, const Color c1, const Color c2, const Color c3, const Color c4,
							const GfxTexture2D *const texture, const float depth )
{
	// Bind Texture
	if( LIKELY( texture != nullptr ) ) { texture->bind( 0 ); }

	// Break Batch
	Gfx::quad_batch_break_check();

	// Write Quad
	const GfxBuiltInQuad quad =
	{
		{ { x1, y1, depth }, { u1, v1 }, { c1.r, c1.g, c1.b, c1.a } },
		{ { x2, y2, depth }, { u2, v1 }, { c2.r, c2.g, c2.b, c2.a } },
		{ { x3, y3, depth }, { u1, v2 }, { c3.r, c3.g, c3.b, c3.a } },
		{ { x4, y4, depth }, { u2, v2 }, { c4.r, c4.g, c4.b, c4.a } },
	};

	fGfx::quadBatchVertexBuffer.write( quad );
}


void Gfx::quad_batch_write( const float x1, const float y1, const float x2, const float y2, const u16 u1, const u16 v1, const u16 u2, const u16 v2,
                            const Color color, const GfxTexture2D *const texture, const float depth )
{
	// Bind Texture
	if( LIKELY( texture != nullptr ) ) { texture->bind( 0 ); }

	// Break Batch
	Gfx::quad_batch_break_check();

	// Write Quad
	const GfxBuiltInQuad quad =
	{
		{ { x1, y1, depth }, { u1, v1 }, { color.r, color.g, color.b, color.a } },
		{ { x2, y1, depth }, { u2, v1 }, { color.r, color.g, color.b, color.a } },
		{ { x1, y2, depth }, { u1, v2 }, { color.r, color.g, color.b, color.a } },
		{ { x2, y2, depth }, { u2, v2 }, { color.r, color.g, color.b, color.a } },
	};

	fGfx::quadBatchVertexBuffer.write( quad );
}


void Gfx::quad_batch_write( const float x1, const float y1, const float x2, const float y2, const float x3, const float y3, const float x4, const float y4,
                            const u16 u1, const u16 v1, const u16 u2, const u16 v2, const Color color, const GfxTexture2D *const texture, const float depth )
{
	// Bind Texture
	if( LIKELY( texture != nullptr ) ) { texture->bind( 0 ); }

	// Break Batch
	quad_batch_break_check();

	// Write Quad
	const GfxBuiltInQuad quad =
	{
		{ { x1, y1, depth }, { u1, v1 }, { color.r, color.g, color.b, color.a } },
		{ { x2, y2, depth }, { u2, v1 }, { color.r, color.g, color.b, color.a } },
		{ { x3, y3, depth }, { u1, v2 }, { color.r, color.g, color.b, color.a } },
		{ { x4, y4, depth }, { u2, v2 }, { color.r, color.g, color.b, color.a } },
	};

	fGfx::quadBatchVertexBuffer.write( quad );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GfxTexture2D::init( void *data, const u16 width, const u16 height, const GfxColorFormat &format )
{
	ErrorIf( !bGfx::rb_texture_2d_init( resource, data, width, height, format ), "Failed to init Texture2D!" );
}


void GfxTexture2D::free()
{
	if( resource == nullptr ) { return; }
	ErrorIf( !bGfx::rb_texture_2d_free( resource ), "Failed to free Texture2D!" );
}


void GfxTexture2D::bind( const int slot ) const
{
	// Texture binding forces a batch break
	if( Gfx::state().textureResource[slot] == resource ) { return; }
	draw_call();

	Gfx::state().textureResource[slot] = resource;
	ErrorIf( !bGfx::rb_texture_2d_bind( resource, slot ), "Failed to bind Texture2D to slot %d!", slot );
}


void GfxTexture2D::release() const
{
	// ...
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool RENDER_TARGET_BOUND = false;
static Matrix RT_CACHE_MATRIX_MODEL;
static Matrix RT_CACHE_MATRIX_VIEW;
static Matrix RT_CACHE_MATRIX_PERSPECTIVE;
static GfxViewport RT_CACHE_VIEWPORT;
static GfxBlendState RT_CACHE_BLEND_STATE;


void GfxRenderTarget2D::init( const u16 width, const u16 height, const GfxRenderTargetDescription &desc )
{
	ErrorIf( !bGfx::rb_render_target_2d_init( resource, textureColor.resource, textureDepth.resource, width, height, desc ),
	         "Failed to init RenderTarget2D!" );

	this->width = width;
	this->height = height;
}


void GfxRenderTarget2D::free()
{
	if( resource == nullptr ) { return; }
	ErrorIf( !bGfx::rb_render_target_2d_free( resource, textureColor.resource, textureDepth.resource ),
	         "Failed to free RenderTarget2D!" );
}


void GfxRenderTarget2D::bind( const int slot ) const
{
	AssertMsg( !RENDER_TARGET_BOUND, "Trying to bind render target to slot that is already bound!" );
	RENDER_TARGET_BOUND = true;

	// Render target binding forces draw call
	draw_call();

	// Backend
	ErrorIf( !bGfx::rb_render_target_2d_bind( resource, 0 ), "Failed to bind RenderTarget2D!" );

	// MVP Matrix
	RT_CACHE_MATRIX_MODEL = Gfx::get_matrix_model();
	Gfx::set_matrix_model( matrix_build_identity() );
	RT_CACHE_MATRIX_VIEW = Gfx::get_matrix_view();
	Gfx::set_matrix_view( matrix_build_identity() );
	RT_CACHE_MATRIX_PERSPECTIVE = Gfx::get_matrix_perspective();
	Gfx::set_matrix_perspective( matrix_build_orthographic( 0.0f, width, 0.0f, height, 0.0f, 1.0f ) );

	// Viewport
	RT_CACHE_VIEWPORT = bGfx::viewport;
	Gfx::set_viewport_size( width, height, false );

	// D3D11 needs to explicitly update depth test state here
	bGfx::rb_set_depth_state( Gfx::state().depth );

	// Blend State
	RT_CACHE_BLEND_STATE = Gfx::state().blend;
	Gfx::set_blend_enabled( true );
	Gfx::set_blend_mode_color( GfxBlendFactor_ONE, GfxBlendFactor_INV_SRC_ALPHA, GfxBlendOperation_ADD );
	Gfx::set_blend_mode_alpha( GfxBlendFactor_ONE, GfxBlendFactor_INV_SRC_ALPHA, GfxBlendOperation_ADD );

	// Apply State
	fGfx::state_apply();
}


void GfxRenderTarget2D::release() const
{
	AssertMsg( RENDER_TARGET_BOUND, "Trying to release render target when it is not bound!" );
	RENDER_TARGET_BOUND = false;

	// Render target releasing forces draw call
	draw_call();

	// Backend
	ErrorIf( !bGfx::rb_render_target_2d_release(), "Failed to release RenderTarget2D!" );

	// MVP Matrix
	Gfx::set_matrix_model( RT_CACHE_MATRIX_MODEL );
	Gfx::set_matrix_view( RT_CACHE_MATRIX_VIEW );
	Gfx::set_matrix_perspective( RT_CACHE_MATRIX_PERSPECTIVE );

	// Viewport
	Gfx::set_viewport_size( RT_CACHE_VIEWPORT.width, RT_CACHE_VIEWPORT.height, RT_CACHE_VIEWPORT.fullscreen );

	// Blend State
	Gfx::set_blend_state( RT_CACHE_BLEND_STATE );

	// Apply State
	fGfx::state_apply();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GfxShader::init( const u32 shaderID, const DiskShader &diskShader )
{
	this->shaderID = shaderID;
	ErrorIf( !bGfx::rb_shader_init( resource, shaderID, diskShader ), "Failed to init shader! (%u)", this->shaderID );
}


void GfxShader::free()
{
	ErrorIf( !bGfx::rb_shader_free( resource ), "Failed to free shader! (%u)", this->shaderID );
}


void GfxShader::bind()
{
	if( Gfx::state().shader.resource != resource )
	{
		draw_call(); // Shader changes force batch break
		Gfx::state().shader.resource = resource;
	}

	ErrorIf( !bGfx::rb_shader_bind( resource ), "Failed to bind shader!" );
	ErrorIf( !bGfx::rb_shader_bind_constant_buffers_vertex[shaderID](), "Failed to bind vertex shader cbuffers! (%u)", this->shaderID );
	ErrorIf( !bGfx::rb_shader_bind_constant_buffers_fragment[shaderID](), "Failed to bind fragment shader cbuffers! (%u)", this->shaderID );
	ErrorIf( !bGfx::rb_shader_bind_constant_buffers_compute[shaderID](), "Failed to bind compute shader cbuffers! (%u)", this->shaderID );
}


void GfxShader::release()
{
	bGfx::shaders[SHADER_DEFAULT].bind();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Gfx::viewport_update()
{
	// Update Viewport
	const i32 viewportWidth = static_cast<i32>( Window::width * Window::scale );
	const i32 viewportHeight = static_cast<i32>( Window::height * Window::scale );
	bGfx::rb_viewport_resize( viewportWidth, viewportHeight, Window::fullscreen );

	// Resize Swapchain
	bGfx::rb_swapchain_resize( Window::width, Window::height, Window::fullscreen );
}


void Gfx::frame_begin()
{
	// Reset State
	fGfx::state_reset();

	// Backend
	bGfx::rb_frame_begin();

	// Quad Batch
	fGfx::quad_batch_begin();

	// Reset Matrices
	const Matrix identity = matrix_build_identity();
	Gfx::set_matrix_model( identity );
	Gfx::set_matrix_view( identity );
	Gfx::set_matrix_perspective( identity );

	// Start Rendering
	bGfx::rendering = true;
}


void Gfx::frame_end()
{
	// Stop Rendering
	bGfx::rendering = false;

	// Quad Batch
	fGfx::quad_batch_end();

	// Backend
	bGfx::rb_frame_end();

	// Statistics
#if PROFILING_GFX
	Gfx::statsPrevious = Gfx::stats;
	Gfx::stats.frame = { };
#endif
}


void Gfx::clear_color( const Color color )
{
	bGfx::rb_clear_color( color );
}


void Gfx::clear_depth( const float depth )
{
	bGfx::rb_clear_depth( depth );
}


void Gfx::set_swapchain_size( const u16 width, const u16 height, const bool fullscreen )
{
	bGfx::rb_swapchain_resize( width, height, fullscreen );
}


void Gfx::set_viewport_size( const u16 width, const u16 height, const bool fullscreen )
{
	bGfx::rb_viewport_resize( width, height, fullscreen );
}


void Gfx::set_shader_globals( const bGfxCBuffer::ShaderGlobals_t &globals )
{
	// Shader globals changes force a batch break
	if( Gfx::state().shader.globals == globals ) { return; }
	Gfx::state().shader.globals = globals;

	draw_call();
	globals.upload();
}


static void update_matrix_mvp()
{
	bGfx::matrixMVP = matrix_multiply( bGfx::matrixPerspective, matrix_multiply( bGfx::matrixView, bGfx::matrixModel ) );

	auto &globals = GfxCBuffer::ShaderGlobals;
	globals.matrixModel = bGfx::matrixModel;
	globals.matrixView = bGfx::matrixView;
	globals.matrixPerspective = bGfx::matrixPerspective;
	globals.matrixMVP = bGfx::matrixMVP;

	Gfx::set_shader_globals( globals );
};


void Gfx::set_matrix_model( const Matrix &matrix )
{
	bGfx::matrixModel = matrix;
	update_matrix_mvp();
}


void Gfx::set_matrix_view( const Matrix &matrix )
{
	bGfx::matrixView = matrix;
	update_matrix_mvp();
}


void Gfx::set_matrix_perspective( const Matrix &matrix )
{
	bGfx::matrixPerspective = matrix;
	update_matrix_mvp();
}


void Gfx::set_matrix_mvp( const Matrix &matModel, const Matrix &matView, const Matrix &matPerspective )
{
	bGfx::matrixModel = matModel;
	bGfx::matrixView = matView;
	bGfx::matrixPerspective = matPerspective;
	update_matrix_mvp();
}


void Gfx::set_matrix_mvp_2d_orthographic( const float x, const float y, const float zoom, const float angle,
                                          const float width, const float height )
{
	bGfx::matrixModel = matrix_build_identity();
	Matrix matrixView = matrix_build_rotation_z( angle * DEG2RAD );
	matrixView = matrix_multiply( matrix_build_scaling( zoom, zoom, 1.0f ), matrixView );
	matrixView = matrix_multiply( matrix_build_translation( -x, -y, 0.0f ), matrixView );
	bGfx::matrixView = matrixView;
	bGfx::matrixPerspective = matrix_build_orthographic( 0.0f, width, 0.0f, height, 0.0f, 1.0f );
	update_matrix_mvp();
}


void Gfx::set_matrix_mvp_3d_perspective( const float fov, const float aspect, const float znear, const float zfar,
                                         const float x, const float y, const float z, const float xto, const float yto,
                                         const float zto, const float xup, const float yup, const float zup )
{
	AssertMsg( znear > 0.0f, "znear must be > 0.0f!" );
	bGfx::matrixModel = matrix_build_identity();
	bGfx::matrixView = matrix_build_lookat( x, y, z, xto, yto, zto, xup, yup, zup );
	bGfx::matrixPerspective = matrix_build_perspective( fov, aspect, znear, zfar );
	update_matrix_mvp();
}


const Matrix &Gfx::get_matrix_model()
{
	return bGfx::matrixModel;
}


const Matrix &Gfx::get_matrix_view()
{
	return bGfx::matrixView;
}


const Matrix &Gfx::get_matrix_perspective()
{
	return bGfx::matrixPerspective;
}


const Matrix &Gfx::get_matrix_mvp()
{
	return bGfx::matrixMVP;
}


void Gfx::set_raster_state( const GfxRasterState &state )
{
	// Raster state changes force a batch break
	if( Gfx::state().raster == state ) { return; }
	draw_call();
	Gfx::state().raster = state;
}


void Gfx::set_fill_mode( const GfxFillMode &mode )
{
	GfxRasterState state = Gfx::state().raster;
	state.fillMode = mode;
	Gfx::set_raster_state( state );
}


void Gfx::set_cull_mode( const GfxCullMode &mode )
{
	GfxRasterState state = Gfx::state().raster;
	state.cullMode = mode;
	Gfx::set_raster_state( state );
}


void Gfx::set_sampler_state( const GfxSamplerState &state )
{
	// Sampler state changes force a batch break
	if( Gfx::state().sampler == state ) { return; }
	draw_call();
	Gfx::state().sampler = state;
}


void Gfx::set_filtering_mode( const GfxFilteringMode &mode )
{
	GfxSamplerState state = Gfx::state().sampler;
	state.filterMode = mode;
	Gfx::set_sampler_state( state );
}


void Gfx::set_uv_wrap_mode( const GfxUVWrapMode &mode )
{
	GfxSamplerState state = Gfx::state().sampler;
	state.wrapMode = mode;
	Gfx::set_sampler_state( state );
}


void Gfx::set_blend_state( const GfxBlendState &state )
{
	// Blend state changes force a batch break
	if( Gfx::state().blend == state ) { return; }
	draw_call();
	Gfx::state().blend = state;
}


void Gfx::set_blend_enabled( const bool enabled )
{
	GfxBlendState state = Gfx::state().blend;
	state.blendEnable = enabled;
	Gfx::set_blend_state( state );
}


void Gfx::set_blend_mode_color( const GfxBlendFactor &srcFactor, const GfxBlendFactor &dstFactor, const GfxBlendOperation &op )
{
	GfxBlendState state = Gfx::state().blend;
	state.srcFactorColor = srcFactor;
	state.dstFactorColor = dstFactor;
	state.blendOperationColor = op;
	Gfx::set_blend_state( state );
}


void Gfx::set_blend_mode_alpha( const GfxBlendFactor &srcFactor, const GfxBlendFactor &dstFactor, const GfxBlendOperation &op )
{
	GfxBlendState state = Gfx::state().blend;
	state.srcFactorAlpha = srcFactor;
	state.dstFactorAlpha = dstFactor;
	state.blendOperationAlpha = op;
	Gfx::set_blend_state( state );
}


void Gfx::set_color_write_mask( const GfxColorWriteFlag &mask )
{
	GfxBlendState state = Gfx::state().blend;
	state.colorWriteMask = mask;
	Gfx::set_blend_state( state );
}


void Gfx::set_depth_state( const GfxDepthState &state )
{
	// Depth state changes force a batch break
	if( Gfx::state().depth == state ) { return; }
	draw_call();
	Gfx::state().depth = state;
}


void Gfx::set_depth_test_mode( const GfxDepthTestMode &mode )
{
	GfxDepthState state = Gfx::state().depth;
	state.depthTestMode = mode;
	Gfx::set_depth_state( state );
}


void Gfx::set_depth_write_mask( const GfxDepthWriteFlag &mask )
{
	GfxDepthState state = Gfx::state().depth;
	state.depthWriteMask = mask;
	Gfx::set_depth_state( state );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
