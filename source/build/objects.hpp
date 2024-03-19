// ObjectFile system based on manta/build/api.hpp
#pragma once

#include <build/string.hpp>
#include <build/list.hpp>
#include <build/fileio.hpp>

#include <types.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum_type( KeywordID, u8 )
{
// EVENTS
	KeywordID_EVENT_CREATE = 0,
	KeywordID_EVENT_DESTROY,
	KeywordID_EVENT_INITIALIZE,

	KeywordID_EVENT_FRAME_START,
	KeywordID_EVENT_FRAME_END,

	KeywordID_EVENT_STEP_CUSTOM,
	KeywordID_EVENT_STEP_GUI,
	KeywordID_EVENT_STEP,

	KeywordID_EVENT_DRAW_CUSTOM,
	KeywordID_EVENT_DRAW_GUI,
	KeywordID_EVENT_DRAW,

	KeywordID_EVENT_SLEEP,
	KeywordID_EVENT_WAKE,

	KeywordID_EVENT_FLAG,
	KeywordID_EVENT_PARTITION,
	KeywordID_EVENT_UI_MASK,

	KeywordID_EVENT_SAVE,
	KeywordID_EVENT_LOAD,

	KeywordID_EVENT_NETWORK_SEND,
	KeywordID_EVENT_NETWORK_RECEIVE,

	EVENT_COUNT,

// KEYWORDS
	KeywordID_HEADER_INCLUDES = EVENT_COUNT,
	KeywordID_SOURCE_INCLUDES,
	KeywordID_OBJECT,
	KeywordID_PARENT,
	KeywordID_COUNT,
	KeywordID_BUCKET_SIZE,
	KeywordID_ABSTRACT,
	KeywordID_NOINHERIT,
	KeywordID_NETWORKED,
	KeywordID_VERSION,
	KeywordID_CONSTRUCTOR,
	KeywordID_PRIVATE,
	KeywordID_PUBLIC,
	KeywordID_GLOBAL,

	KEYWORD_COUNT,
};

struct Keyword
{
	Keyword( KeywordID id = 0, usize start = 0, usize end = 0 ) : id( id ), start( start ), end( end ) { }
	KeywordID id = 0;
	usize start = 0;
	usize end = 0;
};

struct KeywordRequirements
{
	KeywordRequirements( bool required, int count ) : required( required ), count( count ) { }
	bool required;
	int count;
};

struct Event
{
	bool has = false;
	bool disabled = false;
	bool manual = false;
};

enum_type( EventFunction, u8 )
{
	EventFunction_Name,
	EventFunction_ReturnType,
	EventFunction_ReturnValue,
	EventFunction_Parameters,
	EventFunction_ParametersCaller,

	EVENT_FUNCTION_DEFINITION_COUNT,
};

extern const char *g_KEYWORDS[KEYWORD_COUNT];

extern int g_KEYWORD_COUNT[KEYWORD_COUNT];

extern KeywordRequirements g_KEYWORD_REQUIREMENTS[KEYWORD_COUNT];

extern const char *g_EVENT_FUNCTIONS[][EVENT_FUNCTION_DEFINITION_COUNT];

#define KEYWORD_IS_EVENT( keywordID ) ( keywordID < EVENT_COUNT )

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ObjectFile
{
	ObjectFile( const char *path ) : path( path ) { }
	String path;

	void parse();
	void write_header();
	void write_intellisense();

	void write_handle();
	void write_source();

	void parse_keywords( const String &buffer );
	void parse_keywords_values( const String &buffer );
	void parse_keywords_code( const String &buffer );

	String keyword_PARENTHESES_string( const String &buffer, Keyword &keyword, const bool requireParentheses = true );
	int keyword_PARENTHESES_int( const String &buffer, Keyword &keyword, const bool requireParentheses = true );
	double keyword_PARENTHESES_double( const String &buffer, Keyword &keyword, const bool requireParentheses = true );
	float keyword_PARENTHESES_float( const String &buffer, Keyword &keyword, const bool requireParentheses = true );
	bool keyword_PARENTHESES_bool( const String &buffer, Keyword &keyword, const bool requireParentheses = true );

	void keyword_INCLUDES( const String &buffer, Keyword &keyword );
	void keyword_CONSTRUCTOR( const String &buffer, Keyword &keyword );
	void keyword_EVENT( const String &buffer, Keyword &keyword );
	void keyword_EVENT_NULL( const String &buffer, Keyword &keyword );
	void keyword_PRIVATE_PUBLIC_GLOBAL( const String &buffer, Keyword &keyword );

	// ObjectFile Info
	String name;
	String type;
	String nameParent = "OBJECT_BASE";
	String typeParent = "OBJECT_BASE_t";
	String typeParentFull = "iObjects::OBJECT_BASE_t";
	String count = "-1";
	String bucketSize = "1024";
	bool abstract = false;
	bool noinherit = false;
	bool networked = false;
	int version = -1;

	// Topological Sort
	bool visited = false;
	bool hasParent = false;
	u16 depth = 0;
	struct ObjectFile *parent = nullptr;
	List<struct ObjectFile *> children;

	// Code Generation
	List<Keyword> keywords;

	Event events[EVENT_COUNT];
	List<String> eventsHeader;
	List<String> eventsSource;
	List<String> eventsDisabled;

	List<String> constructorHeader;
	List<String> constructorSource;

	List<String> privateVariableHeader;
	List<String> publicVariableHeader;

	List<String> privateFunctionHeader;
	List<String> privateFunctionSource;

	List<String> publicFunctionHeader;
	List<String> publicFunctionSource;

	List<String> globalVariableHeader;
	List<String> globalVariableSource;

	List<String> globalFunctionHeader;
	List<String> globalFunctionSource;

	List<String> inheritedVariables;
	List<String> inheritedFunctions;
	List<String> inheritedEvents;

	//void ErrorIf( bool condition, const char *message, ... );
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Objects
{
	// Output Paths
	extern char pathSource[PATH_SIZE];
	extern char pathHeader[PATH_SIZE];
	extern char pathIntelliSense[PATH_SIZE];
	extern FileTime timeCache;

	// Output Contents
	extern String intellisense;
	extern String headerIncludes;
	extern String sourceIncludes;
	extern String header;
	extern String source;

	// Object Files
	extern List<ObjectFile> objectFiles;
	extern List<ObjectFile *> objectFilesSorted; // TODO: don't store pointers?
	extern usize objectFilesCount;

	extern void begin();
	extern u32 gather( const char *directory, const bool recurse );
	extern void parse();
	extern void resolve();
	extern void generate();
	extern void write();

	extern void sort_objects( ObjectFile *object, u16 depth, List<ObjectFile *> &outList );

	extern void generate_intellisense( String &output );
	extern void generate_header( String &output );
	extern void generate_header_metadata( String &output );
	extern void generate_source( String &output );
	extern void generate_source_metadata( String &output );
	extern void generate_source_events( String &output );

	//void ErrorIf( const bool condition, const char *message, ... );
}