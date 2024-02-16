#include <manta/assets.hpp>

#include <manta/string.hpp>
#include <manta/fileio.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace assets
{
	File binary;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool iassets::init()
{
	// Open Binary File
	char pathBinary[PATH_SIZE];
	strjoin( pathBinary, WORKING_DIRECTORY, SLASH BUILD_PROJECT, ".bin" );
	ErrorReturnIf( !assets::binary.open( pathBinary ), false, "Assets: Failed to open binary file: %s", pathBinary );

	// Success
	return true;
}

bool iassets::free()
{
	// Close Binary File
	return assets::binary.close();
}