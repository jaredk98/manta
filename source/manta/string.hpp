#pragma once

#include <vendor/string.hpp>
#include <vendor/stdarg.hpp>

#include <types.hpp>
#include <debug.hpp>
#include <manta/memory.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline bool char_whitespace( const char c, const bool trimTabs = true )
{
	return c == ' ' || c == '\n' || c == '\r' || ( trimTabs && c == '\t' );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace iString
{
	extern void _strjoin( const usize bufferSize, char *buffer, ... );
	extern void _strjoin_filepath( const usize bufferSize, char *buffer, ... );
	extern void _strappend( const usize bufferSize, char *buffer, const char *string );
}

#define strjoin( buffer, ... ) iString::_strjoin( sizeof( buffer ), buffer, __VA_ARGS__, nullptr )
#define strjoin_filepath( buffer, ... ) iString::_strjoin_filepath( sizeof( buffer ), buffer, __VA_ARGS__, nullptr );
#define strappend( buffer, string ) iString::_strappend( sizeof( buffer ), buffer, string )

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <usize N = 1024>
class FixedString
{
public:
	FixedString() { data[0] = '\0'; }

	FixedString( const char *str )
	{
		memory_copy( data, str, N );
		data[N] = '\0';
	}

	FixedString( const FixedString<N> &other )
	{
		memory_copy( data, other.data, N );
		data[N] = '\0';
	}

	FixedString<N> &operator=( const FixedString<N> &other )
	{
		if( this != &other )
		{
			memory_copy( data, other.data, N );
			data[N] = '\0';
		}
		return *this;
	}

	usize length() const
	{
        usize length = 0;
        while( length < N && data[length] != '\0' ) { ++length; }
        return length;
	}

	bool equals( const char *str ) const
	{
		return strcmp( data, str ) == 0;
	}

	inline bool operator==( const char *str ) const
	{
		return equals( str );
	}

	void upper()
	{
		for( usize i = 0; i < N && data[i] != '\0'; i++ )
		{
			data[i] = ( data[i] >= 'a' && data[i] <= 'z' ) ? ( data[i] & 0xDF ) : data[i];
		}
	}

	void lower()
	{
		for( usize i = 0; i < N && data[i] != '\0'; i++ )
		{
			data[i] = ( data[i] >= 'A' && data[i] <= 'Z' ) ? ( data[i] | 0x20 ) : data[i];
		}
	}

	const char *c_str() const
	{
		return data;
	}

	char &operator[]( const usize index )
	{
		Assert( index <= N );
		return data[index];
	}

	const char& operator[]( const usize index ) const
	{
		Assert( index <= N );
		return data[index];
	}

private:
	char data[N + 1];
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class StringIterator
{
public:
	StringIterator( char *ptr ) : current( ptr ) { }

	char &operator*() { return *current; }
	const char &operator*() const { return *current; }

	StringIterator &operator++() { ++current; return *this; }
	StringIterator operator++( int ) { StringIterator tmp = *this; ++( *this ); return tmp; }

	bool operator==( const StringIterator &other ) const { return current == other.current; }
	bool operator!=( const StringIterator &other ) const { return !( *this == other ); }

private:
	char *current;
};


class String
{
public:
	// c_str constructor (default)
	String( const char *str = "", usize start = 0, usize end = USIZE_MAX )
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
		memory_copy( data, str + start, current );
		data[current] = '\0';
	}

	// Copy constructor
	String( const String &other, const usize start = 0, const usize end = USIZE_MAX )
	{
		copy( other, start, end );
	}

	// Move constructor
	String( String &&other )
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

	// Destructor
	~String()
	{
		if( data == nullptr ) { return; }
		memory_free( data );
		data = nullptr;
	}

	// Copy assignment
	String &operator=( const String &other )
	{
		return copy( other );
	}

	// Move assignment
	String &operator=( String &&other )
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

	// Copy
	String &copy( const String &other, usize start = 0, usize end = USIZE_MAX )
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
			data = reinterpret_cast<char *>( data == nullptr ? memory_alloc( capacity + 1 ) : memory_realloc( data, capacity + 1 ) );
			memory_copy( data, other.data + start, current );
			data[current] = '\0';
		}
		return *this;
	}

	// Appending
	String &append( const char *str )
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

	String operator+( const String &rhs )
	{
		String result { c_str() };
		result.append( rhs.c_str() );
		return result;
	}

	String &operator+=( const String &rhs )
	{
		return append( rhs.c_str() );
	}

	// Insertion
	String &insert( const usize index, const char *str )
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

	// Removal
	String &remove( const usize index, const usize count )
	{
		Assert( index < current );
		Assert( index + count <= current );

		// Shift the right side of the string over
		for( usize i = index; i < current - count; i++ ) { data[i] = data[i + count]; }
		current -= count;
		data[current] = '\0';
		return *this;
	}

	// To Uppercase
	void upper()
	{
		for( usize i = 0; i < current && data[i] != '\0'; i++ )
		{
			data[i] = ( data[i] >= 'a' && data[i] <= 'z' ) ? ( data[i] & 0xDF ) : data[i];
		}
	}

	// To Lowercase
	void lower()
	{
		for( usize i = 0; i < current && data[i] != '\0'; i++ )
		{
			data[i] = ( data[i] >= 'A' && data[i] <= 'Z' ) ? ( data[i] | 0x20 ) : data[i];
		}
	}

	// Trim
	String &trim( const bool trimTabs = true )
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


	// Find
	usize find( const char *substr, usize start = 0, usize end = USIZE_MAX ) const
	{
		// Range validation
		Assert( start <= current );
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

	// Contains At
	bool contains_at( const char *substr, usize index ) const
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

	// Replace
	String & replace( const char *substr, const char *str, usize start, usize end )
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

	// Substr
	String substr( const usize start = 0, const usize end = USIZE_MAX ) const
	{
		// Create new string
		String result;
		result.copy( *this, start, end );
		return result;
	}

	// Length
	inline usize length() const { return current; }

	// Equality
	bool equals( const char *str ) const { return strcmp( data, str ) == 0; }
	bool operator==( const char *str ) const { return equals( str ); }

	// Char String
	inline const char *c_str() const { data[current] = '\0'; return data; }
	inline operator const char *() const { return c_str(); }

	// Indexer
	inline       char &operator[]( const usize index )       { Assert( index <= capacity ); return data[index]; }
	inline const char &operator[]( const usize index ) const { Assert( index <= capacity ); return data[index]; }

	// Iterator
	StringIterator begin() { return StringIterator( data ); }
	StringIterator end() { return StringIterator( data + current ); }

	// Data
	const char *get_pointer( const usize index ) const { return &data[index]; }

	bool save( const char *path );
	bool load( const char *path );

private:
	// Grow memory
	void grow()
	{
		capacity = capacity == 0 ? 1 : capacity * 2;
		data = reinterpret_cast<char *>( memory_realloc( data, capacity + 1 ) );
		data[capacity] = '\0';
	}

	// Data & State
	char *data = nullptr;
	usize capacity = 0;
	usize current = 0;
};