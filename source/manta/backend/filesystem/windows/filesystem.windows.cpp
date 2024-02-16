#include <manta/fileio.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool file_time( const char *path, FileTime *result )
{
	// Open File
	HANDLE file;
	if( ( file = CreateFileA( path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr ) ) == INVALID_HANDLE_VALUE )
	{
		return false;
	}

	// Get File Time
	bool success = true;
	if( !GetFileTime( file, nullptr, nullptr, &result->time ) ) { success = false; }

	// Close File
	CloseHandle( file );

	// Success?
	return success;
}


bool file_time_newer( const FileTime &a, const FileTime &b )
{
	ULARGE_INTEGER integerA;
	integerA.LowPart  = a.time.dwLowDateTime;
	integerA.HighPart = a.time.dwHighDateTime;

	ULARGE_INTEGER integerB;
	integerB.LowPart  = b.time.dwLowDateTime;
	integerB.HighPart = b.time.dwHighDateTime;

	return integerA.QuadPart > integerB.QuadPart;
}


void directory_create( const char *path )
{
	CreateDirectoryA( path, nullptr );
}