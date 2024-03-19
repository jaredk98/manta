#pragma once

#include <types.hpp>
#include <debug.hpp>
#include <traits.hpp>

#include <vendor/string.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline bool char_whitespace( const char c, const bool trimTabs = true )
{
	return c == ' ' || c == '\n' || c == '\r' || ( trimTabs && c == '\t' );
}

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

#define TAB "    "
#define COMMENT_BREAK "////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct StringView
{
	StringView() = default;
	StringView( const char *data ) : data{ data }, length{ strlen( data ) } { }
	StringView( const char *data, usize length ) : data{ data }, length{ length } { }
	const char *data = nullptr;
	usize length = 0;
	char *c_str( char *buffer, const usize size );
};

inline u32 hash( const StringView stringView )
{
	return hash( stringView.data, stringView.length );
}

inline bool equals( const StringView a, const StringView b )
{
	return a.length == b.length && strncmp( a.data, b.data, a.length ) == 0;
}

inline bool is_null( const StringView a ) { return a.data == nullptr; }

inline void set_null( StringView &a ) { a.data = nullptr; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class String
{
public:
	String( const char *str = "", usize start = 0, usize end = USIZE_MAX );
	String( const String &other, const usize start = 0, const usize end = USIZE_MAX );
	String( String &&other );
	~String();

	String &operator=( const String &other );
	String &operator=( String &&other );

	bool save( const char *path );
	bool load( const char *path );

	String &copy( const String &other, usize start = 0, usize end = USIZE_MAX );

	String &append( const char *str );
	String &append( const StringView &view );
	String &append( char str );
	String &append( int integer );
	String &append( u64 integer );
	String &append( double number );
	String operator+( const String &rhs );
	String &operator+=( const String &rhs );

	String &insert( const usize index, const char *str );

	String &remove( const usize index, const usize count );

	void upper();
	void lower();

	String &trim( const bool trimTabs = true );

	usize find( const char *substr, usize start = 0, usize end = USIZE_MAX ) const;

	bool contains_at( const char *substr, usize index ) const;

	String &replace( const char *substr, const char *str, usize start = 0, usize end = USIZE_MAX );

	String substr( const usize start = 0, const usize end = USIZE_MAX ) const;

	inline usize length() const { return current; }

	bool equals( const char *str ) const;
	inline bool operator==( const char *str ) const { return equals( str ); }
	inline bool operator==( const String &str ) const { return equals( str ); }

	inline const char *c_str() const { data[current] = '\0'; return data; }
	inline operator const char *() const { return c_str(); }

	inline       char &operator[]( const usize index )       { Assert( index <= capacity ); return data[index]; }
	inline const char &operator[]( const usize index ) const { Assert( index <= capacity ); return data[index]; }

	const char *get_pointer( const usize index ) const { return &data[index]; }

	class StringIterator
	{
	public:
		StringIterator( char *ptr ) : current( ptr ) { }

		inline char &operator*() { return *current; }
		inline const char &operator*() const { return *current; }

		inline StringIterator &operator++() { ++current; return *this; }
		inline StringIterator operator++( int ) { StringIterator tmp = *this; ++( *this ); return tmp; }

		inline bool operator==( const StringIterator &other ) const { return current == other.current; }
		inline bool operator!=( const StringIterator &other ) const { return !( *this == other ); }

	private:
		char *current;
	};

	inline StringIterator begin() { return StringIterator( data ); }
	inline StringIterator end() { return StringIterator( data + current ); }

	void grow();

	char *data = nullptr;
	usize capacity = 0;
	usize current = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////