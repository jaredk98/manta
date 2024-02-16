#include <manta/gfx.shaders.hpp>

#include <manta/gfx.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace shaders
{
	Shader shaders[shadersCount];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool shaders::init()
{
	// Compile All Shaders
	for( u32 i = 0; i < shaders::shadersCount; i++ )
	{
		Shader &shader = shaders::shaders[i];
		const DiskShader &diskShader = shaders::diskShaders[i];

		if( !bGfx::shader_compile( shader, diskShader ) )
		{
			ErrorReturnMsg( false, "Shaders: Failed to compile shader %i", i );
		}
	}

	// Success
	return true;
}