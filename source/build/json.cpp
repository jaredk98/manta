#include <build/json.hpp>

#include <vendor/string.hpp>
#include <vendor/stdlib.hpp>

#include <debug.hpp>
#include <types.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static usize find_element_key( const String &buffer, const char *key, usize start = 0, usize end = USIZE_MAX )
{
	bool inQuotes = false;
	int depthBraces = 0;
	int depthBrackets = 0;

	for( usize i = start; i < end; i++ )
	{
		// Length check / null terminator
		if( i >= buffer.length() || buffer[i] == '\0' ) { break; }

		// Find Key / Skip Quotes
		if( depthBraces == 0 && depthBrackets == 0 )
		{
			if( buffer[i] == '"' && ( i == 0 || buffer[i-1] != '\\' ) )
			{
				if( inQuotes ) { inQuotes = false; continue; }
				if( buffer.contains_at( key, i + 1 ) && buffer.contains_at( "\":", i + 1 + strlen( key ) ) ) { return i + strlen( key ) + 3; }
				inQuotes = true;
			}
			if( inQuotes ) { continue; }
		}

		// Process Braces/Brackets
		if( buffer[i] == '}' ) { if( depthBraces > 0 ) { depthBraces--; } else { break; } }
		if( buffer[i] == ']' ) { if( depthBrackets > 0 ) { depthBrackets--; } else { break; } }
		if( buffer[i] == '{' ) { depthBraces++; continue; }
		if( buffer[i] == '[' ) { depthBrackets++; continue; }
	}

	// Failure
	return USIZE_MAX;
}

static usize find_element_index( const String &buffer, const usize index, usize start = 0, usize end = USIZE_MAX )
{
	bool inQuotes = false;
	int depthBraces = 0;
	int depthBrackets = 0;

	usize last = start;
	usize count = 0;
	for( usize i = start; i < end; i++ )
	{
		// Length check / null terminator
		if( i >= buffer.length() || buffer[i] == '\0' ) { break; }

		// Skip quotes
		if( buffer[i] == '"' && ( i == 0 || buffer[i-1] != '\\' ) ) { inQuotes = !inQuotes; }
		if( inQuotes ) { continue; }

		// Process Braces/Brackets
		if( buffer[i] == '{' ) { depthBraces++; continue; }
		if( buffer[i] == '[' ) { depthBrackets++; continue; }
		if( buffer[i] == '}' ) { if( depthBraces > 0 ) { depthBraces--; } else { break; } }
		if( buffer[i] == ']' ) { if( depthBrackets > 0 ) { depthBrackets--; } else { break; } }
		if( depthBraces > 0 || depthBrackets > 0 ) { continue; }

		// Process ',' delimiter
		if( buffer[i] == ',' )
		{
			if( count == index ) { return last; }
			last = i + 1;
			count++;
		}
	}

	// Failure
	return count == index ? last : USIZE_MAX;
}

static usize find_element_delimiter( const String &buffer, usize start = 0, usize end = USIZE_MAX )
{
	bool inQuotes = false;
	int depthBraces = 0;
	int depthBrackets = 0;

	for( usize i = start; i < end; i++ )
	{
		// Length check / null terminator
		if( i >= buffer.length() || buffer[i] == '\0' ) { return i; }

		// Skip quotes
		if( buffer[i] == '"' && ( i == 0 || buffer[i-1] != '\\' ) ) { inQuotes = !inQuotes; }
		if( inQuotes ) { continue; }

		// Process Braces/Brackets
		if( buffer[i] == '{' ) { depthBraces++; continue; }
		if( buffer[i] == '[' ) { depthBrackets++; continue; }
		if( buffer[i] == '}' ) { if( depthBraces > 0 ) { depthBraces--; } else { return i; } }
		if( buffer[i] == ']' ) { if( depthBrackets > 0 ) { depthBrackets--; } else { return i; } }
		if( depthBraces > 0 || depthBrackets > 0 ) { continue; }

		// Process ',' delimiter
		if( buffer[i] == ',' ) { return i; }
	}

	// Failure
	return end;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JSON::JSON( String &string )
{
	// Strip Whitespace
	bool inQuotes = false;
	for( usize i = string.length(); i > 0; i-- )
	{
		if( string[i-1] == '"' && ( i <= 1 || string[i-2] != '\\' ) ) { inQuotes = !inQuotes; continue; }
		if( !inQuotes && char_whitespace( string[i-1], true ) ) { string.remove( i - 1, 1 ); continue; }
	}

	this->start = 1;
	AssertMsg( string[0] == '{', "JSON has invalid root scope (no open {)\n\nJSON:\n%s", string.c_str() );
	this->end = string.length() - 1;
	AssertMsg( string[this->end] == '}', "JSON has invalid root scope (no closing })\n\nJSON:\n%s", string.c_str() );
	this->string = &string;
}


JSON::JSONElement JSON::FindElementKey( const char *key )
{
	// Locate Start
	const usize indexStart = find_element_key( *string, key, start, end );
	if( indexStart > end ) { return { 0, 0 }; }

	// Locate End
	const usize indexEnd = find_element_delimiter( *string, indexStart, end );
	if( indexEnd > end || indexEnd <= indexStart ) { return { 0, 0 }; }

	// Success
	return { indexStart, indexEnd };
}


JSON::JSONElement JSON::FindElementIndex( const usize index )
{
	// Locate Start
	const usize indexStart = find_element_index( *string, index, start, end );
	if( indexStart > end ) { return { 0, 0 }; }

	// Locate End
	const usize indexEnd = find_element_delimiter( *string, indexStart, end );
	if( indexEnd > end || indexEnd <= indexStart ) { return { 0, 0 }; }

	// Success
	return { indexStart, indexEnd };
}


usize JSON::Count()
{
	// Early out if empty
	if( end - start <= 1 ) { return 0; }

	bool inQuotes = false;
	int depthBraces = 0;
	int depthBrackets = 0;
	usize count = 1;
	for( usize i = start; i < end; i++ )
	{
		// Length check / null terminator
		if( i >= string->length() || (*string)[i] == '\0' ) { break; }

		// Skip quotes
		if( (*string)[i] == '"' && ( i == 0 || (*string)[i-1] != '\\' ) ) { inQuotes = !inQuotes; }
		if( inQuotes ) { continue; }

		// Process Braces/Brackets
		if( (*string)[i] == '{' ) { depthBraces++; continue; }
		if( (*string)[i] == '[' ) { depthBrackets++; continue; }
		if( (*string)[i] == '}' ) { if( depthBraces > 0 ) { depthBraces--; } else { break; } }
		if( (*string)[i] == ']' ) { if( depthBrackets > 0 ) { depthBrackets--; } else { break; } }
		if( depthBraces > 0 || depthBrackets > 0 ) { continue; }

		// Process ',' delimiter
		if( (*string)[i] == ',' ) { count++; }
	}

	// Failure
	return count;
}


JSON JSON::Object( const char *key )
{
	// Find Element
	JSONElement element = FindElementKey( key );
	if( !element ) { return { string, 0, 0 }; }

	// Get Object
	if( (*string)[element.start] != '{' ) { return { string, 0, 0 }; }
	if( (*string)[element.end-1] != '}' ) { return { string, 0, 0 }; }

	return { string, element.start + 1, element.end - 1 };
}


JSON JSON::ObjectAt( const usize index )
{
	// Find Element
	JSONElement element = FindElementIndex( index );
	if( !element ) { return { string, 0, 0 }; }

	// Get Object
	if( (*string)[element.start] != '{' ) { return { string, 0, 0 }; }
	if( (*string)[element.end-1] != '}' ) { return { string, 0, 0 }; }

	return { string, element.start + 1, element.end - 1 };
}

JSON JSON::Array( const char *key )
{
	// Find Element
	JSONElement element = FindElementKey( key );
	if( !element ) { return { string, 0, 0 }; }

	// Get Array
	if( (*string)[element.start] != '[' ) { return { string, 0, 0 }; }
	if( (*string)[element.end-1] != ']' ) { return { string, 0, 0 }; }
	return { string, element.start + 1, element.end - 1 };
}


JSON JSON::ArrayAt( const usize index )
{
	// Find Element
	JSONElement element = FindElementIndex( index );
	if( !element ) { return { string, 0, 0 }; }

	// Get Array
	if( (*string)[element.start] != '[' ) { return { string, 0, 0 }; }
	if( (*string)[element.end-1] != ']' ) { return { string, 0, 0 }; }
	return { string, element.start + 1, element.end - 1 };
}


String JSON::GetString( const char *key, const char *defaultValue )
{
	// Find Element
	JSONElement element = FindElementKey( key );
	if( !element ) { return defaultValue; }

	// Get String
	if( (*string)[element.start] != '"' ) { return defaultValue; }
	if( (*string)[element.end-1] != '"' ) { return defaultValue; }
	return string->substr( element.start + 1, element.end - 1 ).replace( "\\\"", "\"" ).replace( "\\n", "\n" ).replace( "\\t", "\t" );
}


String JSON::GetStringAt( const usize index, const char *defaultValue )
{
	// Find Element
	JSONElement element = FindElementIndex( index );
	if( !element ) { return defaultValue; }

	// Get String
	if( (*string)[element.start] != '"' ) { return defaultValue; }
	if( (*string)[element.end-1] != '"' ) { return defaultValue; }
	return string->substr( element.start + 1, element.end - 1 ).replace( "\\\"", "\"" ).replace( "\\n", "\n" ).replace( "\\t", "\t" );
}


double JSON::GetDouble( const char *key, const double defaultValue )
{
	// Find Element
	JSONElement element = FindElementKey( key );
	if( !element ) { return defaultValue; }

	// Return Double
	return atof( string->substr( element.start, element.end ).c_str() );
}


double JSON::GetDoubleAt( const usize index, const double defaultValue )
{
	// Find Element
	JSONElement element = FindElementIndex( index );
	if( !element ) { return defaultValue; }

	// Return Double
	return atof( string->substr( element.start, element.end ).c_str() );
}


int JSON::GetInt( const char *key, const int defaultValue )
{
	// Find Element
	JSONElement element = FindElementKey( key );
	if( !element ) { return defaultValue; }

	// Return Integer
	return atoi( string->substr( element.start, element.end ).c_str() );
}


int JSON::GetIntAt( const usize index, const int defaultValue )
{
	// Find Element
	JSONElement element = FindElementIndex( index );
	if( !element ) { return defaultValue; }

	// Return Integer
	return atoi( string->substr( element.start, element.end ).c_str() );
}


bool JSON::GetBool( const char *key, const bool defaultValue )
{
	// Find Element
	JSONElement element = FindElementKey( key );
	if( !element ) { return defaultValue; }

	// Return Boolean
	return string->contains_at( "true", element.start ) || string->contains_at( "1", element.start );
}


bool JSON::GetBoolAt( const usize index, const bool defaultValue )
{
	// Find Element
	JSONElement element = FindElementIndex( index );
	if( !element ) { return defaultValue; }

	// Return Boolean
	return string->contains_at( "true", element.start ) || string->contains_at( "1", element.start );
}