#pragma once

#include <build/string.hpp>

#include <debug.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class JSON
{
public:
	JSON( String &string );
	JSON( String *string, usize start = U64_MAX, usize end = U64_MAX ) : string( string ), start( start ), end( end ) { }

	JSON Object( const char *key );
	JSON ObjectAt( const usize index );

	JSON Array( const char *key );
	JSON ArrayAt( const usize index );

	usize Count();

	String GetString( const char *key, const char *defaultValue = "" );
	String GetStringAt( const usize index, const char *defaultValue = "" );

	double GetDouble( const char *key, const double defaultValue = 0.0 );
	double GetDoubleAt( const usize index, const double defaultValue = 0.0 );

	inline float GetFloat( const char *key, const float defaultValue = 0.0f ) { return static_cast<float>( GetDouble( key, static_cast<double>( defaultValue ) ) ); }
	inline float GetFloatAt( const usize index, const float defaultValue = 0.0f ) { return static_cast<float>( GetDoubleAt( index, static_cast<double>( defaultValue ) ) ); }

	int GetInt( const char *key, const int defaultValue = 0 );
	int GetIntAt( const usize index, const int defaultValue = 0 );

	bool GetBool( const char *key, const bool defaultValue = false );
	bool GetBoolAt( const usize index, const bool defaultValue = false );

	explicit operator bool() const { return start < end; }

//private:
	struct JSONElement
	{
		JSONElement( usize start, usize end ) : start( start ), end( end ) { }
		usize start = 0;
		usize end = 0;
		explicit operator bool() const { return start < end; }
	};

	JSONElement FindElementKey( const char *key );
	JSONElement FindElementIndex( const usize index );

	String *string;
	usize start;
	usize end;
};