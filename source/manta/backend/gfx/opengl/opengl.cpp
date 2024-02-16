#include <manta/backend/gfx/opengl/opengl.hpp>

#include <manta/gfx.hpp>

#include <debug.hpp>
#include <config.hpp>
#include <types.hpp>


// Define OpenGL Procedures
#if !GL_MAC
	#undef  META
	#define META(type, name, ...) n##name##proc n##name;
	#include "opengl.procedures.hpp"
#endif


bool opengl_load()
{
	// Load OpenGL Procedures
	#if !GL_MAC
		#undef  META

		#define META(type, name, ...)                                                                 \
			if( ( n##name = reinterpret_cast<n##name##proc>( opengl_proc( #name ) ) ) == nullptr )    \
				{ ErrorReturnMsg( false, "OPENGL: Failed to load OpenGL procedure (%s)", #name ); }

		#include "opengl.procedures.hpp"
	#endif

	// Success
	return true;
}