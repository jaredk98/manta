#include <manta/assets.hpp>

#include <manta/string.hpp>
#include <manta/fileio.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Assets
{
	File binary;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool iAssets::init()
{
	// Open Binary File
	char pathBinary[PATH_SIZE];
	strjoin( pathBinary, WORKING_DIRECTORY, SLASH BUILD_PROJECT, ".bin" );
	if( !Assets::binary.open( pathBinary ) )
	{
		strjoin( pathBinary, BUILD_PROJECT, ".bin" );
		ErrorReturnIf( !Assets::binary.open( pathBinary ), false, "Assets: Failed to open binary file: %s", pathBinary );
	}

	// Success
	return true;
}


bool iAssets::free()
{
	// Close Binary File
	Assets::binary.close();

	// Success
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////