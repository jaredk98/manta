#include <build/objects.hpp>

#include <build/build.hpp>
#include <build/fileio.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char *s_BASE_OBJECT_DEFINITION =
R"(
OBJECT( BASE_OBJECT )
ABSTRACT( true )

PUBLIC Object id;

EVENT_CREATE MANUAL
{
	/* do nothing */
}

EVENT_DESTROY MANUAL
{
	/* do nothing */
}
)";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char *g_KEYWORDS[] =
{
// Events
	"EVENT_CREATE",          // KeywordID_EVENT_CREATE
	"EVENT_DESTROY",         // KeywordID_EVENT_DESTROY
	"EVENT_INITIALIZE",      // KeywordID_EVENT_INITIALIZE

	"EVENT_FRAME_START",     // KeywordID_EVENT_FRAME_START
	"EVENT_FRAME_END",       // KeywordID_EVENT_FRAME_END

	"EVENT_STEP_CUSTOM",     // KeywordID_EVENT_STEP_CUSTOM
	"EVENT_STEP_GUI",        // KeywordID_EVENT_STEP_GUI
	"EVENT_STEP",            // KeywordID_EVENT_STEP

	"EVENT_DRAW_CUSTOM",     // KeywordID_EVENT_DRAW_CUSTOM
	"EVENT_DRAW_GUI",        // KeywordID_EVENT_DRAW_GUI
	"EVENT_DRAW",            // KeywordID_EVENT_DRAW

	"EVENT_SLEEP",           // KeywordID_EVENT_SLEEP
	"EVENT_WAKE",            // KeywordID_EVENT_WAKE

	"EVENT_FLAG",            // KeywordID_EVENT_FLAG
	"EVENT_PARTITION",       // KeywordID_EVENT_PARTITION
	"EVENT_UI_MASK",         // KeywordID_EVENT_UI_MASK

	"EVENT_SAVE",            // KeywordID_EVENT_SAVE
	"EVENT_LOAD",            // KeywordID_EVENT_LOAD

	"EVENT_NETWORK_SEND",    // KeywordID_EVENT_NETWORK_SEND
	"EVENT_NETWORK_RECEIVE", // KeywordID_EVENT_NETWORK_RECEIVE

// Keywords
	"HEADER_INCLUDES",       // KeywordID_HEADER_INCLUDES
	"SOURCE_INCLUDES",       // KeywordID_SOURCE_INCLUDES
	"OBJECT",                // KeywordID_OBJECT
	"PARENT",                // KeywordID_PARENT
	"COUNT",                 // KeywordID_COUNT
	"BUCKET_SIZE",           // KeywordID_BUCKET_SIZE
	"ABSTRACT",              // KeywordID_ABSTRACT
	"NOINHERIT",             // KeywordID_NOINHERIT
	"NETWORKED",             // KeywordID_NETWORKED
	"VERSION",               // KeywordID_VERSION
	"CONSTRUCTOR",           // KeywordID_CONSTRUCTOR
	"PRIVATE",               // KeywordID_PRIVATE
	"PUBLIC",                // KeywordID_PUBLIC
	"GLOBAL",                // KeywordID_GLOBAL
};
static_assert( ARRAY_LENGTH( g_KEYWORDS ) == KEYWORD_COUNT, "Verify g_KEYWORDS matches KeywordID enum" );

int g_KEYWORD_COUNT[KEYWORD_COUNT];

KeywordRequirements g_KEYWORD_REQUIREMENTS[] =
{
// Events
//  { Required, Max Count }
	{ false,    1 }, // KeywordID_EVENT_CREATE
	{ false,    1 }, // KeywordID_EVENT_DESTROY
	{ false,    1 }, // KeywordID_EVENT_INITIALIZE

	{ false,    1 }, // KeywordID_EVENT_FRAME_START
	{ false,    1 }, // KeywordID_EVENT_FRAME_END

	{ false,    1 }, // KeywordID_EVENT_STEP_CUSTOM
	{ false,    1 }, // KeywordID_EVENT_STEP_GUI
	{ false,    1 }, // KeywordID_EVENT_STEP

	{ false,    1 }, // KeywordID_EVENT_DRAW_CUSTOM
	{ false,    1 }, // KeywordID_EVENT_DRAW_GUI
	{ false,    1 }, // KeywordID_EVENT_DRAW

	{ false,    1 }, // KeywordID_EVENT_SLEEP
	{ false,    1 }, // KeywordID_EVENT_WAKE

	{ false,    1 }, // KeywordID_EVENT_FLAG
	{ false,    1 }, // KeywordID_EVENT_PARTITION
	{ false,    1 }, // KeywordID_EVENT_UI_MASK

	{ false,    1 }, // KeywordID_EVENT_SAVE
	{ false,    1 }, // KeywordID_EVENT_LOAD

	{ false,    1 }, // KeywordID_EVENT_NETWORK_SEND
	{ false,    1 }, // KeywordID_EVENT_NETWORK_RECEIVE

// Keywords
//  { Required, Max Count }
	{ false,   -1 }, // KeywordID_HEADER_INCLUDES
	{ false,   -1 }, // KeywordID_SOURCE_INCLUDES
	{  true,    1 }, // KeywordID_OBJECT
	{ false,    1 }, // KeywordID_PARENT
	{ false,    1 }, // KeywordID_COUNT
	{ false,    1 }, // KeywordID_BUCKET_SIZE
	{ false,    1 }, // KeywordID_ABSTRACT
	{ false,    1 }, // KeywordID_NOINHERIT
	{ false,    1 }, // KeywordID_NETWORKED
	{ false,    1 }, // KeywordID_VERSION
	{ false,   -1 }, // KeywordID_CONSTRUCTOR
	{ false,   -1 }, // KeywordID_PRIVATE
	{ false,   -1 }, // KeywordID_PUBLIC
	{ false,   -1 }, // KeywordID_GLOBAL
};
static_assert( ARRAY_LENGTH( g_KEYWORD_REQUIREMENTS ) == KEYWORD_COUNT, "Verify g_KEYWORD_REQUIREMENTS matches KeywordID enum" );

const char *g_EVENT_FUNCTIONS[][EVENT_FUNCTION_DEFINITION_COUNT] =
{
//  { EventFunction_Name       EventFunction_ReturnType  EventFunction_ReturnValue  EventFunction_Parameters  EventFunction_ParametersCaller
	{ "event_create",          "void",                   "",                        "()",                     "()"         }, // KeywordID_EVENT_CREATE
	{ "event_destroy",         "void",                   "",                        "()",                     "()"         }, // KeywordID_EVENT_DESTROY
	{ "event_initialize",      "void",                   "",                        "()",                     "()"         }, // KeywordID_EVENT_INITIALIZE

	{ "event_frame_start",     "void",                   "",                        "( const Delta delta )",  "( delta )"  }, // KeywordID_EVENT_FRAME_START
	{ "event_frame_end",       "void",                   "",                        "( const Delta delta )",  "( delta )"  }, // KeywordID_EVENT_FRAME_END

	{ "event_step_custom",     "void",                   "",                        "( const Delta delta )",  "( delta )"  }, // KeywordID_EVENT_STEP_CUSTOM
	{ "event_step_gui",        "void",                   "",                        "( const Delta delta )",  "( delta )"  }, // KeywordID_EVENT_STEP_GUI
	{ "event_step",            "void",                   "",                        "( const Delta delta )",  "( delta )"  }, // KeywordID_EVENT_STEP

	{ "event_draw_custom",     "void",                   "",                        "( const Delta delta )",  "( delta )"  }, // KeywordID_EVENT_DRAW_CUSTOM
	{ "event_draw_gui",        "void",                   "",                        "( const Delta delta )",  "( delta )"  }, // KeywordID_EVENT_DRAW_GUI
	{ "event_draw",            "void",                   "",                        "( const Delta delta )",  "( delta )"  }, // KeywordID_EVENT_DRAW

	{ "event_sleep",           "void",                   "",                        "( const Delta delta )",  "( delta )"  }, // KeywordID_EVENT_SLEEP
	{ "event_wake",            "void",                   "",                        "( const Delta delta )",  "( delta )"  }, // KeywordID_EVENT_WAKE
	{ "event_flag",            "void",                   "",                        "( const u64 code )",     "( code )"   }, // KeywordID_EVENT_FLAG
	{ "event_partition",       "void",                   "",                        "( void *ptr )",          "( ptr )"    }, // KeywordID_EVENT_PARTITION
	{ "event_ui_mask",         "int",                    "",                        "( void *ptr )",          "( ptr )"    }, // KeywordID_EVENT_UI_MASK

	{ "event_save",            "bool",                   "",                        "( byte *buffer )",       "( buffer )" }, // KeywordID_EVENT_SAVE
	{ "event_load",            "bool",                   "",                        "( byte *buffer )",       "( buffer )" }, // KeywordID_EVENT_LOAD

	{ "event_network_send",    "bool",                   "",                        "( byte *buffer )",       "( buffer )" }, // KeywordID_EVENT_NETWORK_SEND
	{ "event_network_receive", "bool",                   "",                        "( byte *buffer )",       "( buffer )" }, // KeywordID_EVENT_NETWORK_RECEIVE
};
static_assert( ARRAY_LENGTH( g_EVENT_FUNCTIONS ) == EVENT_COUNT, "Verify g_EVENT_FUNCTIONS matches EventID enum" );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static usize find_closing_brace( const String &buffer, usize start = 0, usize end = USIZE_MAX )
{
	// Nested scope traversal
	int depth = 0;
	while( true )
	{
		const usize nextOpen = buffer.find( "{", start, end );
		const usize nextClose = buffer.find( "}", start, end );
		if( nextOpen == nextClose ) { return USIZE_MAX; }

		if( nextOpen < nextClose )
		{
			depth++;
			start = nextOpen + 1;
		}
		else
		{
			depth--;
			if( depth <= -1 ) { return USIZE_MAX; }
			if( depth == 0 ) { return nextClose; }
			start = nextClose + 1;
		}
	}

	// Failure
	return USIZE_MAX;
}


static bool find_keyword_parentheses( const String &buffer, usize start, usize end, usize &outParenOpen, usize &outParenClose )
{
	outParenOpen = buffer.find( "(", start, end );
	if( outParenOpen > end ) { return false; }
	outParenClose = buffer.find( ")", start, end );
	if( outParenClose > end ) { return false; }
	if( outParenOpen >= outParenClose ) {  return false; }
	return true;
}


static bool char_is_keyword_delimiter( char c )
{
	return !( ( c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_' );
}


static int line_at( const String &buffer, const usize position )
{
	int line = 1;
	usize current = buffer.find( "\n", 0, position );
	while( current < position ) { line++; current = buffer.find( "\n", current + 1, position ); }
	return line;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ObjectFile::parse_keywords( const String &buffer )
{
	// Setup
	for( KeywordID keywordID = 0; keywordID < KEYWORD_COUNT; keywordID++ )
	{
		g_KEYWORD_COUNT[keywordID] = 0;
	}

	// Gather keywords
	const usize length = buffer.length();
	for( usize current = 0; current < length; current++ )
	{
		// Comments
		if( buffer[current] == '/' )
		{
			// Single line comment
			if( buffer.contains_at( "//", current ) )
			{
				current = buffer.find( "\n", current );
				if( current == USIZE_MAX ) { break; }
				continue;
			}

			// Block comment
			if( buffer.contains_at( "/*", current ) )
			{
				current = buffer.find( "*/", current );
				if( current == USIZE_MAX ) { break; }
				current++;
				continue;
			}
		}

		// Shortcut common chars
		if( buffer[current] < 'A' || buffer[current] > 'Z' ) { continue; }

		// Find top keyword
		Keyword keyword { 0, buffer.length(), buffer.length() };
		for( KeywordID keywordID = 0; keywordID < KEYWORD_COUNT; keywordID++ )
		{
			if( buffer.contains_at( g_KEYWORDS[keywordID], current ) )
			{
				keyword.start = current;
				keyword.id = keywordID;
				break;
			}
		}
		if( keyword.start == keyword.end ) { continue; }

		// Check to make sure the keyword is a valid, stand-alone word
		const char prevChar = keyword.start > 0 ? buffer[keyword.start - 1] : ' ';
		const char postChar = keyword.start + strlen( g_KEYWORDS[keyword.id] ) < buffer.length() ? buffer[keyword.start + strlen( g_KEYWORDS[keyword.id] )] : ' ';
		if( !char_is_keyword_delimiter( prevChar ) || !char_is_keyword_delimiter( postChar ) ) { continue; }

		// Adjust previous keyword end
		if( keywords.size() > 0 )
		{
			Keyword &keywordPrev = keywords[keywords.size() - 1];
			keywordPrev.end = keyword.start;
		}

		// Register Keyword
		keyword.start += strlen( g_KEYWORDS[keyword.id] );
		keywords.add( keyword );
		g_KEYWORD_COUNT[keyword.id]++;
		current = keyword.start - 1;
	}

	// Validate Keyword Requirements
	for( KeywordID keywordID = 0; keywordID < KEYWORD_COUNT; keywordID++ )
	{
		const KeywordRequirements &requirements = g_KEYWORD_REQUIREMENTS[keywordID];

		// Requirements
		if( requirements.required && g_KEYWORD_COUNT[keywordID] < requirements.count )
		{
			ErrorIf( true, "Object must have at least %d '%s'", requirements.count, g_KEYWORDS[keywordID], keywordID );
		}

		// Limit
		if( requirements.count != -1 && g_KEYWORD_COUNT[keywordID] > requirements.count )
		{
			ErrorIf( true, "Object can only have %d '%s'", requirements.count, g_KEYWORDS[keywordID] );
		}
	}
}


void ObjectFile::parse_keywords_values( const String &buffer )
{
	// Loop through keywords
	for( Keyword &keyword : keywords )
	{
		// Process Keyword
		switch( keyword.id )
		{
			// OBJECT
			case KeywordID_OBJECT:
			{
				name = keyword_PARENTHESES_string( buffer, keyword );
				ErrorIf( name.length() == 0, "%s() must be a valid string!", g_KEYWORDS[KeywordID_OBJECT] );
				type = name;
				type.append( "_t" );
			}
			break;

			// PARENT
			case KeywordID_PARENT:
			{
				nameParent = keyword_PARENTHESES_string( buffer, keyword );
				if( nameParent.length() == 0 ) { nameParent = "BASE_OBJECT"; }
				ErrorIf( nameParent.length() == 0, "%s() must be a valid string!", g_KEYWORDS[KeywordID_PARENT] );
				typeParent = nameParent;
				typeParent.append( "_t" );
				typeParentFull = "iobjects::";
				typeParentFull.append( typeParent );
			}
			break;

			// COUNT
			case KeywordID_COUNT:
			{
				count = keyword_PARENTHESES_string( buffer, keyword );
				if( count.length() == 0 ) { count = "-1"; }
				const int integerAssert = atoi( count );
				ErrorIf( integerAssert < -1, "%s() must be >= -1 (range: -1 to 2,147,483,647)", g_KEYWORDS[KeywordID_COUNT] );
				ErrorIf( integerAssert > 2147483647, "%s() must be <= 2147483647 (range: -1 to 2,147,483,647)", g_KEYWORDS[KeywordID_COUNT] );
			}
			break;

			// BUCKET_SIZE
			case KeywordID_BUCKET_SIZE:
			{
				bucketSize = keyword_PARENTHESES_string( buffer, keyword );
				if( bucketSize.length() == 0 ) { bucketSize = "-1"; }
				const int integerAssert = atoi( bucketSize );
				ErrorIf( integerAssert < 0, "%s() must be >= 0 (range: 0 to 65535)", g_KEYWORDS[KeywordID_BUCKET_SIZE] );
				ErrorIf( integerAssert > 65535, "%s() must be <= 65535 (range: 0 to 65535)", g_KEYWORDS[KeywordID_BUCKET_SIZE] );
			}
			break;

			// ABSTRACT
			case KeywordID_ABSTRACT:
			{
				abstract = keyword_PARENTHESES_bool( buffer, keyword );
			}
			break;

			// NOINHERIT
			case KeywordID_NOINHERIT:
			{
				noinherit = keyword_PARENTHESES_bool( buffer, keyword );
			}
			break;

			// NETWORKED
			case KeywordID_NETWORKED:
			{
				networked = keyword_PARENTHESES_bool( buffer, keyword );
			}
			break;

			// VERSION
			case KeywordID_VERSION:
			{
				version = keyword_PARENTHESES_int( buffer, keyword );
			}
			break;
		}
	}

	// If NETWORKED, require VERSION
	ErrorIf( networked && version == -1, "Objects with %s() must also have a specified %s()", g_KEYWORDS[KeywordID_NETWORKED], g_KEYWORDS[KeywordID_VERSION] );

	// If ABSTRACT, do not allow parent
	//AssertMsg( !( abstract && ( nameParent != "BASE_OBJECT" || nameParent != "" ) ), "ERROR: ABSTRACT objects cannot have a PARENT" );
}


void ObjectFile::parse_keywords_code( const String &buffer )
{
	// Loop through keywords
	for( Keyword &keyword : keywords )
	{
		// Event Keywords (KeywordID_EVENT_...)
		if( KEYWORD_IS_EVENT( keyword.id ) )
		{
			keyword_EVENT( buffer, keyword );
		}

		// Other Keywords
		switch( keyword.id )
		{
			// HEADER_INCLUDES / SOURCE_INCLUDES
			case KeywordID_HEADER_INCLUDES:
			case KeywordID_SOURCE_INCLUDES:
			{
				keyword_INCLUDES( buffer, keyword );
			}
			break;

			// CONSTRUCTOR
			case KeywordID_CONSTRUCTOR:
			{
				keyword_CONSTRUCTOR( buffer, keyword );
			}
			break;

			// PRIVATE / PUBLIC / GLOBAL
			case KeywordID_PRIVATE:
			case KeywordID_PUBLIC:
			case KeywordID_GLOBAL:
			{
				keyword_PRIVATE_PUBLIC_GLOBAL( buffer, keyword );
			}
			break;
		}
	}
}


void ObjectFile::keyword_INCLUDES( const String &buffer, Keyword &keyword )
{
	String &includes = keyword.id == KeywordID_HEADER_INCLUDES ? objects::headerIncludes : objects::sourceIncludes;

	usize start = buffer.find( "#i", keyword.start, keyword.end );
	while( start != USIZE_MAX )
	{
		const usize end = buffer.find( "\n", start );
		ErrorIf( end == USIZE_MAX, "%s #include has no endline (line: %d)", g_KEYWORDS[keyword.id], line_at( buffer, keyword.start ) );

		String include = buffer.substr( start, end );
		if( includes.find( include ) == USIZE_MAX ) { includes.append( include ).append( "\n" ); }

		if( end == USIZE_MAX ) { break; }
		start = buffer.find( "#i", start + 1, keyword.end );
	}
}


void ObjectFile::keyword_CONSTRUCTOR( const String &buffer, Keyword &keyword )
{
	// Detect function scope braces
	usize scopeEnd = find_closing_brace( buffer, keyword.start, keyword.end );
	ErrorIf( scopeEnd == USIZE_MAX, "'%s' does not have a valid scope (line: %d)", g_KEYWORDS[keyword.id], line_at( buffer, keyword.start ) );
	usize scopeStart = buffer.find( "{", keyword.start, keyword.end );
	ErrorIf( scopeStart == USIZE_MAX, "'%s' does not have valid scope (line: %d)", g_KEYWORDS[keyword.id], line_at( buffer, keyword.start ) );
	String scope = buffer.substr( scopeStart, scopeEnd + 1 );

	// Detect function arguments
	usize argsStart, argsEnd;
	ErrorIf( !find_keyword_parentheses( buffer, keyword.start, scopeStart, argsStart, argsEnd ),
	         "'%s' does not have valid arguments (line: %d)", g_KEYWORDS[keyword.id], line_at( buffer, keyword.start ) );
	String arguments = buffer.substr( argsStart + 1, argsEnd ).trim();
	ErrorIf( arguments.length() == 0, "'%s' must have arguments!" );


	// Source
	String source;
	source.append( "iobjects::" ).append( type ).append( "::" ).append( type ).append( "( " ).append( arguments ).append( " )\n" );
	source.append( scope ).append( "\n" );
	constructorSource.add( static_cast<String &&>( source ) );

	// Header
	String header;
	header.append( type ).append( "( " ).append( arguments ).append( " );" );
	constructorHeader.add( static_cast<String &&>( header ) );
}


void ObjectFile::keyword_EVENT( const String &buffer, Keyword &keyword )
{
	u8 eventID = keyword.id;
	events[eventID].has = true;

	// DISABLED event?
	usize disabled = buffer.find( "DISABLE", keyword.start, keyword.end );
	if( disabled != USIZE_MAX )
	{
		events[eventID].disabled = true;
		events[eventID].manual = true;
		keyword_EVENT_NULL( buffer, keyword );
		return;
	}

	// MANUAL event?
	usize manual = buffer.find( "MANUAL", keyword.start, keyword.end );
	events[eventID].manual = manual != USIZE_MAX;

	// Detect function scope braces
	keyword.end = find_closing_brace( buffer, keyword.start, keyword.end );
	ErrorIf( keyword.end == USIZE_MAX, "'%s' does not have a valid scope (line: %d)", g_KEYWORDS[keyword.id], line_at( buffer, keyword.start ) );
	keyword.start = buffer.find( "{", keyword.start, keyword.end );
	ErrorIf( keyword.start == USIZE_MAX, "'%s' does not have valid scope (line: %d)", g_KEYWORDS[keyword.id], line_at( buffer, keyword.start ) );

	// Source
	String eventSource;
	eventSource.append( g_EVENT_FUNCTIONS[eventID][EventFunction_ReturnType] );
	eventSource.append( " iobjects::" );
	eventSource.append( type );
	eventSource.append( "::" );
	eventSource.append( g_EVENT_FUNCTIONS[eventID][EventFunction_Name] );
	eventSource.append( g_EVENT_FUNCTIONS[eventID][EventFunction_Parameters] );
	eventSource.append( "\n" );
	eventSource.append( buffer.substr( keyword.start, keyword.end + 1 ).trim() );
	eventSource.append( "\n" );
	eventsSource.add( static_cast<String &&>( eventSource ) );

	// Header
	String eventHeader;
	eventHeader.append( g_EVENT_FUNCTIONS[eventID][EventFunction_ReturnType] ).append( " " );
	eventHeader.append( g_EVENT_FUNCTIONS[eventID][EventFunction_Name] );
	eventHeader.append( g_EVENT_FUNCTIONS[eventID][EventFunction_Parameters] ).append( ";" );
	eventsHeader.add( static_cast<String &&>( eventHeader ) );
}


void ObjectFile::keyword_EVENT_NULL( const String &buffer, Keyword &keyword )
{
	// Header -- null events are simply inline, private, and empty scope (prevents inheritance)
	String eventDisabled;
	u8 eventID = keyword.id;
	eventDisabled.append( "inline " );
	eventDisabled.append( g_EVENT_FUNCTIONS[eventID][EventFunction_ReturnType] );
	eventDisabled.append( " " );
	eventDisabled.append( g_EVENT_FUNCTIONS[eventID][EventFunction_Name] );
	eventDisabled.append( g_EVENT_FUNCTIONS[eventID][EventFunction_Parameters] );
	eventDisabled.append( " { " );
	eventDisabled.append( strlen( g_EVENT_FUNCTIONS[eventID][EventFunction_ReturnValue] ) != 0 ? "return " : "return" );
	eventDisabled.append( g_EVENT_FUNCTIONS[eventID][EventFunction_ReturnValue] );
	eventDisabled.append( "; }" );
	eventsDisabled.add( static_cast<String &&>( eventDisabled ) );
}


void ObjectFile::keyword_PRIVATE_PUBLIC_GLOBAL( const String &buffer, Keyword &keyword )
{
	usize start = keyword.start;
	usize end = keyword.end;

	// Determine if keyword is for a function or data
	bool isFunction = true;
	const usize semicolon = buffer.find( ";", start, end );
	const usize openingBrace = buffer.find( "{", start, end );
	const usize closingBrace = find_closing_brace( buffer, start, end );
	if( closingBrace == USIZE_MAX ) { isFunction = false; } else
	if( closingBrace > end ) { isFunction = false; } else
	if( semicolon < openingBrace ) { isFunction = false; } else
	if( buffer.contains_at( "};", closingBrace ) ) { isFunction = false; }

	// PUBLIC / PRIVATE
	if( keyword.id == KeywordID_PUBLIC || keyword.id == KeywordID_PRIVATE )
	{
		// FUNCTION
		if( isFunction )
		{
			// Output
			List<String> &header = keyword.id == KeywordID_PRIVATE ? privateFunctionHeader : publicFunctionHeader;
			List<String> &source = keyword.id == KeywordID_PRIVATE ? privateFunctionSource : publicFunctionSource;

			// Update 'end'
			end = closingBrace + 1;

			// Copy function
			String functionSource = buffer.substr( start, end ).trim();
			const usize openBrace = buffer.find( "{", start, end  );
			ErrorIf( openBrace == USIZE_MAX, "Invalid function declaration: invalid/missing scope (line: %d)", line_at( buffer, start ) );
			String functionHeader = buffer.substr( start, openBrace ).trim();
			String functionInherited = functionHeader;

			// Format source function (e.g. source.cpp: "int OBJECT::foo() { ... }" )
			functionSource.append( "\n" );
			usize current = functionSource.find( "(", 0, functionSource.find( "{" ) ) - 1;
			ErrorIf( current == USIZE_MAX - 1, "Invalid function declaration: invalid/missing parameter parentheses (line: %d)", line_at( buffer, start ) );
			bool onName = false;
			while( true )
			{
				ErrorIf( current == 0, "Invalid function--no return type (line: %d)", line_at( buffer, keyword.start ) );
				const bool whitespace = char_whitespace( functionSource[current] );
				if( whitespace && onName ) { current++; break; }
				current--;
				onName |= !whitespace;
			}
			functionSource.insert( current, "::" );
			functionSource.insert( current, type );
			functionSource.insert( current, "iobjects::" );

			// Format header function (e.g. header.hpp: "int foo();")
			functionHeader.append( ";" );

			// Register functions
			source.add( static_cast<String &&>( functionSource ) );
			header.add( static_cast<String &&>( functionHeader ) );
		}
		else
		// VARIABLE / DATA
		{
			// Output
			List<String> &header = keyword.id == KeywordID_PRIVATE ? privateVariableHeader : publicVariableHeader;

			// Update 'end'
			end = buffer.find( ";", start, end );
			ErrorIf( end == USIZE_MAX, "Invalid declaration: missing terminating semicolon (line: %d)", line_at( buffer, keyword.start ) );
			end++;

			// Variable?
			const bool isVariable = !( buffer.find( "struct", start, end ) != USIZE_MAX ||
									   buffer.find( "class", start, end ) != USIZE_MAX ||
									   buffer.find( "enum", start, end ) != USIZE_MAX ||
									   buffer.find( "enum_type", start, end ) != USIZE_MAX );
			if( !isVariable )
			{
				end = find_closing_brace( buffer, start );
				ErrorIf( end == USIZE_MAX, "Invalid declaration: missing scope (line: %d)", line_at( buffer, keyword.start ) );
				end += 2;
			}

			// Copy expression
			String expression = buffer.substr( start, end ).trim();

			// Format expression
			expression.replace( "\n", "\n\t" );

			// Register expression
			header.add( static_cast<String &&>( expression ) );
		}
	}
	else
	// GLOBAL
	{
		// FUNCTION
		if( isFunction )
		{
			// Output
			List<String> &source = globalFunctionSource;
			List<String> &header = globalFunctionHeader;

			// Update 'end'
			end = closingBrace + 1;

			// Copy function
			String functionSource = buffer.substr( start, end ).trim();

			// Format source function (e.g. source.cpp: "int foo() { ... }" )
			functionSource.append( "\n" );

			// Format header function (e.g. header.hpp: "extern int foo();" )
			const usize openBrace = functionSource.find( "{" );
			ErrorIf( openBrace == USIZE_MAX, "Invalid function declaration: invalid/missing scope (line: %d)", line_at( buffer, keyword.start ) );
			String functionHeader = functionSource.substr( 0, openBrace ).trim();
			functionHeader.insert( 0, "extern " );
			functionHeader.append( ";\n" );

			// Register functions
			source.add( static_cast<String &&>( functionSource ) );
			header.add( static_cast<String &&>( functionHeader ) );
		}
		else
		// VARIABNLE / DATA
		{
			// Output
			List<String> &source = globalVariableSource;
			List<String> &header = globalVariableHeader;

			// Update 'end'
			end = buffer.find( ";", start );
			ErrorIf( end == USIZE_MAX, "Invalid declaration: missing terminating ';' (line %d)", line_at( buffer, keyword.start ) );
			end++;

			// Variable?
			const bool isVariable = !( buffer.find( "struct", start, end ) != USIZE_MAX ||
									   buffer.find( "class", start, end ) != USIZE_MAX ||
									   buffer.find( "enum", start, end ) != USIZE_MAX ||
									   buffer.find( "enum_type", start, end ) != USIZE_MAX );
			if( !isVariable )
			{
				end = find_closing_brace( buffer, start );
				ErrorIf( end == USIZE_MAX, "Invalid declaration: missing scope (line: %d)", line_at( buffer, keyword.start ) );
				end += 2;
			}

			// Copy expression
			String expressionSource = buffer.substr( start, end ).trim();
			String expressionHeader = expressionSource;

			// Variable expression
			if( isVariable )
			{
				// Format source expression (e.g. source.cpp: "int g_Variable = 10;" )
				expressionSource.append( "\n" );

				// Format header expression (e.g. header.hpp: "extern int g_Variable;")
				const usize endBrace = expressionHeader.find( "{" );
				const usize endSemicolon = expressionHeader.find( ";" );
				const usize endAssignment = expressionHeader.find( "=" );
				ErrorIf( endAssignment == USIZE_MAX && endSemicolon == USIZE_MAX && endSemicolon == USIZE_MAX,
					"Invalid declaration: missing '{ }', '=', or ';'", keyword.start, keyword.end );
				const usize endIndex = min( endBrace, min( endSemicolon, endAssignment ) );
				expressionHeader.remove( endIndex, expressionHeader.length() - endIndex );
				expressionHeader.trim();
				expressionHeader.insert( 0, "extern " );
				expressionHeader.append( ";\n" );

				// Register expressions
				source.add( static_cast<String &&>( expressionSource ) );
				header.add( static_cast<String &&>( expressionHeader ) );
			}
			else
			// Type expression (struct, class, enum, etc.)
			{
				// Format header expression
				expressionHeader.insert( 0, "\n" );
				expressionHeader.append( "\n" );

				// Register expression
				header.add( static_cast<String &&>( expressionHeader ) );
			}
		}
	}
}


String ObjectFile::keyword_PARENTHESES_string( const String &buffer, Keyword &keyword, const bool requireParentheses )
{
	// Find parentheses
	usize open = 0;
	usize close = 0;
	const bool found = find_keyword_parentheses( buffer, keyword.start, keyword.end, open, close );
	if( found ) { return buffer.substr( open + 1, close ).trim(); }
	ErrorIf( requireParentheses, "Keyword '%s' requires parentheses (line %d)", g_KEYWORDS[keyword.id], line_at( buffer, keyword.start ) );
	return "";
}


int ObjectFile::keyword_PARENTHESES_int( const String &buffer, Keyword &keyword, const bool requireParentheses )
{
	const String string = keyword_PARENTHESES_string( buffer, keyword );
	return string.length() == 0 ? 0 : atoi( string.c_str() );
}


double ObjectFile::keyword_PARENTHESES_double( const String &buffer, Keyword &keyword, const bool requireParentheses )
{
	const String string = keyword_PARENTHESES_string( buffer, keyword );
	return string.length() == 0 ? 0.0 : atof( string.c_str() );
}


float ObjectFile::keyword_PARENTHESES_float( const String &buffer, Keyword &keyword, const bool requireParentheses )
{
	const String string = keyword_PARENTHESES_string( buffer, keyword );
	return string.length() == 0 ? 0.0f : static_cast<float>( atof( string.c_str() ) );
}


bool ObjectFile::keyword_PARENTHESES_bool( const String &buffer, Keyword &keyword, const bool requireParentheses )
{
	const String string = keyword_PARENTHESES_string( buffer, keyword );
	if( string.length() == 0 ) { return true; }
	if( string == "true" || string == "1" ) { return true; }
	if( string == "false" || string == "0" ) { return false; }
	ErrorIf( true, "Invalid boolean parentheses for keyword (%s) (line %d)", string.c_str(), line_at( buffer, keyword.start ) );
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ObjectFile::parse()
{
	// Buffer
	String buffer;

	// Skip BASE_OBJECT
	if( name == "BASE_OBJECT" )
	{
		// Read from s_BASE_OBJECT_DEFINITION
		buffer = s_BASE_OBJECT_DEFINITION;
	}
	else
	{
		// Read from file
		ErrorIf( !buffer.load( path ), "Failed to open object file: %s", path.c_str() );
	}

	// Console
	if( verbose_output() ) { PrintColor( LOG_CYAN, "\t\tParse %s", path.length() == 0 ? "BASE_OBJECT" : path.c_str() ); }
	Timer timer;
	{
		// Parse Keywords
		parse_keywords( buffer );

		// Process "Read" Keywords
		parse_keywords_values( buffer );

		// Write Keywords
		parse_keywords_code( buffer );
	}
	if( verbose_output() ) { PrintLnColor( LOG_WHITE, " (%.3f ms)", timer.elapsed_ms() ); }
}


void ObjectFile::write_header()
{
	// Header
	String &output = objects::header;

	// Header Break
	output.append( "////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\n\n" );

	// GLOBAL VARIABLE / DATA
	if( globalVariableHeader.size() > 0 )
	{
		for( String &str : globalVariableHeader ) { output.append( str ); }
		output.append( "\n" );
	}

	// OBJECT_DECLARATION_BEGIN
	output.append( "__INTERNAL_OBJECT_DEFINITION_BEGIN()\n" );
	output.append( "class " );
	output.append( type );
	if( parent != nullptr )
	{
		output.append( " : public " );
		output.append( typeParent );
	}
	output.append( "\n{\n" );
	{
		// PUBLIC
		output.append( "public:\n" );
		{
			// Constructors
			output.append( "\t" );
			output.append( type );
			output.append( "() = default;\n" );

			if( constructorHeader.size() > 0 )
			{
				for( String &str : constructorHeader ) { output.append( "\t" ).append( str ).append( "\n" ); }
			}
			output.append( "\n" );

			// Public Variable
			if( publicVariableHeader.size() > 0 )
			{
				output.append( "\t// PUBLIC DATA\n" );
				for( String &str : publicVariableHeader )
				{
					// Only write members that are unique to this object (factor inheritance of members)
					if( parent == nullptr || !( parent->inheritedVariables.contains( str ) ) )
					{
						output.append( "\t" ).append( str ).append( "\n" );
					}
				}
				output.append( "\n" );
			}

			// Public Functions
			if( publicFunctionHeader.size() > 0 )
			{
				output.append( "\t// PUBLIC FUNCTIONS\n" );
				for( String &str : publicFunctionHeader ) { output.append( "\tvirtual " ).append( str ).append( "\n" ); }
				output.append( "\n" );
			}

			// Events
			if( eventsHeader.size() > 0 )
			{
				output.append( "\t// EVENTS\n" );
				for( String &str : eventsHeader ) { output.append( "\tvirtual " ).append( str ).append( "\n" ); }
			}
		}

		// Private
		output.append( "private:\n" );
		{
			// Private Variables
			if( privateVariableHeader.size() > 0 )
			{
				output.append( "\t// PRIVATE DATA\n" );
				for( String &str : privateVariableHeader )
				{
					// Only write members that are unique to this object (factor inheritance of members)
					if( parent == nullptr || !( parent->inheritedVariables.contains( str ) ) )
					{
						output.append( "\t" ).append( str ).append( "\n" );
					}
				}
				output.append( "\n" );
			}

			// Private Functions
			if( privateFunctionHeader.size() > 0 )
			{
				output.append( "\t// PRIVATE FUNCTIONS\n" );
				for( String &str : privateFunctionHeader ) { output.append( "\t" ).append( str ).append( "\n" ); }
				output.append( "\n" );
			}

			// Disabled Events
			if( eventsDisabled.size() > 0 )
			{
				output.append( "\t// DISABLED EVENTS\n" );
				for( String &str : eventsDisabled ) { output.append( "\t" ).append( str ).append( "\n" ); }
			}
		}
	}
	output.append( "};\n" );
	output.append( "__INTERNAL_OBJECT_DEFINITION_END()\n\n" );

	// Object Constructor
	if( constructorHeader.size() > 0 )
	{
		output.append( "template <typename... Args> struct iobjects::object_constructor<" ).append( name ).append( ", Args...>\n{\n" );
		output.append( "\tstatic void construct( Args... args )\n\t{\n" );
		output.append( "\t\tnew ( iobjects::OBJECT_CTOR_MANUAL_BUFFER + iobjects::OBJECT_CTOR_BUFFER_OFFSET[" );
		output.append( name ).append( "] ) iobjects::" ).append( type ).append( "( args... );\n\t}\n};\n\n" );
	}

	// ObjectHandle
	output.append( "template <> struct ObjectHandle<" ).append( name ).append( ">\n{\n" );
	output.append( "\tiobjects::" ).append( type ).append( " *data = nullptr;\n" );
	output.append( "\tiobjects::" ).append( type ).append( " *operator->() const { Assert( data != nullptr ); return data; }\n" );
	output.append( "\texplicit operator bool() const { return data != nullptr; }\n" );
	output.append( "\tObjectHandle( void *object ) { data = reinterpret_cast<iobjects::" ).append( type ).append( " *>( object ); }\n" );
	output.append( "};\n\n" );

	// Global Functions
	if( globalFunctionHeader.size() > 0 )
	{
		for( String &str : globalFunctionHeader ) { output.append( str ); }
		output.append( "\n" );
	}
	output.append( "\n" );
}


void ObjectFile::write_handle()
{
	String &output = objects::source;
	output.append( "template <> ObjectHandle<" ).append( name ).append( "> Object::handle<" ).append( name );
	output.append( ">( const ObjectContext &context ) const\n{\n" );
	output.append( "\treturn { context.object_pointer( *this ) };\n}\n\n" );
}


void ObjectFile::write_source()
{
	// Header
	String &output = objects::source;
	bool hasParent = ( nameParent != "BASE_OBJECT" );

	// Header Break
	output.append( "////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\n\n" );

	// Global Data
	for( String &str : globalVariableSource ) { output.append( str ).append( "\n" ); }

	// Constructors
	for( String &str : constructorSource )
	{
		output.append( str ).append( "\n" );
	}

	// Events
	for( String &str : eventsSource )
	{
		if( hasParent ) { str.replace( "INHERIT", typeParentFull ); }
		output.append( str ).append( "\n" );
	}

	// Private Functions
	for( String &str : privateFunctionSource )
	{
		if( hasParent ) { str.replace( "INHERIT", typeParentFull ); }
		output.append( str ).append( "\n" );
	}

	// Public Functions
	for( String &str : publicFunctionSource )
	{
		if( hasParent ) { str.replace( "INHERIT", typeParentFull ); }
		output.append( str ).append( "\n" );
	}

	// Global Functions
	for( String &str : globalFunctionSource ) { output.append( str ).append( "\n" ); }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace objects
{
	// Output Paths
	char pathSource[PATH_SIZE];
	char pathHeader[PATH_SIZE];
	char pathIntelliSense[PATH_SIZE];
	FileTime timeCache;

	// Output Contents
	String intellisense;
	String headerIncludes;
	String sourceIncludes;
	String header;
	String source;

	// Object Files
	List<ObjectFile> objectFiles;
	List<ObjectFile *> objectFilesSorted; // TODO: don't store pointers?
	usize objectFilesCount = 0;
}


void objects::begin()
{
	// BASE_OBJECT
	ObjectFile object { "" };
	object.name = "BASE_OBJECT"; // Initialize system with BASE_OBJECT
	object.nameParent = "";
	objectFiles.add( object );

	// Output Paths
	strjoin( pathIntelliSense, build::pathOutput, SLASH "generated" SLASH "objects.generated.intellisense" );
	strjoin( pathHeader, build::pathOutput, SLASH "generated" SLASH "objects.generated.hpp" );
	strjoin( pathSource, build::pathOutput, SLASH "generated" SLASH "objects.generated.cpp" );

	// Cache
	FileTime timeIntelliSense;
	if( !file_time( pathIntelliSense, &timeIntelliSense ) ) { build::cacheDirtyObjects = true; return; }
	FileTime timeHeader;
	if( !file_time( pathHeader, &timeHeader ) ) { build::cacheDirtyObjects = true; return; }
	FileTime timeSource;
	if( !file_time( pathSource, &timeSource ) ) { build::cacheDirtyObjects = true; return; }

	timeCache = file_time_newer( timeIntelliSense, timeHeader ) ? timeIntelliSense : timeHeader;
	timeCache = file_time_newer( timeCache, timeSource ) ? timeCache : timeSource;
}


u32 objects::gather( const char *directory, const bool recurse )
{
	// Iterate Directories
	Timer timer;
	List<FileInfo> objectFilesDisk;
	directory_iterate( objectFilesDisk, directory, ".opp", true );

	// Add Objects
	for( FileInfo &objectFile : objectFilesDisk )
	{
		// Check Cache
		if( !build::cacheDirtyObjects )
		{
			FileTime timeObject;
			file_time( objectFile.path, &timeObject );
			build::cacheDirtyObjects |= file_time_newer( timeObject, timeCache );
		}

		// Add Object
		objectFiles.add( { objectFile.path } );
		objectFilesCount++;
	}

	const u32 objectCount = objectFilesDisk.size();
	if( verbose_output() )
	{
		PrintColor( LOG_CYAN, "\t\t%u object%s found in: %s", objectCount, objectCount == 1 ? "" : "s", directory );
		PrintLnColor( LOG_WHITE, " (%.3f ms)", timer.elapsed_ms() );
	}
	return objectCount;
}


void objects::parse()
{
	// Parse Objects
	for( ObjectFile &object : objectFiles ) { object.parse(); }
}


void objects::resolve()
{
	if( verbose_output() ) { PrintColor( LOG_CYAN, "\t\tResolve Inheritance" ); }
	Timer timer;

	// Resolve object dependencies (build inheritance N-tree)
	for( ObjectFile &object : objectFiles )
	{
		// Search for parent
		if( object.nameParent.length() == 0 ) { continue; }
		for( ObjectFile &parent : objectFiles )
		{
			// Parent found?
			if( object.nameParent == parent.name )
			{
				// Don't allow self-inheritance
				ErrorIf( parent.name == object.name, "OBJECT(%s) attempting to inherit itself\n\t > %s", object.name.c_str(), object.path.c_str() );

				// Don't allow inheritance of no-inherit objects
				ErrorIf( parent.noinherit, "OBJECT(%s) trying to inherit %s(%s) marked as NOINHERIT()\n\t > %s",
					object.name.c_str(), g_KEYWORDS[KeywordID_PARENT], parent.name.c_str(), object.path.c_str() );

				// Success
				object.parent = &parent;
				parent.children.add( &object );
				break;
			}
		}

		// Make sure we resolved a parent
		ErrorIf( object.parent == nullptr, "No %s(%s) exists\n\t > %s",
			g_KEYWORDS[KeywordID_PARENT], object.nameParent.c_str(), object.path.c_str() );
	}

	// Sort objects based on inheritance depth
	objectFilesSorted.add( &objectFiles[0] ); // BASE_OBJECT
	objectFiles[0].depth = 0;
	objectFiles[0].visited = true;
	sort_objects( &objectFiles[0], 1, objectFilesSorted );

	// Inherit variables, functions, & events
	for( ObjectFile &object : objectFiles )
	{
		ObjectFile *parent = object.parent;
		while( parent != nullptr )
		{
			// Variables
			for( String &variable : parent->publicVariableHeader )
			{
				if( !( object.inheritedVariables.contains( variable ) ) )
				{
					object.inheritedVariables.add( variable );
				}
			}

			// Functions
			for( String &function : parent->publicFunctionHeader )
			{
				if( !( object.inheritedFunctions.contains( function ) ) )
				{
					object.inheritedFunctions.add( function );
				}
			}

			// Events
			for( String &event : parent->eventsHeader )
			{
				if( !( object.inheritedEvents.contains( event ) ) )
				{
					object.inheritedEvents.add( event );
				}
			}

			// Continue loop
			parent = parent->parent;
		}
	}

	// Log
	if( verbose_output() ) { PrintLnColor( LOG_WHITE, " (%.3f ms)", timer.elapsed_ms() ); }
};


void objects::sort_objects( ObjectFile *object, const u16 depth, List<ObjectFile *> &outList )
{
	// This performs a "topological sort" of the ObjectFiles based on the inheritance tree
	// The output ensures that parents always precede children

	// Visit all children
	ErrorIf( depth == U16_MAX, "Exceeded maximum inheritance depth (%d)\n\t > %s", U16_MAX, object->path.c_str() );
	for( ObjectFile *child : object->children )
	{
		// Skip already visited children
		if( child->visited ) { continue; }

		// Set inheritance depth
		child->depth = depth;
		child->visited = true;

		// Propogate events
		for( u8 eventID = 0; eventID < EVENT_COUNT; eventID++ )
		{
			Event &myEvent = object->events[eventID];
			Event &childEvent = child->events[eventID];

			if( ( myEvent.has && !myEvent.disabled ) && !childEvent.has )
			{
				childEvent.has = true;
				childEvent.manual = myEvent.manual;
			}
		}

		// Add child & recurse
		outList.add( child );
		if( child->children.size() > 0 ) { sort_objects( child, depth + 1, outList ); }
	}
}


void objects::generate()
{
	// Generates C++ source & header contents including:
	//     - Class definitions and member function implementations for each object type
	//     - Boilerplate datastructures & functions necessary for the object system (manta/objects.hpp)

	// IntelliSense
	generate_intellisense( objects::intellisense );

	// Header
	generate_header( objects::header );

	// Source
	generate_source( objects::source );
}


void objects::generate_intellisense( String &output )
{
	if( verbose_output() ) { PrintColor( LOG_CYAN, "\t\tGenerate output/generated/objects.generated.intellisense" ); }
	Timer timer;

	// File Info
	output.append( "/*\n" );
	output.append( " * File generated by build.exe--do not edit!\n" );
	output.append( " * Refer to: source/build/objects.cpp (objects::GenerateIntelliSense)\n" );
	output.append( " *\n" );
	output.append( " * This header should only be included in manta/source/objects_api.hpp\n" );
	output.append( " * The purpose of this file is to provide IntelliSense and syntax highlighting for inherited objects\n" );
	output.append( " *\n" );
	output.append( " * This file is NOT compiled into the runtime executable!\n" );
	output.append( " */\n" );

	// #pragma once
	output.append( "#pragma once\n\n\n" );

	// IntelliSense
	for( ObjectFile *object : objectFilesSorted )
	{
		output.append( "////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\n\n" );
		output.append( "namespace iobjects::ObjectIntelliSense_" ).append( object->name ).append( "\n{\n" );

		// Inherited Variables
		for( String &variable : object->inheritedVariables )
		{
			output.append( "\t" ).append( variable ).append( "\n" );
		}

		// Inherited Functions
		for( String &function : object->inheritedFunctions )
		{
			output.append( "\t" ).append( function ).append( "\n" );
		}

		// Inherited Events
		for( String &event : object->inheritedEvents )
		{
			output.append( "\t" ).append( event ).append( "\n" );
		}

		output.append( "}\n\n" );
	}

	if( verbose_output() ) { PrintLnColor( LOG_WHITE, " (%.3f ms)", timer.elapsed_ms() ); }
}


void objects::generate_header( String &output )
{
	// Log
	if( verbose_output() ) { PrintColor( LOG_CYAN, "\t\tGenerate output/generated/objects.generated.hpp" ); }
	Timer timer;

	// File Info
	output.append( "/*\n" );
	output.append( " * File generated by build.exe--do not edit!\n" );
	output.append( " * Refer to: source/build/objects.cpp (objects::GenerateHeader)\n" );
	output.append( " *\n" );
	output.append( " * This header should only be included in source/manta/objects.hpp\n" );
	output.append( " */\n" );

	// #pragma once
	output.append( "#pragma once\n\n" );

	// OBJECT SYSTEM INCLUDES
	output.append( "// OBJECT SYSTEM INCLUDES\n" );
	output.append( "#include <vendor/new.hpp>\n" );
	output.append( "#include <manta/objects.hpp>\n" );
	output.append( "#include <types.hpp>\n" );
	output.append( "\n" );

	// HEADER_INCLUDES
	output.append( "// HEADER_INCLUDES\n" );
	output.append( objects::headerIncludes );
	output.append( objects::headerIncludes.length() == 0 ? "// ...\n\n" : "\n\n" );

	// Object System Internals
	generate_header_metadata( output );

	// Object Type IDs
	output.append( "////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\n\n" );
	output.append( "enum\n{\n" );
	for( ObjectFile *object : objectFilesSorted )
	{
		output.append( "\t" );
		output.append( object->name );
		output.append( ",\n" );
	}
	output.append( "\tOBJECT_TYPE_COUNT,\n" );
	output.append( "};\n\n" );

	// Object Class Definitions
	for( ObjectFile *object : objectFilesSorted ) { object->write_header(); }

	// Log
	if( verbose_output() ) { PrintLnColor( LOG_WHITE, " (%.3f ms)", timer.elapsed_ms() ); }
}


void objects::generate_header_metadata( String &output )
{
	output.append( "////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\n\n" );
	output.append( "namespace iobjects\n{\n" );
	{
		output.append( "\textern byte *OBJECT_CTOR_MANUAL_BUFFER;\n" );
		output.append( "\textern byte *OBJECT_CTOR_DEFAULT_BUFFER;\n" );
		output.append( "\textern u32 OBJECT_CTOR_BUFFER_SIZE;\n" );
		output.append( "\textern u32 OBJECT_CTOR_BUFFER_OFFSET[];\n\n" );
		output.append( "\textern const u16 OBJECT_TYPE_BUCKET_CAPACITY[];\n" );
		output.append( "\textern const i32 OBJECT_TYPE_MAX_COUNT[];\n" );
		output.append( "\textern const u16 OBJECT_TYPE_INHERITANCE_DEPTH[];\n" );
		output.append( "\textern const u16 OBJECT_TYPE_SIZE[];\n" );

		output.append( "#if COMPILE_DEBUG\n" );
		output.append( "\textern const char *OBJECT_TYPE_NAME[];\n" );
		output.append( "#endif\n\n" );

		output.append( "\ttemplate <int N, typename... Args> struct object_constructor;\n\n" );

		output.append( "\textern bool init();\n" );
	}
	output.append( "}\n\n" );
}


void objects::generate_source( String &output )
{
	// Log
	if( verbose_output() ) { PrintColor( LOG_CYAN, "\t\tGenerate output/generated/objects.generated.cpp" ); }
	Timer timer;

	// File Info
	output.append( "/*\n" );
	output.append( " * File generated by build.exe--do not edit!\n" );
	output.append( " * Refer to: source/build/objects.cpp (objects::GenerateSource)\n" );
	output.append( " *\n" );
	output.append( " * Provides implementations for source/manta/objects.hpp internals and output/generated/objects.generated.hpp\n" );
	output.append( " */\n\n" );

	// OBJECT SYSTEM INCLUDES
	output.append( "// OBJECT SYSTEM INCLUDES\n" );
	output.append( "#include <manta/objects.hpp>\n" );
	output.append( "#include <manta/memory.hpp>\n" );
	output.append( "#include <vendor/new.hpp>\n" );
	output.append( "\n" );

	// SOURCE_INCLUDES
	output.append( "// SOURCE_INCLUDES\n" );
	output.append( objects::sourceIncludes );
	output.append( objects::sourceIncludes.length() == 0 ? "// ...\n\n" : "\n\n" );

	// Safety Defines (INHERIT)
	output.append( "////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\n\n" );
	output.append( "#define INHERIT\n\n" );

	// Object System Internals
	generate_source_metadata( output );

	// ObjectContext Events
	generate_source_events( output );

	// Object handle<...>
	output.append( "////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\n\n" );
	for( ObjectFile *object : objectFilesSorted ) { object->write_handle(); }

	// Object Classes
	for( ObjectFile *object : objectFilesSorted ) { object->write_source(); }

	// Log
	if( verbose_output() ) { PrintLnColor( LOG_WHITE, " (%.3f ms)", timer.elapsed_ms() ); }
};


void objects::generate_source_metadata( String &output )
{
	output.append( "////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\n\n" );

	output.append( "byte *iobjects::OBJECT_CTOR_MANUAL_BUFFER = nullptr;\n" );
	output.append( "byte *iobjects::OBJECT_CTOR_DEFAULT_BUFFER = nullptr;\n" );
	output.append( "u32 iobjects::OBJECT_CTOR_BUFFER_SIZE = 0;\n" );
	output.append( "u32 iobjects::OBJECT_CTOR_BUFFER_OFFSET[OBJECT_TYPE_COUNT];\n\n" );

	// OBJECT_BUCKET_CAPACITY
	output.append( "const u16 iobjects::OBJECT_TYPE_BUCKET_CAPACITY[OBJECT_TYPE_COUNT] =\n{\n\t" );
	for( usize i = 0, j = 0; i < objectFilesSorted.size(); i++, j++ )
	{
		ObjectFile &object = *objectFilesSorted[i];
		output.append( object.abstract ? "0" : object.bucketSize.c_str() );
		output.append( ( j % 7 == 0 && i != 0 && i != objectFilesSorted.size() - 1 ) ? ",\n\t" : ", " );
	}
	output.append( "\n};\n\n" );

	// OBJECT_TYPE_MAX_COUNT
	output.append( "const i32 iobjects::OBJECT_TYPE_MAX_COUNT[OBJECT_TYPE_COUNT] =\n{\n\t" );
	for( usize i = 0, j = 0; i < objectFilesSorted.size(); i++, j++ )
	{
		ObjectFile &object = *objectFilesSorted[i];
		output.append( object.abstract ? "0" : object.count.c_str() );
		output.append( ( j % 7 == 0 && j != 0 && i != objectFilesSorted.size() - 1 ) ? ",\n\t" : ", " );
	}
	output.append( "\n};\n\n" );

	// OBJECT_TYPE_INHERITANCE_DEPTH
	output.append( "const u16 iobjects::OBJECT_TYPE_INHERITANCE_DEPTH[OBJECT_TYPE_COUNT] =\n{\n\t" );
	for( usize i = 0, j = 0; i < objectFilesSorted.size(); i++, j++ )
	{
		ObjectFile &object = *objectFilesSorted[i];
		char buffer[16];
		snprintf( buffer, sizeof( buffer ), "%d", object.depth );
		output.append( buffer );
		output.append( ( j % 7 == 0 && j != 0 && i != objectFilesSorted.size() - 1 ) ? ",\n\t" : ", " );
	}
	output.append( "\n};\n\n" );

	// OBJECT_TYPE_SIZE
	output.append( "const u16 iobjects::OBJECT_TYPE_SIZE[OBJECT_TYPE_COUNT] =\n{\n\t" );
	for( usize i = 0, j = 0; i < objectFilesSorted.size(); i++, j++ )
	{
		ObjectFile &object = *objectFilesSorted[i];
		output.append( "sizeof( iobjects::" );
		output.append( object.type );
		output.append( ( j % 3 == 0 && j != 0 && i != objectFilesSorted.size() - 1 ) ? " ),\n\t" : " ), " );
	}
	output.append( "\n};\n\n\n" );

	// OBJECT_TYPE_NAME
	output.append( "#if COMPILE_DEBUG\n" );
	output.append( "const char *iobjects::OBJECT_TYPE_NAME[OBJECT_TYPE_COUNT] =\n{\n\t" );
	for( usize i = 0, j = 0; i < objectFilesSorted.size(); i++, j++ )
	{
		ObjectFile &object = *objectFilesSorted[i];
		output.append( "\"" ).append( object.name );
		output.append( ( j % 3 == 0 && j != 0 && i != objectFilesSorted.size() - 1 ) ? "\",\n\t" : "\", " );
	}
	output.append( "\n};\n" );
	output.append( "#endif\n\n" );

	// #define OBJECT_CONSTRUCTOR_OFFSET
	output.append( "#define OBJECT_CONSTRUCTOR_OFFSET( typeID ) \\\n" );
	output.append( "\tiobjects::OBJECT_CTOR_BUFFER_OFFSET[typeID] = iobjects::OBJECT_CTOR_BUFFER_SIZE; \\\n" );
	output.append( "\tiobjects::OBJECT_CTOR_BUFFER_SIZE += iobjects::OBJECT_TYPE_SIZE[typeID];\n\n" );

	// #define OBJECT_CONSTRUCTOR_OFFSET
	output.append( "#define OBJECT_CONSTRUCTOR_DATA( typeID, type ) \\\n" );
	output.append( "\t{ new ( iobjects::OBJECT_CTOR_DEFAULT_BUFFER + iobjects::OBJECT_CTOR_BUFFER_OFFSET[typeID] ) type(); }\n\n" );

	// void init()
	output.append( "bool iobjects::init()\n{\n" );
	{
		for( ObjectFile *object : objectFilesSorted )
		{
			output.append( "\tOBJECT_CONSTRUCTOR_OFFSET( " );
			output.append( object->name );
			output.append( " );\n" );
		}
		output.append( "\t// ...\n\n" );

		output.append( "\tiobjects::OBJECT_CTOR_MANUAL_BUFFER = reinterpret_cast<byte *>( memory_alloc( iobjects::OBJECT_CTOR_BUFFER_SIZE ) );\n" );
		output.append( "\tif( iobjects::OBJECT_CTOR_MANUAL_BUFFER == nullptr ) { return false; }\n\n" );

		output.append( "\tiobjects::OBJECT_CTOR_DEFAULT_BUFFER = reinterpret_cast<byte *>( memory_alloc( iobjects::OBJECT_CTOR_BUFFER_SIZE ) );\n" );
		output.append( "\tif( iobjects::OBJECT_CTOR_DEFAULT_BUFFER == nullptr ) { return false; }\n\n" );

		for( ObjectFile *object : objectFilesSorted )
		{
			output.append( "\tOBJECT_CONSTRUCTOR_DATA( " );
			output.append( object->name );
			output.append( ", iobjects::" );
			output.append( object->type );
			output.append( " );\n" );
		}
		output.append( "\t// ...\n\n" );

		output.append( "\treturn true;\n" );
	}
	output.append( "}\n\n" );
}


void objects::generate_source_events( String &output )
{
	output.append( "////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\n\n" );

	for( u8 eventID = 0; eventID < EVENT_COUNT; eventID++ )
	{
		// Skip Certain Events
		if( eventID == KeywordID_EVENT_CREATE ) { continue; }

		output.append( "void ObjectContext::" );
		output.append( g_EVENT_FUNCTIONS[eventID][EventFunction_Name] );
		output.append( g_EVENT_FUNCTIONS[eventID][EventFunction_Parameters] );
		output.append( "\n{\n" );
		for( ObjectFile *object : objectFilesSorted )
		{
			if( !object->events[eventID].has || object->events[eventID].manual ) { continue; }

			// TODO: Optimize this?
			output.append( "\tforeach_object( ( *this ), " ).append( object->name ).append( ", handle ) { " );
			output.append( "handle->" ).append( g_EVENT_FUNCTIONS[eventID][EventFunction_Name] );
			output.append( g_EVENT_FUNCTIONS[eventID][EventFunction_ParametersCaller] ).append( "; }\n" );
		}
		output.append( "}\n\n" );
	}
}


void objects::write()
{
	// Write IntelliSense
	{
		if( verbose_output() ) { PrintColor( LOG_CYAN, "\t\tWrite %s", pathIntelliSense ); }
		Timer timer;
		ErrorIf( !objects::intellisense.save( pathIntelliSense ), "Failed to write '%s'", pathIntelliSense );
		if( verbose_output() ) { PrintLnColor( LOG_WHITE, " (%.3f ms)", timer.elapsed_ms() ); }
	}

	// Write Header
	{
		if( verbose_output() ) { PrintColor( LOG_CYAN, "\t\tWrite %s", pathHeader ); }
		Timer timer;
		ErrorIf( !objects::header.save( pathHeader ), "Failed to write '%s'", pathHeader );
		if( verbose_output() ) { PrintLnColor( LOG_WHITE, " (%.3f ms)", timer.elapsed_ms() ); }
	}

	// Write Source
	{
		if( verbose_output() ) { PrintColor( LOG_CYAN, "\t\tWrite %s", pathSource ); }
		Timer timer;
		ErrorIf( !objects::source.save( pathSource ), "Failed to write '%s'", pathSource );
		if( verbose_output() ) { PrintLnColor( LOG_WHITE, " (%.3f ms)", timer.elapsed_ms() ); }
	}
}