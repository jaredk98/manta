#include <build/string.hpp>

#include <build/memory.hpp>
#include <build/fileio.hpp>

#include <vendor/string.hpp>
#include <vendor/stdarg.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void iString::_strjoin( const usize bufferSize, char *buffer, ... )
{
	if( buffer == nullptr || bufferSize == 0 ) { return; }

	va_list args;
	va_start( args, buffer );
	usize length = 0;

	const char *input;
	while( ( input = va_arg( args, const char * ) ) != nullptr )
	{
		usize inputLength = strlen( input );
		if( length + inputLength >= bufferSize )
		{
			buffer[length] = '\0';
			AssertMsg( false, "strjoin - length exceeded buffer size" )
			va_end( args );
			return;
		}
		strcpy( buffer + length, input );
		length += inputLength;
	}

	va_end( args );
	buffer[length] = '\0';
}


void iString::_strjoin_filepath( const usize bufferSize, char *buffer, ... )
{
	if( buffer == nullptr || bufferSize == 0 ) { return; }
	const usize slashLength = strlen( SLASH );

	va_list args;
	va_start( args, buffer );

	// Copy the first string
	buffer[0] = '\0';
	strncat( buffer, va_arg( args, const char * ), bufferSize - 1 );
	buffer[bufferSize - 1] = '\0';
	usize length = strlen( buffer );

	const char *input;
	while( ( input = va_arg( args, const char * ) ) != nullptr )
	{
		usize inputLength = slashLength + strlen( input );
		if( length + inputLength >= bufferSize )
		{
			buffer[length] = '\0';
			AssertMsg( false, "strjoin_filepath - length exceeded buffer size" )
			va_end( args );
			return;
		}
		strcpy( buffer + length, SLASH );
		strcpy( buffer + length + slashLength, input );
		length += inputLength;
	}

	va_end( args );
	buffer[length] = '\0';
}


void iString::_strappend( const usize bufferSize, char *buffer, const char *string )
{
	if( buffer == nullptr || string == nullptr || bufferSize == 0 ) { return; }

	usize bufferLength = strlen( buffer );
	usize stringLength = strlen( string );

	AssertMsg( bufferLength + stringLength < bufferSize, "strappend - length exceeded buffer size" )
	if( bufferLength + stringLength < bufferSize ) { strncat( buffer, string, bufferSize - bufferLength - 1 ); }

	buffer[bufferSize - 1] = '\0';
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char *StringView::c_str( char *buffer, usize size )
{
	Assert( size > length );
	memory_copy( buffer, data, length );
	buffer[length] = '\0';
	return buffer;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

String::String( const char *str, usize start, usize end )
{
	// Range validation
	const usize length = strlen( str );
	Assert( start <= length );
	if( end == USIZE_MAX ) { end = length; }
	Assert( end <= length );
	Assert( end >= start );

	// c_str
	capacity = end - start;
	current = capacity;
	data = reinterpret_cast<char *>( memory_alloc( capacity + 1 ) );
	ErrorIf( data == nullptr, "Failed to allocate memory for init() String (%p: memory_alloc %d bytes)", data, capacity + 1 );
	memory_copy( data, str + start, current );
	data[current] = '\0';
}


String::String( const String &other, const usize start, const usize end )
{
	copy( other, start, end );
}


String::String( String &&other )
{
	if( this != &other )
	{
		Assert( other.data != nullptr );
		data = other.data;
		capacity = other.capacity;
		current = other.current;

		other.data = nullptr;
		other.capacity = 0;
		other.current = 0;
	}
}


String::~String()
{
	if( data == nullptr ) { return; }
	memory_free( data );
	data = nullptr;
}


bool String::save( const char *path )
{
	Assert( data != nullptr );
	FILE *file = fopen( path, "wb" );
	if( file == nullptr ) { return false; }

	bool returnCode = true;
	if( fwrite( data, current, 1, file ) < 1 ) { returnCode = false; goto cleanup; }

cleanup:
	if( fclose( file ) != 0 ) { return false; }
	return returnCode;
}


bool String::load( const char *path )
{
	Assert( data != nullptr );
	FILE *file = fopen( path, "rb" );
	if( file == nullptr ) { return false; }

	bool returnCode = true;
	const usize size = fsize( file );
	if( size == 0 ) { returnCode = false; goto cleanup; }

	capacity = size + 1;
	current = size;
	data = reinterpret_cast<char *>( memory_realloc( data, capacity ) );
	ErrorIf( data == nullptr, "Failed to allocate memory for load() String (%p: memory_alloc %d bytes)", data, capacity );

	if( fread( data, size, 1, file ) < 1 ) { returnCode = false; goto cleanup; }
	data[size] = '\0';

cleanup:
	if( fclose( file ) != 0 ) { return false; }
	return returnCode;
}


String & String::operator=( const String &other )
{
	return copy( other );
}


String & String::operator=( String &&other )
{
	if( this != &other )
	{
		if( data != nullptr ) { memory_free( data ); }

		data = other.data;
		capacity = other.capacity;
		current = other.current;
		data[current] = '\0';

		other.data = nullptr;
		other.capacity = 0;
		other.current = 0;
	}
	return *this;
}


String & String::copy( const String &other, usize start, usize end )
{
	// Range validation
	Assert( start <= other.current );
	if( end == USIZE_MAX ) { end = other.current; }
	Assert( end <= other.current );
	Assert( end >= start );

	// Copy data
	if( this != &other )
	{
		capacity = end - start;
		current = capacity;
		if( data == nullptr )
		{
			data = reinterpret_cast<char *>( memory_alloc( capacity + 1 ) );
			ErrorIf( data == nullptr, "Failed to allocate memory for copy() String (%p: memory_alloc %d bytes)", data, capacity + 1 );
		}
		else
		{
			data = reinterpret_cast<char *>( memory_realloc( data, capacity + 1 ) );
			ErrorIf( data == nullptr, "Failed to reallocate memory for copy() String (%p: memory_realloc %d bytes)", data, capacity + 1 );
		}
		memory_copy( data, other.data + start, current );
		data[current] = '\0';
	}
	return *this;
}


String & String::append( const char *str )
{
	// Grow data (if necessary)
	const usize length = strlen( str );
	while( capacity < current + length ) { grow(); }

	// Append string
	memory_copy( data + current, str, length );
	current += length;
	data[current] = '\0';
	return *this;
}


String & String::append( const StringView &view )
{
	// Grow data (if necessary)
	while( capacity < current + view.length ) { grow(); }

	// Append string
	memory_copy( data + current, view.data, view.length );
	current += view.length;
	data[current] = '\0';
	return *this;
}


String & String::append( char c )
{
	// Grow data (if necessary)
	while( capacity < current + 1 ) { grow(); }

	// Append string
	data[current++] = c;
	data[current] = '\0';
	return *this;
}


String & String::append( int integer )
{
	char buffer[32];
	snprintf( buffer, 32, "%d", integer );
	return this->append( buffer );
}


String & String::append( u64 integer )
{
	char buffer[32];
	snprintf( buffer, 32, "%llu", integer );
	return this->append( buffer );
}


String & String::append( double number )
{
	char buffer[32];
	snprintf( buffer, 32, "%f", number );
	return this->append( buffer );
}


String String::operator+( const String &rhs )
{
	String result { c_str() };
	result.append( rhs.c_str() );
	return result;
}


String & String::operator+=( const String &rhs )
{
	return append( rhs.c_str() );
}


String & String::insert( const usize index, const char *str )
{
	// Check for empty substring
	if( str == nullptr || str[0] == '\0' ) { return *this; }

	// Grow data (if necessary)
	Assert( index <= current );
	const usize length = strlen( str );
	while( capacity < current + length ) { grow(); }

	// Move chars after index to the right
	for( usize i = current + length; i >= index + length; i-- )
	{
		data[i] = data[i-length];
	}

	// Insert string
	memory_copy( data + index, str, length );
	current += length;
	data[current] = '\0';
	return *this;
}


String & String::remove( const usize index, const usize count )
{
	Assert( index < current );
	Assert( index + count <= current );

	// Shift the right side of the string over
	for( usize i = index; i < current - count; i++ ) { data[i] = data[i + count]; }
	current -= count;
	data[current] = '\0';
	return *this;
}


void String::upper()
{
	for( usize i = 0; i < current && data[i] != '\0'; i++ )
	{
		data[i] = ( data[i] >= 'a' && data[i] <= 'z' ) ? ( data[i] & 0xDF ) : data[i];
	}
}


void String::lower()
{
	for( usize i = 0; i < current && data[i] != '\0'; i++ )
	{
		data[i] = ( data[i] >= 'A' && data[i] <= 'Z' ) ? ( data[i] | 0x20 ) : data[i];
	}
}


String & String::trim( const bool trimTabs )
{
	// The string is empty; nothing to trim
	if( current == 0 ) { return *this; }

	// Trim leading whitespace characters
	usize leadingSpaces = 0;
	while( leadingSpaces < current && char_whitespace( data[leadingSpaces], trimTabs ) ) { leadingSpaces++; }

	// Trim trailing whitespace characters
	usize trailingSpaces = 0;
	while( trailingSpaces < current && char_whitespace(data[current - trailingSpaces - 1], trimTabs ) ) { trailingSpaces++; }

	if( leadingSpaces > 0 || trailingSpaces > 0 )
	{
		// There are leading or trailing spaces to trim
		Assert( leadingSpaces + trailingSpaces <= current );
		current -= ( leadingSpaces + trailingSpaces );
		for( usize i = 0; i < current; i++ ) { data[i] = data[i + leadingSpaces]; }
		data[current] = '\0';
	}

	return *this;
}


usize String::find( const char *substr, usize start, usize end ) const
{
	// Range validation
	if( start >= current ) { return USIZE_MAX; }
	if( end == USIZE_MAX ) { end = current; }
	Assert( end <= current );
	Assert( end >= start );

	// Check for empty substring
	if( substr == nullptr || substr[0] == '\0' ) { return USIZE_MAX; }

	// Iterate through the string
	const usize length = strlen( substr );
	end = current - length < end ? current - length + 1 : end;
	for( usize i = start; i < end; i++ )
	{
		bool found = true;
		for( usize j = 0; substr[j] != '\0'; j++ )
		{
			if( data[i + j] != substr[j] )
			{
				found = false;
				break;
			}
		}

		if( found ) { return i; }
	}

	// Substring not found
	return USIZE_MAX;
}


bool String::contains_at( const char *substr, usize index ) const
{
	// Range validation
	if( index >= current ) { return false; }

	// Check for empty substring
	if( substr == nullptr || substr[0] == '\0' ) { return false; }

	// Check if substr at index fits within the string
	const usize length = strlen( substr );
	if( index + length > current ) { return false; }

	// Compare substr at index
	for( usize i = 0; i < length; i++ )
	{
		if( data[index + i] != substr[i] ) { return false; }
	}

	// Success
	return true;
}


String & String::replace( const char *substr, const char *str, usize start, usize end )
{
	// Range validation
	Assert( start <= current );
	if( end == USIZE_MAX ) { end = current; }
	Assert( end <= current );
	Assert( end >= start );

	// Empty strings?
	if( substr == nullptr || substr[0] == '\0' || str == nullptr) { return *this; }

	const usize lenSubstr = strlen( substr );
	const usize lenStr = strlen( str );
	const i64 lenDiff = ( lenStr - lenSubstr );

	// Replace substr's
	usize index = find( substr, start, end );
	while( index != USIZE_MAX && index + lenSubstr <= end )
	{
		remove( index, lenSubstr );
		insert( index, str );

		// Find next
		start = index + lenStr;
		end += lenDiff;
		index = find( substr, start, end );
	}

	return *this;
}


String String::substr( const usize start, const usize end ) const
{
	// Create new string
	String result;
	result.copy( *this, start, end );
	return result;
}


bool String::equals( const char *str ) const { return strcmp( data, str ) == 0; }


void String::grow()
{
	capacity = capacity == 0 ? 1 : capacity * 2;
	data = reinterpret_cast<char *>( memory_realloc( data, capacity + 1 ) );
	ErrorIf( data == nullptr, "Failed to allocate memory for grow() String (%p: memory_alloc %d bytes)", data, capacity + 1 );
	data[capacity] = '\0';
}