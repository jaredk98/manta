#include <manta/gfx.hpp>

#include <manta/backend/gfx/opengl/opengl.hpp>

#include <manta/window.hpp>

#include <config.hpp>
#include <debug.hpp>
#include <types.hpp>

// TODO: remove
#include <manta/fileio.hpp>
#include <manta/memory.hpp>
#include <manta/string.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct GlTexImage2DFormat
{
	GlTexImage2DFormat( const GLenum format, const GLenum formatInternal, const GLenum formatType ) :
		format{ format }, formatInternal{ formatInternal }, formatType{ formatType } { }

	GLenum format, formatInternal, formatType;
};


static const GlTexImage2DFormat GlColorFormats[] =
{
	{ GL_RGBA, GL_RGBA8,    GL_UNSIGNED_BYTE },           // GfxColorFormat_NONE
	{ GL_RGBA, GL_RGBA8,    GL_UNSIGNED_BYTE },           // GfxColorFormat_R8G8B8A8
	{ GL_RGBA, GL_RGB10_A2, GL_UNSIGNED_INT_10_10_10_2 }, // GfxColorFormat_R10G10B10A2
	{ GL_RED,  GL_R8,       GL_UNSIGNED_BYTE },           // GfxColorFormat_R8
	{ GL_RG,   GL_RG8,      GL_UNSIGNED_BYTE },           // GfxColorFormat_R8G8
	{ GL_RED,  GL_R16F,     GL_UNSIGNED_SHORT },          // GfxColorFormat_R16
	{ GL_RG,   GL_RG16F,    GL_UNSIGNED_SHORT },          // GfxColorFormat_R16G16
	{ GL_RED,  GL_R32F,     GL_FLOAT },                   // GfxColorFormat_R32
};
static_assert( ARRAY_LENGTH( GlColorFormats ) == GFXCOLORFORMAT_COUNT, "Missing GfxColorFormat!" );


static const GlTexImage2DFormat GlDepthFormats[] =
{
	{ 0, 0, 0 },                                                        // GfxDepthFormat_NONE
	{ GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT16, GL_FLOAT },             // GfxDepthFormat_R16_FLOAT
	{ GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT16, GL_UNSIGNED_SHORT },    // GfxDepthFormat_R16_UINT
	{ GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT32F, GL_FLOAT },            // GfxDepthFormat_R32_FLOAT
	{ GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT32, GL_UNSIGNED_INT },      // GfxDepthFormat_R32_UINT
	{ GL_DEPTH_STENCIL,   GL_DEPTH24_STENCIL8,  GL_UNSIGNED_INT_24_8 }, // GfxDepthFormat_R24_UINT_G8_UINT
};
static_assert( ARRAY_LENGTH( GlDepthFormats ) == GFXDEPTHFORMAT_COUNT, "Missing GfxDepthFormat!" );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static GLuint batch_vao;
static GLuint batch_vbo;
static GLuint batch_ebo;

static Matrix MATRIX_PROJECTION_PREVIOUS;
static int DEPTH_TEST_MODE_PREVIOUS;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool bGfx::init()
{
	// Init Shader State
	//render_init_shaders();

	// Initialize OpenGL Context
	if( !opengl_init() )
	{
		ErrorReturnMsg( false, "%s: Failed to init OpenGL", __FUNCTION__ );
	}

	// Setup Blend State
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	// Create Objects
	nglGenVertexArrays( 1, &batch_vao );
	nglGenBuffers( 1, &batch_vbo );
	nglGenBuffers( 1, &batch_ebo );

	// Bind Objects
	nglBindVertexArray( batch_vao );
	nglBindBuffer( GL_ARRAY_BUFFER, batch_vbo );
	nglBindBuffer( GL_ELEMENT_ARRAY_BUFFER, batch_ebo );

	// Setup Vertex Buffer
	nglBufferData( GL_ARRAY_BUFFER, RENDER_QUAD_BATCH_SIZE * sizeof( GfxQuad<GfxVertex> ), nullptr, GL_DYNAMIC_DRAW );

	// Setup Vertex Format - TODO (jkrahn): Support multiple vertex formats?
	nglVertexAttribPointer( 0, 3, GL_FLOAT, false, sizeof( GfxVertex ), reinterpret_cast<void *>( 0 ) );
	nglVertexAttribPointer( 1, 2, GL_UNSIGNED_SHORT, true, sizeof( GfxVertex ), reinterpret_cast<void *>( 12 ) );
	nglVertexAttribPointer( 2, 4, GL_UNSIGNED_BYTE, true, sizeof( GfxVertex ), reinterpret_cast<void *>( 16 ) );

	nglEnableVertexAttribArray( 0 );
	nglEnableVertexAttribArray( 1 );
	nglEnableVertexAttribArray( 2 );

	// Setup Index Buffer
	u16 indices[RENDER_QUAD_BATCH_SIZE * 6];
	::Gfx::quad_batch_build_indices( indices );

	// Setup Index Buffer
	nglBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW );

	// Setup Viewport
	glViewport( 0, 0, window::width, window::height );

	// Init Shaders
	if( !shaders::init() )
	{
		ErrorReturnMsg( false, "%s: Failed to init shaders", __FUNCTION__ );
	}

	// Load textures (TODO: Refactor)
	for( u16 i = 0; i < assets::texturesCount; i++ )
	{
		GfxTexture2D &texture = bGfx::textures2D[i];
		const DiskTexture &diskTexture = assets::textures[i];

		if( !rb_texture2d_load( texture, assets::binary.data + diskTexture.offset, diskTexture.width, diskTexture.height ) )
		{
			ErrorReturnMsg( false, "%s: Failed to load Texture2D %d (%dx%d)", __FUNCTION__, i, diskTexture.width, diskTexture.height );
		}
	}

	// Sucess
	return true;
}


bool bGfx::free()
{
	// Success
	return true;
}


bool bGfx::shader_compile( Shader &shader, const DiskShader &diskShader )
{
	char info[1024];
	int status;

	// Create Shaders
	shader.shaderVertex = nglCreateShader( GL_VERTEX_SHADER );
	shader.shaderFragment =  nglCreateShader( GL_FRAGMENT_SHADER );

	// Source Shaders
	const byte *codeVertex = assets::binary.data + diskShader.offsetVertex;
	nglShaderSource( shader.shaderVertex, 1, reinterpret_cast<const GLchar **>( &codeVertex ), reinterpret_cast<const GLint *>( &diskShader.sizeVertex ) );
	const byte *codeFragment = assets::binary.data + diskShader.offsetFragment;
	nglShaderSource( shader.shaderFragment, 1, reinterpret_cast<const GLchar **>( &codeFragment ), reinterpret_cast<const GLint *>( &diskShader.sizeFragment ) );
	// TODO... compute shaders

	// Compile Shaders
	nglCompileShader( shader.shaderVertex );
	nglCompileShader( shader.shaderFragment );
	// TODO... compute shaders

	// Check Vertex Shader
	if( nglGetShaderiv( shader.shaderVertex, GL_COMPILE_STATUS, &status ), !status )
	{
	#if COMPILE_DEBUG
		// TODO: Shader names in error messages
		nglGetShaderInfoLog( shader.shaderVertex, sizeof( info ), nullptr, info );
		ErrorReturnMsg( false, "%s: Failed to get vertex shader info log (OpenGL)\nVertex shader error: %s", __FUNCTION__, info );
	#endif
	}

	// Check Fragment Shader
	if( nglGetShaderiv( shader.shaderFragment, GL_COMPILE_STATUS, &status ), !status )
	{
	#if COMPILE_DEBUG
		// TODO: Shader names in error messages
		nglGetShaderInfoLog( shader.shaderFragment, sizeof( info ), nullptr, info );
		ErrorReturnMsg( false, "%s: Failed to get fragment shader info log (OpenGL)\nFragment shader error: %s", __FUNCTION__, info );
	#endif
	}

	// Create Program
	shader.program = nglCreateProgram();
	nglAttachShader( shader.program, shader.shaderVertex );
	nglAttachShader( shader.program, shader.shaderFragment );
	// TODO... compute shaders

	// OpenGL 3.3 added explicit "layout" directives so that shaders can
	// specify the index of vertex attributes, but on OpenGL 3.2 and lower
	// we need to bind the variables by name to a specific index.
	nglBindAttribLocation( shader.program, 0, "vertPosition" );
	nglBindAttribLocation( shader.program, 1, "vertUV" );
	nglBindAttribLocation( shader.program, 2, "vertColor" );

	// Link Program
	nglLinkProgram( shader.program );

	#if COMPILE_DEBUG
		// TODO: Check Link Status
		// ...
	#endif

	// Delete Shaders
	nglDeleteShader( shader.shaderVertex );
	nglDeleteShader( shader.shaderFragment );
	// TODO... compute shaders

	return true;
}


void bGfx::shader_free( Shader &shader )
{
	nglDeleteProgram( shader.program );
}


void bGfx::rb_shader_bind( Shader &shader )
{
	nglUseProgram( shader.program );
	bGfx::rb_set_matrix_mvp( bGfx::state.matrixMVP );
}


bool bGfx::rb_texture2d_load( GfxTexture2D &texture, const void *pixels, const u16 width, const u16 height )
{
	// Create Texture2D
	glGenTextures( 1, &texture.texture );
	GLenum error = glGetError();
	if( error != GL_NO_ERROR )
	{
		ErrorReturnMsg( false, "%s: Failed to generated texture (%d)", __FUNCTION__, error );
	}

	// Setup Texture2D Data (TODO: Switch to sampler objects)
	glBindTexture( GL_TEXTURE_2D, texture.texture );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels );
	glBindTexture( GL_TEXTURE_2D, 0 );

	// Success
	return true;
}


void bGfx::rb_texture2d_free( GfxTexture2D &texture )
{
	glDeleteTextures( 1, &texture.texture );
}


void bGfx::rb_texture2d_bind( int slot, const GfxTexture2D &texture )
{
	//AssertMsg( slot < ENGINE_RENDER_TEXTURE_SLOTS, "The engine only supports %d bound textures. Change this in config!", ENGINE_RENDER_TEXTURE_SLOTS );

	// Find texture location
	int location = -1;//g_RENDER_STATE.shader_LocationTexture[ g_RENDER_STATE.shader ][ slot ];
	if( location == -1 )
	{
		char buffer[16];
		location = rb_shader_get_uniform( ::shaders::shaders[SHADER_DEFAULT], "texture0" );
		//g_RENDER_STATE.shader_LocationTexture[ g_RENDER_STATE.shader ][ slot ] = location;
	}

	// Bind Texture2D
	nglUniform1i( location, slot );
	nglActiveTexture( GL_TEXTURE0 + slot );
	glBindTexture( GL_TEXTURE_2D, texture.texture );

	// OpenGL sets texture filtering on per-texture basis (TODO: Fix this?)
	bGfx::rb_set_filtering_mode( bGfx::state.filteringMode );

	//if( slot == 0 ) { g_RENDER_STATE.textureLast = &texture; }
}


void bGfx::rb_texture2d_unbind( int slot )
{
	// Do nothing on OpenGL
}


void bGfx::rb_set_filtering_mode( GfxFilteringMode mode )
{
	switch( mode )
	{
		case GfxFilteringMode_NEAREST:
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		return;

		case GfxFilteringMode_LINEAR:
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		return;

		case GfxFilteringMode_ANISOTROPIC:
			// TODO:
			// GLfloat maxAnisotropy;
			// glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy );
			// glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
			// glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			// glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy );
		return;
	}
}


void bGfx::rb_set_depthtest_mode( GfxDepthTestMode mode )
{
	// Enable depth testing if we need it
	if( mode != GfxDepthTestMode_NONE && !glIsEnabled( GL_DEPTH_TEST ) )
	{
		glEnable( GL_DEPTH_TEST );
	}

	switch( mode )
	{
		case GfxDepthTestMode_NONE:
			glDisable( GL_DEPTH_TEST );
		break;

		case GfxDepthTestMode_LESS:
			glDepthFunc( GL_LESS );
		break;

		case GfxDepthTestMode_LESS_EQUALS:
			glDepthFunc( GL_LEQUAL );
		break;

		case GfxDepthTestMode_ALWAYS:
			glDepthFunc( GL_ALWAYS );
		break;
	}
}


void bGfx::viewport_set_size( const i32 width, const i32 height, const bool fullscreen )
{
	glViewport( 0, 0, width, height );
}


void bGfx::swapchain_resize( const i32 width, const i32 height, const bool fullscreen )
{
	opengl_update();
}


int bGfx::rb_shader_get_uniform( Shader &shader, const char *name )
{
	return nglGetUniformLocation( shader.program, name ); // returns -1 if error
}


void bGfx::rb_shader_set_uniform_matrix( const int location, const Matrix &matrix )
{
	nglUniformMatrix4fv( static_cast<GLint>( location ), 1, false, matrix.data );
}


void bGfx::rb_set_matrix_mvp( const Matrix &matrix )
{
	// TODO - replace this with core GfxBuffer?
	int location = bGfx::rb_shader_get_uniform( ::shaders::shaders[SHADER_DEFAULT], "matrixMVP" );
	bGfx::rb_shader_set_uniform_matrix( location, matrix );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Gfx::frame_begin()
{
	// State
	bGfx::rendering = true;
	bGfx::state_reset();

	// Begin Batch
	quad_batch_begin();
}

void Gfx::frame_end()
{
	// Finish Batch
	quad_batch_end();

#if COMPILE_DEBUG
	// Check OpenGL Errors
	while( true )
	{
		int error = glGetError();
		if( error == GL_NO_ERROR ) { break; }
		Error( "OpenGL Error: %d\n", error );
	}
#endif

	// Swap Buffers
	{
		//PROFILE_SCOPE( "Swapchain" )
		const bool swap = opengl_swap();
		Assert( swap );
	}

	// Reset State
	bGfx::rendering = false;
}


void Gfx::clear_color( Color color )
{
	static constexpr float INV_255 = 1.0f / 255.0f; // Compiler likely does this regardless, but mul instruction is faster than div
	glClearColor( color.r * INV_255, color.g * INV_255, color.b * INV_255, color.a * INV_255 );
	glClear( GL_COLOR_BUFFER_BIT );
}


void Gfx::clear_depth()
{
	glDepthMask( true );
	glClear( GL_DEPTH_BUFFER_BIT );
}


void Gfx::quad_batch_begin()
{
	bGfx::quadBatchStart = reinterpret_cast<GfxQuad<GfxVertex> *>( nglMapBufferRange( GL_ARRAY_BUFFER, 0, RENDER_QUAD_BATCH_SIZE * sizeof( GfxQuad<GfxVertex> ), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT ) );
	bGfx::quadBatchPointer = bGfx::quadBatchStart;
}


void Gfx::quad_batch_end()
{
	// Render State
	bGfx::state_update();

	// Unmap Buffer
	nglUnmapBuffer( GL_ARRAY_BUFFER );

	// Submit Buffer
	if( bGfx::quadBatchStart != bGfx::quadBatchPointer )
	{
		GLsizei count = static_cast<GLsizei>( bGfx::quadBatchPointer - bGfx::quadBatchStart );
		glDrawElements( GL_TRIANGLES, count * 6, GL_UNSIGNED_SHORT, 0 ); // Draw Call
		//DEBUG_COUNTER_INCREMENT( DebugCounter_Render_DrawCalls )
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GfxSurface2D::init( const u16 width, const u16 height, const GfxSurface2DDescription &desc )
{
	// Create FBO
	GLuint fbo;
	nglGenFramebuffers( 1, &fbo );

	// Create Texture
	GLuint textureColor;
	glGenTextures( 1, &textureColor );
	glBindTexture( GL_TEXTURE_2D, textureColor );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	const GlTexImage2DFormat &colorFormat = GlColorFormats[desc.colorFormat];
	glTexImage2D( GL_TEXTURE_2D, 0, colorFormat.formatInternal, width, height, 0, colorFormat.format, colorFormat.formatType, nullptr );

	// Create Depth Texture
	GLuint textureDepth = 0;
	if( desc.depthFormat != GfxDepthFormat_NONE )
	{
		glGenTextures( 1, &textureDepth );
		glBindTexture( GL_TEXTURE_2D, textureDepth );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		const GlTexImage2DFormat &depthFormat = GlDepthFormats[desc.depthFormat];
		glTexImage2D( GL_TEXTURE_2D, 0, depthFormat.formatInternal, width, height, 0, depthFormat.format, depthFormat.formatType, nullptr );
	}

	// Bind Framebuffer
	nglBindFramebuffer( GL_FRAMEBUFFER, fbo );

	// Attach Color
	nglFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColor, 0 );

	// Attach Depth
	if( desc.depthFormat != GfxDepthFormat_NONE )
	{
		nglFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureDepth, 0 );
	}

	// Check Status
	ErrorIf( nglCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE, "%s: Failed to create GL framebuffer", __FUNCTION__ );

	// Unbind Framebuffer & Texture
	nglBindFramebuffer( GL_FRAMEBUFFER, 0 );

	if( bGfx::state.texture2D != nullptr )
	{
		glBindTexture( GL_TEXTURE_2D, bGfx::state.texture2D->texture );
	}

	// Store State
	this->exists = true;
	this->width = width;
	this->height = height;
	this->desc = desc;
	this->textureColor.texture = textureColor;
	this->textureDepth.texture = textureDepth;
	this->fbo = fbo;
}


void GfxSurface2D::free()
{
	if( !this->exists ) { return; }
	nglDeleteBuffers( 1, &fbo );
	this->exists = false;
}

// TODO: This is a hack, refactor this with proper support for render target binding
static u32 SURFACE_CACHE_VIEWPORT_WIDTH;
static u32 SURFACE_CACHE_VIEWPORT_HEIGHT;
static Matrix SURFACE_CACHE_MVP_MATRIX;

void GfxSurface2D::bind()
{
	AssertMsg( this->exists, "Trying to bind surface that doesn't exist!" );

	// Break Batch
	Gfx::quad_batch_break();

	// Bind Render Targets
	nglBindFramebuffer( GL_FRAMEBUFFER, this->fbo );
	nglFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->textureColor.texture, 0 );
	GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
	nglDrawBuffers( sizeof( buffers ) / sizeof( buffers[0] ), buffers );

	// MVP Matriox
	SURFACE_CACHE_MVP_MATRIX = bGfx::state.matrixMVP;
	Gfx::set_matrix_mvp( matrix_build_mvp_orthographic( 0.0f, this->width, 0.0f, this->height, 0.0f, 1.0f ) );

	// Viewport
	SURFACE_CACHE_VIEWPORT_WIDTH = bGfx::viewportWidth;
	SURFACE_CACHE_VIEWPORT_HEIGHT = bGfx::viewportHeight;
	Gfx::viewport_set_size( this->width, this->height, false );

	// Alpha Blend Mode
	glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
}


void GfxSurface2D::release()
{
	// Break Batch
	Gfx::quad_batch_break();

	// Reset glDrawBuffesr
	GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
	nglDrawBuffers( sizeof( buffers ) / sizeof( buffers[0] ), buffers );

	// Release Render Targets
	nglFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0 );
	nglBindFramebuffer( GL_FRAMEBUFFER, 0 );

	// Reset Viewport
	Gfx::viewport_set_size( SURFACE_CACHE_VIEWPORT_WIDTH, SURFACE_CACHE_VIEWPORT_HEIGHT, window::fullscreen );

	// Reset MVP Matrix
	Gfx::set_matrix_mvp( SURFACE_CACHE_MVP_MATRIX );

	// Reset Alpha Blend Mode
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}