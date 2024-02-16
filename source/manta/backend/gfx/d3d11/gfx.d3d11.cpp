#include <manta/gfx.hpp>

#include <manta/backend/gfx/d3d11/d3d11.hpp>

#include <manta/window.hpp>
#include <manta/backend/window/windows/window.windows.hpp>

#include <manta/matrix.hpp>
#include <manta/memory.hpp>

#include <config.hpp>

#define DECL_ZERO(description, name) description name; memory_set( &name, 0, sizeof( description ) )

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct D3D11ColorFormat
{
	D3D11ColorFormat( const DXGI_FORMAT texture2DDescFormat ) : texture2DDescFormat{ texture2DDescFormat } { }

	DXGI_FORMAT texture2DDescFormat;
};

static const D3D11ColorFormat D3D11ColorFormats[] =
{
	{ DXGI_FORMAT_UNKNOWN },           // GfxColorFormat_NONE (TODO: Handle this properly)
	{ DXGI_FORMAT_R8G8B8A8_UNORM },    // GfxColorFormat_R8G8B8A8
	{ DXGI_FORMAT_R10G10B10A2_UNORM }, // GfxColorFormat_R10G10B10A2
	{ DXGI_FORMAT_R8_UNORM },          // GfxColorFormat_R8
	{ DXGI_FORMAT_R8G8_UNORM },        // GfxColorFormat_R8G8
	{ DXGI_FORMAT_R16_UNORM },         // GfxColorFormat_R16
	{ DXGI_FORMAT_R16G16_UNORM },      // GfxColorFormat_R16G16
	{ DXGI_FORMAT_R32_FLOAT },         // GfxColorFormat_R32
};
static_assert( ARRAY_LENGTH( D3D11ColorFormats ) == GFXCOLORFORMAT_COUNT, "Missing GfxColorFormat!" );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct D3D11DepthStencilFormat
{
	D3D11DepthStencilFormat( const DXGI_FORMAT t2DFormat, const DXGI_FORMAT dsvFormat, const DXGI_FORMAT srvFormat ) :
		texture2DDescFormat{ t2DFormat }, depthStencilViewDescFormat{ dsvFormat }, shaderResourceViewDescFormat{ srvFormat }{ }

	DXGI_FORMAT texture2DDescFormat, depthStencilViewDescFormat, shaderResourceViewDescFormat;
};

static const D3D11DepthStencilFormat D3D11DepthStencilFormats[] =
{
	{ DXGI_FORMAT_UNKNOWN,        DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN, },              // GfxDepthFormat_NONE
	{ DXGI_FORMAT_R16_TYPELESS,   DXGI_FORMAT_D16_UNORM,         DXGI_FORMAT_R16_FLOAT },             // GfxDepthFormat_R16_FLOAT
	{ DXGI_FORMAT_R16_TYPELESS,   DXGI_FORMAT_D16_UNORM,         DXGI_FORMAT_R16_UINT },              // GfxDepthFormat_R16_UINT
	{ DXGI_FORMAT_R32_TYPELESS,   DXGI_FORMAT_D32_FLOAT,         DXGI_FORMAT_R32_FLOAT },             // GfxDepthFormat_R32_FLOAT
	{ DXGI_FORMAT_R16_TYPELESS,   DXGI_FORMAT_D16_UNORM,         DXGI_FORMAT_R32_UINT },              // GfxDepthFormat_R32_UINT
	{ DXGI_FORMAT_R24G8_TYPELESS, DXGI_FORMAT_D24_UNORM_S8_UINT, DXGI_FORMAT_R24_UNORM_X8_TYPELESS }, // GfxDepthFormat_R24_UINT_G8_UINT
};
static_assert( ARRAY_LENGTH( D3D11DepthStencilFormats ) == GFXDEPTHFORMAT_COUNT, "Missing GfxDepthFormat!" );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// The GUIDs used for this API
static const GUID IID_IDXGIFactory    { 0x7B7166EC, 0x21C7, 0x44AE, { 0xB2, 0x1A, 0xC9, 0xAE, 0x32, 0x1A, 0xE3, 0x69 } };
static const GUID IID_ID3D11Texture2D { 0x6F15AAF2, 0xD208, 0x4E89, { 0x9A, 0xB4, 0x48, 0x95, 0x35, 0xD3, 0x4F, 0x9C } };

// Graphics Device Interface
static ID3D11Device *device;

// Graphics Context Interface
static ID3D11DeviceContext *context;

// Swap Chain Description
static DXGI_SWAP_CHAIN_DESC sc_desc;

// Swap Chain Interface
static IDXGISwapChain *swap_chain;

// The flags used to create the swap chain
static UINT swap_flags_create;

// The flags used to present the swap chain
static UINT swap_flags_present;

// Raster State Description
static D3D11_RASTERIZER_DESC rs_desc;

// Sampler State Description
static D3D11_SAMPLER_DESC ss_desc;

// Blend State Description
static D3D11_BLEND_DESC bs_desc;

// Depth-Stencil State Description
static D3D11_DEPTH_STENCIL_DESC ds_desc;

// Currently bound color buffer
static ID3D11RenderTargetView *target_color;

// Currently bound depth buffer
static ID3D11DepthStencilView *target_depth;

// Buffer Description & Data
static D3D11_BUFFER_DESC bf_desc;
static D3D11_SUBRESOURCE_DATA bf_data;

// TODO: Refactor
static ID3D11Buffer *vertex_buffer;
static ID3D11Buffer *index_buffer;
static const UINT stride = sizeof( GfxVertex );
static const UINT offset = 0;

// TODO - temporary
static ID3D11Buffer *matrix_buffer;

struct MatrixBuffer
{
	Matrix matrixMVP;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool bGfx::init()
{
	// Flags
#if COMPILE_DEBUG
	UINT flags = D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_DEBUG;
#else
	UINT flags = D3D11_CREATE_DEVICE_SINGLETHREADED;
#endif

	// Feature Level
	D3D_FEATURE_LEVEL featureLevel;
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
	};
	UINT numFeatureLevels = sizeof( featureLevels ) / sizeof( featureLevels[0] );

	// Create Device
	if( FAILED( D3D11CreateDevice( nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags,
	                               featureLevels, numFeatureLevels, D3D11_SDK_VERSION,
								   &device, &featureLevel, &context ) ) )
	{
		ErrorReturnMsg( false, "%s: Failed to create device", __FUNCTION__ );
	}

	// Create Factor
	IDXGIFactory *factory;
	if( FAILED( CreateDXGIFactory( IID_IDXGIFactory, reinterpret_cast<void **>( &factory ) ) ) )
	{
		ErrorReturnMsg( false, "%s: Failed to create DXGI factory", __FUNCTION__ );
	}

	// Setup Swap Chain Description
	sc_desc.BufferDesc.Width = window::width;
	sc_desc.BufferDesc.Height = window::height;
	sc_desc.BufferDesc.RefreshRate.Numerator = 0;
	sc_desc.BufferDesc.RefreshRate.Denominator = 1;
	sc_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sc_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sc_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	sc_desc.SampleDesc.Count = 1;
	sc_desc.SampleDesc.Quality = 0;

	sc_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sc_desc.BufferCount = 2;
	sc_desc.OutputWindow = iwindow::handle;
	sc_desc.Windowed = true;
	sc_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sc_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

	// Create Swap Chain
	if( FAILED( factory->CreateSwapChain( device, &sc_desc, &swap_chain ) ) )
	{
		// If we fail to create a swap chain that should be compatible with Windows 10,
		// we'll falll back to a windows 8.1 compatible one...
		sc_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;

		if( FAILED( factory->CreateSwapChain( device, &sc_desc, &swap_chain ) ) )
		{
			// If we fail to create a swap chain that should be compatible with Windows 8.1,
			// we'll fall back to a standard compatible one...
			sc_desc.BufferCount = 1;
			sc_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
			sc_desc.Flags = 0;

			if( FAILED( factory->CreateSwapChain( device, &sc_desc, &swap_chain ) ) )
			{
				// We were unable to make any swap chain... this is an error
				ErrorReturnMsg( false, "%s: Failed to create swap chain", __FUNCTION__ );
			}
		}
	}

	swap_flags_create = sc_desc.Flags;
	swap_flags_present = sc_desc.Flags == DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING ? DXGI_PRESENT_ALLOW_TEARING : 0;

	// Disable Alt + Enter
	IDXGIFactory *parent;
	if( SUCCEEDED( swap_chain->GetParent( IID_IDXGIFactory, reinterpret_cast<void **>( &parent ) ) ) )
	{
		parent->MakeWindowAssociation( iwindow::handle, DXGI_MWA_NO_ALT_ENTER );
		parent->Release();
	}

	// Get Back Buffer Pointer
	ID3D11Texture2D *buffer;
	if( FAILED( swap_chain->GetBuffer( 0, IID_ID3D11Texture2D, reinterpret_cast<void **>( &buffer ) ) ) )
	{
		ErrorReturnMsg( false, "%s: Failed to get back buffer pointer", __FUNCTION__ );
	}

	// Create Render Target View
	if( FAILED( device->CreateRenderTargetView( buffer, nullptr, &target_color ) ) )
	{
		ErrorReturnMsg( false, "%s: Failed to create render target view", __FUNCTION__ );
	}

	// Create Raster State
	rs_desc.FillMode = D3D11_FILL_SOLID;
	rs_desc.CullMode = D3D11_CULL_NONE;
	rs_desc.FrontCounterClockwise = false;
	rs_desc.DepthBias = 0;
	rs_desc.DepthBiasClamp = 0.0f;
	rs_desc.SlopeScaledDepthBias = 0.0f;
	rs_desc.DepthClipEnable = false;
	rs_desc.MultisampleEnable = false;
	rs_desc.AntialiasedLineEnable = false;

	ID3D11RasterizerState *rs;
	if( FAILED( device->CreateRasterizerState( &rs_desc, &rs ) ) )
	{
		ErrorReturnMsg( false, "%s: Failed to create rasterizer state", __FUNCTION__ );
	}

	// Create Sampler State
	ss_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	ss_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	ss_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	ss_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	ss_desc.MipLODBias = 0.0f;
	ss_desc.MaxAnisotropy = 1;
	ss_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	ss_desc.BorderColor[0] = 0.0f;
	ss_desc.BorderColor[1] = 0.0f;
	ss_desc.BorderColor[2] = 0.0f;
	ss_desc.BorderColor[3] = 0.0f;
	ss_desc.MinLOD = 0.0f;
	ss_desc.MaxLOD = D3D11_FLOAT32_MAX;

	ID3D11SamplerState *ss;
	if( FAILED( device->CreateSamplerState( &ss_desc, &ss ) ) )
	{
		ErrorReturnMsg( false, "%s: Failed to create sampler state", __FUNCTION__ );
	}

	// Setup Blend State
	bs_desc.AlphaToCoverageEnable = false;
	bs_desc.IndependentBlendEnable = false;
	bs_desc.RenderTarget[0].BlendEnable = true;
	bs_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bs_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bs_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bs_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bs_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	bs_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bs_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	ID3D11BlendState *bs;
	if( FAILED( device->CreateBlendState( &bs_desc, &bs ) ) )
	{
		ErrorReturnMsg( false, "%s: Failed to create blend state", __FUNCTION__ );
	}

	// Setup Depth-Stencil State
	ds_desc.DepthEnable = false;
	ds_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	ds_desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	ds_desc.StencilEnable = false;
	ds_desc.StencilReadMask = 0;
	ds_desc.StencilWriteMask = 0;

	ID3D11DepthStencilState *ds;
	if( FAILED( device->CreateDepthStencilState( &ds_desc, &ds ) ) )
	{
		ErrorReturnMsg( false, "%s: Failed to create depth stencil state", __FUNCTION__ );
	}

	// Create Matrix Buffer (TODO: Refactor)
	bf_desc.ByteWidth = sizeof( MatrixBuffer );
	bf_desc.Usage = D3D11_USAGE_DYNAMIC;
	bf_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bf_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bf_desc.MiscFlags = 0;
	bf_desc.StructureByteStride = 0;

	if( FAILED( device->CreateBuffer( &bf_desc, nullptr, &matrix_buffer ) ) )
	{
		ErrorReturnMsg( false, "%s: Failed to create matrixMVP buffer", __FUNCTION__ );
	}

	// Create Vertex Buffer
	bf_desc.ByteWidth = RENDER_QUAD_BATCH_SIZE * sizeof( GfxQuad<GfxVertex> ); // TODO: Refactor
	bf_desc.Usage = D3D11_USAGE_DYNAMIC;
	bf_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bf_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	if( FAILED( device->CreateBuffer( &bf_desc, nullptr, &vertex_buffer ) ) )
	{
		ErrorReturnMsg( false, "%s: Failed to create vertex buffer", __FUNCTION__ );
	}

	// Create Index Buffer
	u16 indices[RENDER_QUAD_BATCH_SIZE * 6];
	Gfx::quad_batch_build_indices( indices );

	bf_desc.ByteWidth = sizeof( indices );
	bf_desc.Usage = D3D11_USAGE_IMMUTABLE;
	bf_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bf_desc.CPUAccessFlags = 0;

	bf_data.pSysMem = indices;

	if( FAILED( device->CreateBuffer( &bf_desc, &bf_data, &index_buffer ) ) )
	{
		ErrorReturnMsg( false, "%s: Failed to create index buffer", __FUNCTION__ );
	}

	// Setup Viewport
	D3D11_VIEWPORT viewport;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<FLOAT>( window::width );
	viewport.Height = static_cast<FLOAT>( window::height );
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	// Setup Context State
	context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	context->IASetVertexBuffers( 0, 1, &vertex_buffer, &stride, &offset );
	context->IASetIndexBuffer( index_buffer, DXGI_FORMAT_R16_UINT, 0 );
	context->VSSetConstantBuffers( 0, 1, &matrix_buffer );
	context->RSSetState( rs );
	context->RSSetViewports( 1, &viewport );
	context->PSSetSamplers( 0, 1, &ss );
	context->OMSetBlendState( bs, nullptr, 0xFFFFFFFF );

	// Release Objects
	factory->Release();
	buffer->Release();

	// Compile Shaders
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

	// Success
	return true;
}


bool bGfx::free()
{
	// Success
	return true;
}


bool bGfx::shader_compile( Shader &shader, const DiskShader &diskShader )
{
	ID3D10Blob *info;
	ID3D10Blob *vsCode, *vsStripped;
	ID3D10Blob *psCode, *psStripped;
	u32 vsSize, psSize;

	// Compile Vertex Shader
	const void *codeVertex = reinterpret_cast<const void *>( assets::binary.data + diskShader.offsetVertex );
	if( FAILED( D3DCompile( codeVertex, diskShader.sizeVertex, nullptr, nullptr, nullptr, "vs_main", "vs_4_0", D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, &vsCode, &info ) ) )
	{
		ErrorReturnMsg( false, "%s: Failed to compile vertex shader\n\n%s", __FUNCTION__, reinterpret_cast<const char *>( info->GetBufferPointer() ) );
	}

	// Compile Fragment Shader
	const void *codeFragment = reinterpret_cast<const void *>( assets::binary.data + diskShader.offsetFragment );
	if( FAILED( D3DCompile( codeFragment, diskShader.sizeFragment, nullptr, nullptr, nullptr, "ps_main", "ps_4_0", D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, &psCode, &info ) ) )
	{
		ErrorReturnMsg( false, "%s: Failed to compile fragment shader\n\n%s", __FUNCTION__, reinterpret_cast<const char *>( info->GetBufferPointer() ) );
	}

	// Strip Vertex Shader
	D3DStripShader( vsCode->GetBufferPointer(), vsCode->GetBufferSize(), D3DCOMPILER_STRIP_REFLECTION_DATA | D3DCOMPILER_STRIP_DEBUG_INFO | D3DCOMPILER_STRIP_PRIVATE_DATA, &vsStripped );

	// Strip Fragment Shader
	D3DStripShader( psCode->GetBufferPointer(), psCode->GetBufferSize(), D3DCOMPILER_STRIP_REFLECTION_DATA | D3DCOMPILER_STRIP_DEBUG_INFO | D3DCOMPILER_STRIP_PRIVATE_DATA, &psStripped );

	// Create Vertex Shader
	if( FAILED( device->CreateVertexShader( vsStripped->GetBufferPointer(), vsStripped->GetBufferSize(), nullptr, &shader.vs ) ) )
	{
		ErrorReturnMsg( false, "%s: Failed to create vertex shader", __FUNCTION__ );
	}

	// Create Pixel Shader
	if( FAILED( device->CreatePixelShader( psStripped->GetBufferPointer(), psStripped->GetBufferSize(), nullptr, &shader.ps ) ) )
	{
		ErrorReturnMsg( false, "%s: Failed to create pixel shader", __FUNCTION__ );
	}

	// TODO: Custom vertex formats
	// TODO: Generate this
	// TODO: Generate corresponding "Vertex" struct for CPU writing/reading
	static const D3D11_INPUT_ELEMENT_DESC desc[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R16G16_UNORM, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	// Create Input Layout
	if( FAILED( device->CreateInputLayout( desc, 3, vsStripped->GetBufferPointer(), vsStripped->GetBufferSize(), &shader.il ) ) )
	{
		ErrorReturnMsg( false, "%s: Failed to create input layout", __FUNCTION__ );
	}

	// Success
	return true;
}


void bGfx::shader_free( Shader &shader )
{
	shader.vs->Release();
	shader.ps->Release();
	shader.il->Release();
}


void bGfx::rb_shader_bind( Shader &shader )
{
	// Bind Shaders
	context->VSSetShader( shader.vs, nullptr, 0 );
	context->PSSetShader( shader.ps, nullptr, 0 );

	// Bind Input Layout
	context->IASetInputLayout( shader.il );
}


bool bGfx::rb_texture2d_load( GfxTexture2D &texture, const void *pixels, const u16 width, const u16 height )
{
	// Setup Texture Description
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	// Setup Texture Data
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = pixels;
	data.SysMemPitch = width * 4;
	data.SysMemSlicePitch = 0;

	// Create Texture
	ID3D11Texture2D *peer;
	if( FAILED( device->CreateTexture2D( &desc, &data, &peer ) ) )
	{
		ErrorReturnMsg( false, "%s: Failed to create texture 2D", __FUNCTION__ );
	}

	// Create Texture View
	if( FAILED( device->CreateShaderResourceView( peer, nullptr, &texture.view ) ) )
	{
		ErrorReturnMsg( false, "%s: Failed to create shader resource view", __FUNCTION__ );
	}

	// Release
	peer->Release();

	// Success
	texture.exists = true;
	return true;
}


void bGfx::rb_texture2d_free( GfxTexture2D &texture )
{
	// Release Texture
	if( !texture.exists ) { return; }
	texture.view->Release();
	texture.exists = false;
}


void bGfx::rb_texture2d_bind( int slot, const GfxTexture2D &texture )
{
	// TODO: Multiple render targets
	context->PSSetShaderResources( slot, 1, &texture.view );
}


void bGfx::rb_texture2d_unbind( int slot )
{
    ID3D11ShaderResourceView *const pSRV[1] = { nullptr };
    context->PSSetShaderResources( slot, 1, pSRV );
}


void bGfx::rb_set_filtering_mode( GfxFilteringMode mode )
{
	switch( mode )
	{
		case GfxFilteringMode_NEAREST:
			ss_desc.Filter = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
		break;

		case GfxFilteringMode_LINEAR:
			ss_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		break;

		case GfxFilteringMode_ANISOTROPIC:
			ss_desc.Filter = D3D11_FILTER_ANISOTROPIC;
		break;

		default:
			AssertMsg( false, "rb_set_filtering_mode: unknown mode %u", mode );
		break;
	}

	ID3D11SamplerState *ss;
	device->CreateSamplerState( &ss_desc, &ss );
	context->PSSetSamplers( 0, 1, &ss );
}


void bGfx::rb_set_depthtest_mode( GfxDepthTestMode mode )
{
	switch( mode )
	{
		case GfxDepthTestMode_NONE:
		{
			ds_desc.DepthEnable = false;
			ds_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			ds_desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
		}
		break;

		case GfxDepthTestMode_LESS:
		{
			ds_desc.DepthEnable = true;
			ds_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			ds_desc.DepthFunc = D3D11_COMPARISON_LESS;
		}
		break;

		case GfxDepthTestMode_LESS_EQUALS:
		{
			ds_desc.DepthEnable = true;
			ds_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			ds_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		}
		break;

		case GfxDepthTestMode_ALWAYS:
		{
			ds_desc.DepthEnable = true;
			ds_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			ds_desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
		}
		break;

		default:
			AssertMsg( false, "rb_set_depthtest_mode: unknown mode %u", mode );
		break;
	}

	ID3D11DepthStencilState *ds;
	device->CreateDepthStencilState( &ds_desc, &ds );
	context->OMSetDepthStencilState( ds, 0 );
}


void bGfx::viewport_set_size( const i32 width, const i32 height, const bool fullscreen )
{
	// Update Viewport
	D3D11_VIEWPORT viewport;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<FLOAT>( width );
	viewport.Height = static_cast<FLOAT>( height );
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	context->RSSetViewports( 1, &viewport );
}


void bGfx::swapchain_resize( const i32 width, const i32 height, const bool fullscreen )
{
	// Unbind Render Target
	context->OMSetRenderTargets( 0, nullptr, nullptr );

	// Release Render Target
	target_color->Release();

	// Resize Buffers
	if( FAILED( swap_chain->ResizeBuffers( 0, width, height, DXGI_FORMAT_UNKNOWN, swap_flags_create ) ) )
	{
		Error( "%s: Failed to resize swap chain buffers", __FUNCTION__ );
	}

	// Get Back Buffer Pointer
	ID3D11Texture2D *buffer;
	if( FAILED( swap_chain->GetBuffer( 0, IID_ID3D11Texture2D, reinterpret_cast<void **>( &buffer ) ) ) )
	{
		Error( "%s: Failed to get swap chain buffer", __FUNCTION__ );
	}

	// Create Render Target
	if( FAILED( device->CreateRenderTargetView( buffer, nullptr, &target_color ) ) )
	{
		Error( "%s: Failed to create device render target view", __FUNCTION__ );
	}

	// Bind Render Target
	context->OMSetRenderTargets( 1, &target_color, nullptr );

	// Free Back Buffer Pointer
	buffer->Release();

	// Update Viewport
	bGfx::viewport_set_size( width, height, fullscreen );
}


int bGfx::rb_shader_get_uniform( Shader &shader, const char *name )
{
	return 0; // D3D11 does not have uniforms (TODO: Remove these entirely?)
}


void bGfx::rb_shader_set_uniform_matrix( const int location, const Matrix &matrix )
{
	// Map Constant Buffer
	D3D11_MAPPED_SUBRESOURCE resource;
	context->Map( matrix_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource );

	// Write Constant Buffer
	reinterpret_cast<MatrixBuffer *>( resource.pData )->matrixMVP = matrix;

	// Unmap Constant Buffer
	context->Unmap( matrix_buffer, 0 );
}


void bGfx::rb_set_matrix_mvp( const Matrix &matrix )
{
	// Map Constant Buffer
	D3D11_MAPPED_SUBRESOURCE resource;
	context->Map( matrix_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource );

	// Write Constant Buffer
	reinterpret_cast<MatrixBuffer *>( resource.pData )->matrixMVP = matrix;

	// Unmap Constant Buffer
	context->Unmap( matrix_buffer, 0 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Gfx::frame_begin()
{
	// State
	bGfx::rendering = true;
	bGfx::state_reset();

	// Reset Render Targets
	context->OMSetRenderTargets( 1, &target_color, nullptr );

	// Begin Batch
	quad_batch_begin();
}


void Gfx::frame_end()
{
	// Finish Batch
	quad_batch_end();

	// Swap Buffers
	{
		//PROFILE_SCOPE("Swapchain")
		const bool swap = SUCCEEDED( swap_chain->Present( false/* || APP_WINDOW_FORCE_VSYNC*/, swap_flags_present ) );
		Assert( swap );
	}

	// Reset State
	bGfx::rendering = false;
}


void Gfx::clear_color( Color color )
{
	FLOAT rgba[4];
	static constexpr float INV_255 = 1.0f / 255.0f;
	rgba[0] = color.r * INV_255;
	rgba[1] = color.g * INV_255;
	rgba[2] = color.b * INV_255;
	rgba[3] = color.a * INV_255;
	context->ClearRenderTargetView( target_color, rgba );
}


void Gfx::clear_depth()
{
	if( target_depth == nullptr ) { return; }
	context->ClearDepthStencilView( target_depth, D3D11_CLEAR_DEPTH, 1.0f, 0 );
}


void Gfx::quad_batch_begin()
{
	// Map Vertex Buffer
	D3D11_MAPPED_SUBRESOURCE resource;
	context->Map( vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource );

	bGfx::quadBatchStart = reinterpret_cast<GfxQuad<GfxVertex> *>( resource.pData );
	bGfx::quadBatchPointer = bGfx::quadBatchStart;
}


void Gfx::quad_batch_end()
{
	// Render State
	bGfx::state_update();

	// Unmap Vertex Buffer
	context->Unmap( vertex_buffer, 0 );

	// Submit Vertex Buffer
	if( bGfx::quadBatchStart != bGfx::quadBatchPointer )
	{
		UINT count = static_cast<UINT>( bGfx::quadBatchPointer - bGfx::quadBatchStart );
		context->DrawIndexed( count * 6, 0, 0 ); // Draw Call
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GfxSurface2D::init( const u16 width, const u16 height, const GfxSurface2DDescription &desc )
{
	AssertMsg( !this->exists, "Initializing surface that already exists!" );

	// Color
	{
		// Create Texture2D
		DECL_ZERO( D3D11_TEXTURE2D_DESC, tex_desc );
		tex_desc.Width = width;
		tex_desc.Height = height;
		tex_desc.MipLevels = 1;
		tex_desc.ArraySize = 1;
		tex_desc.Format = D3D11ColorFormats[desc.colorFormat].texture2DDescFormat;
		tex_desc.SampleDesc.Count = 1;
		tex_desc.SampleDesc.Quality = 0;
		tex_desc.Usage = D3D11_USAGE_DEFAULT;
		tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		tex_desc.CPUAccessFlags = 0;
		tex_desc.MiscFlags = 0;

		ID3D11Texture2D *textureColor = nullptr;
		ErrorIf( FAILED( device->CreateTexture2D( &tex_desc, nullptr, &textureColor ) ),
				"%s: Failed to create color texture 2d", __FUNCTION__ );

		// Create Render Target View
		DECL_ZERO( D3D11_RENDER_TARGET_VIEW_DESC, rtv_desc );
		rtv_desc.Format = tex_desc.Format;
		rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtv_desc.Texture2D.MipSlice = 0;

		Assert( this->targetColor == nullptr );
		ErrorIf( FAILED( device->CreateRenderTargetView( textureColor, &rtv_desc, &this->targetColor ) ),
				"%s: Failed to create color render target view", __FUNCTION__ );

		// Crate Shader Resource View
		DECL_ZERO( D3D11_SHADER_RESOURCE_VIEW_DESC, sr_desc );
		sr_desc.Format = D3D11ColorFormats[desc.colorFormat].texture2DDescFormat;
		sr_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		sr_desc.Texture2D.MostDetailedMip = 0;
		sr_desc.Texture2D.MipLevels = 1;

		ErrorIf( FAILED( device->CreateShaderResourceView( textureColor, &sr_desc, &this->textureColor.view ) ),
				"%s: Failed to create color shader resource view", __FUNCTION__ );

		// Free textureColor
		textureColor->Release();
	}

	// Depth
	if( desc.depthFormat != GfxDepthFormat_NONE )
	{
		// Create Texture2D
		DECL_ZERO( D3D11_TEXTURE2D_DESC, tex_desc );
		tex_desc.Width = width;
		tex_desc.Height = height;
		tex_desc.MipLevels = 1;
		tex_desc.ArraySize = 1;
		tex_desc.Format = D3D11DepthStencilFormats[desc.depthFormat].texture2DDescFormat;
		tex_desc.SampleDesc.Count = 1;
		tex_desc.SampleDesc.Quality = 0;
		tex_desc.Usage = D3D11_USAGE_DEFAULT;
		tex_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		tex_desc.CPUAccessFlags = 0;
		tex_desc.MiscFlags = 0;

		ID3D11Texture2D *textureDepth = nullptr;
		ErrorIf( FAILED( device->CreateTexture2D( &tex_desc, nullptr, &textureDepth ) ),
		         "%s: Failed to create depth texture 2d", __FUNCTION__ );

		// Depth-Stencil View
		DECL_ZERO( D3D11_DEPTH_STENCIL_VIEW_DESC, dsv_desc );
		dsv_desc.Format = D3D11DepthStencilFormats[desc.depthFormat].depthStencilViewDescFormat;
		dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsv_desc.Flags = 0;
		dsv_desc.Texture2D.MipSlice = 0;

		Assert( this->targetDepth == nullptr );
		ErrorIf( FAILED( device->CreateDepthStencilView( textureDepth, &dsv_desc, &this->targetDepth ) ),
				"%s: Failed to create depth render target view", __FUNCTION__ );

		// Shader Resource
		DECL_ZERO( D3D11_SHADER_RESOURCE_VIEW_DESC, sr_desc );
		sr_desc.Format = D3D11DepthStencilFormats[desc.depthFormat].shaderResourceViewDescFormat;
		sr_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		sr_desc.Texture2D.MostDetailedMip = 0;
		sr_desc.Texture2D.MipLevels = 1;

		ErrorIf( FAILED( device->CreateShaderResourceView( textureDepth, &sr_desc, &this->textureDepth.view ) ),
				"%s: Failed to create depth shader resource view", __FUNCTION__ );

		// Free textureDepth
		textureDepth->Release();
	}

	// Store State
	this->exists = true;
	this->width = width;
	this->height = height;
	this->desc = desc;
}


void GfxSurface2D::free()
{
	if( !this->exists ) { return; }

	// Free Color Texture
	this->targetColor->Release();
	this->targetColor = nullptr;
	bGfx::rb_texture2d_free( this->textureColor );

	// Free Depth Texture
	if( this->desc.depthFormat != GfxDepthFormat_NONE )
	{
		this->targetDepth->Release();
		bGfx::rb_texture2d_free( this->textureDepth );
	}
	this->targetDepth = nullptr;

	this->exists = false;
}

// TODO: This is a hack, refactor this with proper support for render target binding
static ID3D11RenderTargetView *SURFACE_CACHE_TARGET_COLOR;
static ID3D11DepthStencilView *SURFACE_CACHE_TARGET_DEPTH;
static u32 SURFACE_CACHE_VIEWPORT_WIDTH;
static u32 SURFACE_CACHE_VIEWPORT_HEIGHT;
static Matrix SURFACE_CACHE_MVP_MATRIX;
static D3D11_BLEND_DESC SURFACE_CACHE_BLEND_STATE;

void GfxSurface2D::bind()
{
	// Break Batch
	Gfx::quad_batch_break();

	// Bind Render Target
	SURFACE_CACHE_TARGET_COLOR = target_color;
	SURFACE_CACHE_TARGET_DEPTH = target_depth;
	target_color = this->targetColor;
	target_depth = this->targetDepth;

	ID3D11RenderTargetView *colorTargets[1];
	colorTargets[0] = this->targetColor;
	context->OMSetRenderTargets( 1, colorTargets, target_depth );

	// MVP Matrix
	SURFACE_CACHE_MVP_MATRIX = bGfx::state.matrixMVP;
	Gfx::set_matrix_mvp( matrix_build_mvp_orthographic( 0.0f, this->width, this->height, 0.0f, 0.0f, 1.0f ) );

	// Viewport
	SURFACE_CACHE_VIEWPORT_WIDTH = bGfx::viewportWidth;
	SURFACE_CACHE_VIEWPORT_HEIGHT = bGfx::viewportHeight;
	Gfx::viewport_set_size( this->width, this->height, false );

	// D3D11 needs to explicitly update depth test state here
	bGfx::rb_set_depthtest_mode( bGfx::state.depthTestMode );

	// Blend State (TODO: Refactor)
	SURFACE_CACHE_BLEND_STATE = bs_desc;
	bs_desc.AlphaToCoverageEnable = false;
	bs_desc.IndependentBlendEnable = false;
	bs_desc.RenderTarget[0].BlendEnable = true;
	bs_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	bs_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bs_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bs_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bs_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	bs_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bs_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	ID3D11BlendState *bs;
	if( FAILED( device->CreateBlendState( &bs_desc, &bs ) ) )
	{
		Error( "%s: Failed to create blend state", __FUNCTION__ );
	}

	context->OMSetBlendState( bs, nullptr, 0xFFFFFFFF );
}


void GfxSurface2D::release()
{
	// Break Batch
	Gfx::quad_batch_break();

	// Release Render Target
	target_color = SURFACE_CACHE_TARGET_COLOR;
	target_depth = SURFACE_CACHE_TARGET_DEPTH;
	context->OMSetRenderTargets( 1, &target_color, target_depth );

	// Reset Viewport
	Gfx::viewport_set_size( SURFACE_CACHE_VIEWPORT_WIDTH, SURFACE_CACHE_VIEWPORT_HEIGHT, window::fullscreen );

	// Reset MVP Matrix
	Gfx::set_matrix_mvp( SURFACE_CACHE_MVP_MATRIX );

	// Reset Blend State (TODO: Refactor)
	bs_desc = SURFACE_CACHE_BLEND_STATE;

	ID3D11BlendState *bs;
	if( FAILED( device->CreateBlendState( &bs_desc, &bs ) ) )
	{
		Error( "%s: Failed to create blend state", __FUNCTION__ );
	}

	context->OMSetBlendState( bs, nullptr, 0xFFFFFFFF );
}