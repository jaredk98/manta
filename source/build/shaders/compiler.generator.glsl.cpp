#include <build/shaders/compiler.generator.hpp>

#include <build/string.hpp>
#include <vendor/stdio.hpp>

namespace ShaderCompiler
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct SemanticSubsituation
{
	SemanticSubsituation( const char *string, const char *substitute ) :
		string{ string }, substitute{ substitute } { }

	String string;
	String substitute;
};


static List<SemanticSubsituation> SemanticSubstitutions;


static void substitute_semantics( String &output )
{
	for( SemanticSubsituation &sub : SemanticSubstitutions )
	{
		output.replace( sub.string.c_str(), sub.substitute.c_str() );
	}

	SemanticSubstitutions.clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char *GLSLPrimitives[] =
{
	"void",             // Primitive_Void
	"bool",             // Primitive_Bool
	"bvec2",            // Primitive_Bool2
	"bvec3",            // Primitive_Bool3
	"bvec4",            // Primitive_Bool4
	"int",              // Primitive_Int
	"ivec2",            // Primitive_Int2
	"ivec3",            // Primitive_Int3
	"ivec4",            // Primitive_Int4
	"uint",             // Primitive_UInt
	"uvec2",            // Primitive_UInt2
	"uvec3",            // Primitive_UInt3
	"uvec4",            // Primitive_UInt4
	"float",            // Primitive_Float
	"vec2",             // Primitive_Float2
	"vec3",             // Primitive_Float3
	"vec4",             // Primitive_Float4
	"mat2",             // Primitive_Float2x2
	"mat3",             // Primitive_Float3x3
	"mat4",             // Primitive_Float4x4
	"double",           // Primitive_Double
	"dvec2",            // Primitive_Double2
	"dvec3",            // Primitive_Double3
	"dvec4",            // Primitive_Double4
	"dmat2",            // Primitive_Double2x2
	"dmat3",            // Primitive_Double3x3
	"dmat4",            // Primitive_Double4x4
	"sampler1D",        // Primitive_Texture1D
	"sampler1DArray",   // Primitive_Texture1DArray
	"sampler2D",        // Primitive_Texture2D
	"sampler2DArray",   // Primitive_Texture2DArray
	"sampler3D",        // Primitive_Texture3D
	"samplerCube",      // Primitive_TextureCube
	"samplerCubeArray", // Primitive_TextureCubeArray
};
static_assert( ARRAY_LENGTH( GLSLPrimitives ) == PRIMITIVE_COUNT, "Missing Primitive!" );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GeneratorGLSL::process_names()
{
	// Types
	{
		for( TypeID typeID = 0; typeID < parser.types.size(); typeID++ )
		{
			String &name = typeNames.add( "" );
			Type &type = parser.types[typeID];

			if( typeID < PRIMITIVE_COUNT )
			{
				// Built-in Types
				name.append( GLSLPrimitives[typeID] );
			}
			else
			{
				// Custom Types
				#if SHADER_OUTPUT_PREFIX_IDENTIFIERS
					name.append( "t_" ).append( type.name );
				#else
					name.append( type.name );
				#endif
			}
		}
	}

	// Functions
	{
		for( FunctionID functionID = 0; functionID < parser.functions.size(); functionID++ )
		{
			String &name = functionNames.add( "" );
			Function &function = parser.functions[functionID];

			if( functionID < INTRINSIC_COUNT )
			{
				// Built-in Functions
				name.append( function.name );
			}
			else
			{
				// Custom Types
				#if SHADER_OUTPUT_PREFIX_IDENTIFIERS
					name.append( "f_" ).append( function.name );
				#else
					name.append( function.name );
				#endif
			}
		}
	}

	// Variables
	{
		for( VariableID variableID = 0; variableID < parser.variables.size(); variableID++ )
		{
			String &name = variableNames.add( "" );
			Variable &variable = parser.variables[variableID];

			switch( variable.typeID )
			{
				case Primitive_Texture1D:
				case Primitive_Texture1DArray:
				case Primitive_Texture2D:
				case Primitive_Texture2DArray:
				case Primitive_Texture3D:
				case Primitive_TextureCube:
				case Primitive_TextureCubeArray:
				{
					name.append( "u_texture" ).append( variable.slot );
				}
				break;

				default:
				{
				#if SHADER_OUTPUT_PREFIX_IDENTIFIERS
					name.append( "v_" ).append( variable.name );
				#else
					name.append( variable.name );
				#endif
				}
				break;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GeneratorGLSL::generate_stage( ShaderStage stage )
{
	// GLSL Version
	output.append( "#version 410 core\n\n" );

	// Super
	Generator::generate_stage( stage );

	// Replace Semantics
	substitute_semantics( output );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GeneratorGLSL::generate_function_declaration( NodeFunctionDeclaration *node )
{
	switch( node->functionType )
	{
		case FunctionType_MainVertex:
		case FunctionType_MainFragment:
		case FunctionType_MainCompute:
			generate_function_declaration_main( node );
		return;
	}

	Function &function = parser.functions[node->functionID];
	const String &returnTypeName = type_name( function.typeID );
	const String &functionName = function_name( node->functionID );

	// Return Type & Name
	output.append( indent ).append( returnTypeName ).append( " " );
	output.append( functionName );

	// Parameter List
	const VariableID first = function.parameterFirst;
	const VariableID last = first + function.parameterCount;

	if( first != last )
	{
		output.append( "(" );
		VariableID parameterCount = 0;
		for( VariableID i = first; i < last; i++ )
		{
			Variable &parameter = parser.variables[i];
			Type &parameterType = parser.types[parameter.typeID];

			// Skip CBuffers (global namespace)
			if( parameterType.tokenType == TokenType_CBuffer ) { continue; }

			output.append( parameterCount == 0 ? " " : ", " );
			const String &parameterTypeName = type_name( parameter.typeID );
			const String &parameterVariableName = variable_name( i );

			// <in> / <out> / <inout>
			if(  parameter.in && !parameter.out ) { output.append( "in " ); } else
			if( !parameter.in &&  parameter.out ) { output.append( "out " ); } else
			if(  parameter.in &&  parameter.out ) { output.append( "inout " ); }

			// <const>
			if( parameter.constant ) { output.append( "const " ); }

			// <type> <variable>
			output.append( parameterTypeName ).append( " " ).append( parameterVariableName );
			parameterCount++;
		}
		output.append( parameterCount > 0 ? " )\n" : ")\n" );
	}
	else
	{
		output.append( "()\n" );
	}

	// Body
	generate_statement_block( reinterpret_cast<NodeStatementBlock *>( node->block ) );
	output.append( "\n" );
}


void GeneratorGLSL::generate_function_declaration_main( NodeFunctionDeclaration *node )
{
	Function &function = parser.functions[node->functionID];
	Type &returnType = parser.types[function.typeID];

	// Return Type & Name
	output.append( indent ).append( "void main" ).append( "()\n" );
	generate_statement_block( reinterpret_cast<NodeStatementBlock *>( node->block ) );
	output.append( "\n" );
}


static NodeExpressionList *get_param( Node *paramFirst, u32 paramIndex )
{
	u32 paramCount = 0;
	Node *param = paramFirst;
	while( param != nullptr )
	{
		Assert( param->nodeType == NodeType_ExpressionListNode );
		NodeExpressionList *paramNode = reinterpret_cast<NodeExpressionList *>( param );
		if( paramCount == paramIndex ) { return paramNode; }
		param = paramNode->next;
		paramCount++;
	}

	Error( "unable to get parameter %u", paramIndex );
	return nullptr;
}


void GeneratorGLSL::generate_function_call_intrinsics( NodeFunctionCall *node )
{
	Intrinsic intrinsic = node->functionID;
	Assert( intrinsic < INTRINSIC_COUNT );

	switch( intrinsic )
	{
		case Intrinsic_Mul:
		{
			output.append( "( ( " );
			generate_node( get_param( node->param, 0 )->expr );
			output.append( " ) * ( " );
			generate_node( get_param( node->param, 1 )->expr );
			output.append( " ) )" );
		}
		return;

		case Intrinsic_SampleTexture1D:
		case Intrinsic_SampleTexture1DArray:
		case Intrinsic_SampleTexture2D:
		case Intrinsic_SampleTexture2DArray:
		case Intrinsic_SampleTexture3D:
		case Intrinsic_SampleTextureCube:
		case Intrinsic_SampleTextureCubeArray:
		{
			// <texture>.Sample( sampler, <uv> );
			output.append( "texture( " );
			generate_node( get_param( node->param, 0 )->expr );
			output.append( ", " );
			generate_node( get_param( node->param, 1 )->expr );
			output.append( " )" );
		}
		return;

		case Intrinsic_SampleTexture2DLevel:
		{
			// <texture>.SampleLevel( sampler, <uv> );
			output.append( "texture( " );
			generate_node( get_param( node->param, 0 )->expr );
			output.append( ", " );
			generate_node( get_param( node->param, 1 )->expr );
			output.append( " )" );
		}
		return;

		default:
			generate_function_call( node );
		return;
	}
}


void GeneratorGLSL::generate_expression_binary( NodeExpressionBinary *node )
{
	static const char *binaryOperators[] =
	{
		".",     // ExpressionBinaryType_Dot
		"",      // ExpressionBinaryType_Subscript
		" = ",   // ExpressionBinaryType_Assign
		" + ",   // ExpressionBinaryType_Add
		" += ",  // ExpressionBinaryType_AddAssign
		" - ",   // ExpressionBinaryType_Sub
		" -= ",  // ExpressionBinaryType_SubAssign
		" * ",   // ExpressionBinaryType_Mul
		" *= ",  // ExpressionBinaryType_MulAssign
		" / ",   // ExpressionBinaryType_Div
		" /= ",  // ExpressionBinaryType_DivAssign
		" % ",   // ExpressionBinaryType_Mod
		" %= ",  // ExpressionBinaryType_ModAssign
		" & ",   // ExpressionBinaryType_BitAnd
		" &= ",  // ExpressionBinaryType_BitAndAssign
		" | ",   // ExpressionBinaryType_BitOr
		" |= ",  // ExpressionBinaryType_BitOrAssign
		" ^ ",   // ExpressionBinaryType_BitXor
		" ^= ",  // ExpressionBinaryType_BitXorAssign
		" << ",  // ExpressionBinaryType_BitShiftLeft
		" <<= ", // ExpressionBinaryType_BitShiftLeftAssign
		" >> ",  // ExpressionBinaryType_BitShiftRight
		" >>= ", // ExpressionBinaryType_BitShiftRightAssign
		" == ",  // ExpressionBinaryType_Equals
		" != ",  // ExpressionBinaryType_NotEquals
		" && ",  // ExpressionBinaryType_And
		" || ",  // ExpressionBinaryType_Or
		" > ",   // ExpressionBinaryType_Greater
		" >= ",  // ExpressionBinaryType_GreaterEquals
		" < ",   // ExpressionBinaryType_Less
		" <= ",  // ExpressionBinaryType_LessEquals
	};
	static_assert( ARRAY_LENGTH( binaryOperators ) == EXPRESSIONBINARYTYPE_COUNT, "Missing ExpressionBinaryType!" );

	switch( node->exprType )
	{
		case ExpressionBinaryType_Subscript:
		{
			generate_node( node->expr1 );
			output.append( "[" );
			generate_node( node->expr2 );
			output.append( "]" );
		}
		break;

		case ExpressionBinaryType_Dot:
		{
			// CBuffer members are in the global namespace, so we prefix them by structure name
			if( node->expr1->nodeType == NodeType_Variable )
			{
				NodeVariable *nodeVariable = reinterpret_cast<NodeVariable *>( node->expr1 );
				Variable &variable = parser.variables[nodeVariable->variableID];
				Type &type = parser.types[variable.typeID];

				if( type.global )
				{
					String &typeName = type_name( variable.typeID );
					output.append( typeName ).append( "_" );
					generate_node( node->expr2 );
					break;
				}
			}

			// Only reachable if LHS not cbuffer type
			generate_node( node->expr1 );
			output.append( "." );
			generate_node( node->expr2 );
		}
		break;

		default:
		{
			generate_node( node->expr1 );
			output.append( binaryOperators[node->exprType] );
			generate_node( node->expr2 );
		}
		break;
	}
}


void GeneratorGLSL::generate_structure( NodeStruct *node )
{
	Struct &structure = parser.structs[node->structID];
	Type &type = parser.types[structure.typeID];
	const String &typeName = type_name( structure.typeID );
	const VariableID first = type.memberFirst;
	const VariableID last = first + type.memberCount;

	static const char *structNames[] =
	{
		"struct",                   // StructType_Struct
		"layout( std140 ) uniform", // StructType_CBuffer
		"",                         // StructType_VertexInput
		"",                         // StructType_VertexOutput
		"",                         // StructType_FragmentInput
		"",                         // StructType_FragmentOutput
		"",                         // StructType_ComputeInput
		"",                         // StructType_ComputeOutput
	};
	static_assert( ARRAY_LENGTH( structNames ) == STRUCTTYPE_COUNT, "Missing TextureType" );
	const bool expectSlot =  ( node->structType == StructType_CBuffer );
	const bool expectMeta = !( node->structType == StructType_CBuffer || node->structType == StructType_Struct );

	bool hasBody, hasIn, hasOut, hasLayout;
	switch( node->structType )
	{
		default:
		case StructType_Struct:         { hasBody = true;  hasIn = false; hasOut = false; hasLayout = false; } break;
		case StructType_CBuffer:        { hasBody = true;  hasIn = false; hasOut = false; hasLayout = false; } break;
		case StructType_VertexInput:    { hasBody = false; hasIn = true;  hasOut = false; hasLayout = true;  } break;
		case StructType_VertexOutput:   { hasBody = false; hasIn = false; hasOut = true;  hasLayout = true;  } break;
		case StructType_FragmentInput:  { hasBody = false; hasIn = true;  hasOut = false; hasLayout = true;  } break;
		case StructType_FragmentOutput: { hasBody = false; hasIn = false; hasOut = true;  hasLayout = true;  } break;
		case StructType_ComputeInput:   { hasBody = false; hasIn = true;  hasOut = false; hasLayout = false; } break;
		case StructType_ComputeOutput:  { hasBody = false; hasIn = false; hasOut = true;  hasLayout = false; } break;
	}

	// { <body> }
	if( hasBody )
	{
		// <name> <type>
		output.append( structNames[node->structType] ).append( " " ).append( typeName );
		output.append( "\n{\n" );
		indent_add();
	}

	for( usize i = first, location = 0; i < last; i++, location++ )
	{
		Variable &memberVariable = parser.variables[i];
		Type &memberVariableType = parser.types[memberVariable.typeID];
		const String &memberVariableName = variable_name( i );
		const String &memberTypeName = type_name( memberVariable.typeID );
		output.append( indent );

		// GLSL Restrictions
		switch( node->structType )
		{
			// vertex_output
			case StructType_VertexOutput:
			{
				switch( memberVariable.semantic )
				{
					case SemanticType_POSITION:
					{
						output.append( "// " );
						String replace = String( typeName ).append( "_" ).append( memberVariableName );
						SemanticSubstitutions.add( SemanticSubsituation( replace.c_str(), "gl_Position" ) );
					}
					break;
				}
			}
			break;

			// fragment_input
			case StructType_FragmentInput:
			{
				switch( memberVariable.semantic )
				{
					case SemanticType_POSITION:
					{
						output.append( "// " );
						String replace = String( typeName ).append( "_" ).append( memberVariableName );
						SemanticSubstitutions.add( SemanticSubsituation( replace.c_str(), "gl_FragCoord" ) );
					}
					break;
				}
			}
			break;

			// fragment_output
			case StructType_FragmentOutput:
			{
				switch( memberVariable.semantic )
				{
					case SemanticType_DEPTH:
					{
						output.append( "// " );
						String replace = String( typeName ).append( "_" ).append( memberVariableName );
						SemanticSubstitutions.add( SemanticSubsituation( replace.c_str(), "gl_Depth" ) );
					}
					break;
				}
			}
			break;
		}

		// <layout>
		if( hasLayout )
		{
			int slot = memberVariable.slot != -1 ? memberVariable.slot : static_cast<int>( location );
			output.append( "layout( location = " ).append( slot ).append( " ) " );
		}

		// <in> / <out>
		if( hasIn ) { output.append( "in " ); }
		if( hasOut ) { output.append( "out " ); }

		// <type>
		output.append( memberTypeName ).append( " " );

		// <name>
		if( node->structType == StructType_CBuffer || !hasBody )
		{
			// cbuffer, in, and out members are in global namespace, so we prefix them with the structure name
			output.append( typeName ).append( "_" );
		}
		output.append( memberVariableName );

		// [arrayX]
		if( memberVariable.arrayLengthX > 0 )
		{
			output.append( "[" ).append( memberVariable.arrayLengthX ).append( "]");
		}

		// [arrayY]
		if( memberVariable.arrayLengthY > 0 )
		{
			output.append( "[" ).append( memberVariable.arrayLengthY ).append( "]");
		}

		output.append( ";\n" );
	}

	if( hasBody )
	{
		indent_sub();
		output.append( "};\n" );
	}

	output.append( "\n" );
}


bool GeneratorGLSL::generate_structure_gfx_vertex( NodeStruct *node )
{
	// Register this vertex format in the system
	if( !Generator::generate_structure_gfx_vertex( node ) )
	{
		return false; // Already registered -- no need to continue
	}

	// Generate OpenGL/GLSL Vertex Format interfaces
	const Struct &structure = parser.structs[node->structID];
	const Type &type = parser.types[structure.typeID];
	const VariableID first = type.memberFirst;
	const VariableID last = first + type.memberCount;
	const String &structureName = type_name( structure.typeID );

	int byteOffset = 0;
	int semanticIndex[SEMANTICTYPE_COUNT];
	for( int i = 0; i < SEMANTICTYPE_COUNT; i++ ) { semanticIndex[i] = 0; }

	String link;
	String bind;

	for( usize i = first; i < last; i++ )
	{
		Variable &memberVariable = parser.variables[i];
		const String &memberVariableName = variable_name( i );

		// opengl_vertex_input_layout_init
		{
			link.append( "\tnglBindAttribLocation( program, " ).append( static_cast<int>( i - first ) ).append( ", \"" );
			link.append( structureName ).append( "_" ).append( memberVariableName ).append( "\" );\n" );
		}

		// opengl_verteX_input_layout_bind
		{
			// Get glVertexAttribPointer function
			const char *glVertexAttribFunc = "";
			bool hasNormFlag = false;
			switch( memberVariable.typeID )
			{
				case Primitive_Int:
				case Primitive_UInt:
				case Primitive_Int2:
				case Primitive_UInt2:
				case Primitive_Int3:
				case Primitive_UInt3:
				case Primitive_Int4:
				case Primitive_UInt4:
					glVertexAttribFunc = "nglVertexAttribIPointer";
				break;

				case Primitive_Double:
				case Primitive_Double2:
				case Primitive_Double3:
				case Primitive_Double4:
					glVertexAttribFunc = "nglVertexAttribLPointer";
				break;

				default:
					glVertexAttribFunc = "nglVertexAttribPointer";
					hasNormFlag = true;
				break;
			}

			// Get type dimensions (vector length)
			int dimensions = 0;
			switch( memberVariable.typeID )
			{
				case Primitive_Bool:
				case Primitive_Int:
				case Primitive_UInt:
				case Primitive_Float:
				case Primitive_Double:
					dimensions = 1;
				break;

				case Primitive_Bool2:
				case Primitive_Int2:
				case Primitive_UInt2:
				case Primitive_Float2:
				case Primitive_Double2:
					dimensions = 2;
				break;

				case Primitive_Bool3:
				case Primitive_Int3:
				case Primitive_UInt3:
				case Primitive_Float3:
				case Primitive_Double3:
					dimensions = 3;
				break;

				case Primitive_Bool4:
				case Primitive_Int4:
				case Primitive_UInt4:
				case Primitive_Float4:
				case Primitive_Double4:
					dimensions = 4;
				break;

				default:
					Error( "Unexpected vertex input type: %llu (%s)", memberVariable.typeID, memberVariableName.c_str() );
				break;
			}

			// Get type format
			struct FormatInfo
			{
				FormatInfo() = default;
				FormatInfo( const char *type, bool normalized, int size, int stride = 0 ) :
					type{ type }, normalized{ normalized }, size{ size }, stride{ stride } { }

				const char *type = "";
				bool normalized = false;
				int size = 1;
				int stride = 0;
			};

			FormatInfo format;
			switch( memberVariable.format )
			{
				// 1 Byte
				case InputFormat_UNORM8:
					//format = FormatInfo( "GL_UNSIGNED_BYTE", true, 1, 0 );
					format = FormatInfo( "GL_UNSIGNED_BYTE", true, 1, 0 );
				break;

				case InputFormat_SNORM8:
					format = FormatInfo( "GL_BYTE", true, 1, 0 );
				break;

				case InputFormat_UINT8:
					format = FormatInfo( "GL_UNSIGNED_BYTE", false, 1, 0 );
				break;

				case InputFormat_SINT8:
					format = FormatInfo( "GL_BYTE", false, 1, 0 );
				break;

				// 2 Bytes
				case InputFormat_UNORM16:
					format = FormatInfo( "GL_UNSIGNED_SHORT", true, 2, 0 );
				break;

				case InputFormat_SNORM16:
					format = FormatInfo( "GL_SHORT", true, 2, 0 );
				break;

				case InputFormat_UINT16:
					format = FormatInfo( "GL_UNSIGNED_SHORT", false, 2, 0 );
				break;

				case InputFormat_SINT16:
					format = FormatInfo( "GL_SHORT", false, 2, 0 );
				break;

				case InputFormat_FLOAT16:
					format = FormatInfo( "GL_HALF_FLOAT", false, 2, 0 );
				break;

				// 4 Bytes
				case InputFormat_UNORM32:
					format = FormatInfo( "GL_UNSIGNED_INT", true, 4, 0 );
				break;

				case InputFormat_SNORM32:
					format = FormatInfo( "GL_INT", true, 4, 0 );
				break;

				case InputFormat_UINT32:
					format = FormatInfo( "GL_UNSIGNED_INT", false, 4, 0 );
				break;

				case InputFormat_SINT32:
					format = FormatInfo( "GL_INT", false, 4, 0 );
				break;

				case InputFormat_FLOAT32:
					format = FormatInfo( "GL_FLOAT", false, 4, 0 );
				break;
			}

			bind.append( "\t" ).append( glVertexAttribFunc ).append( "( " );
			bind.append( static_cast<int>( i - first ) ).append( ", " );
			bind.append( dimensions ).append( ", " ).append( format.type ).append( ", " );
			if( hasNormFlag ) { bind.append( format.normalized ? "true" : "false" ).append( ", " ); }
			bind.append( "sizeof( GfxVertex::" ).append( type.name ).append( " ), " );
			bind.append( "reinterpret_cast<void *>( " ).append( byteOffset ).append( " ) );\n" );
			byteOffset += format.size * dimensions;

			bind.append( "\tnglEnableVertexAttribArray( " ).append( static_cast<int>( i - first ) ).append( " );\n" );
		}
	}

	// Write To gfx.api.generated.cpp
	shader.source.append( "static void opengl_vertex_input_layout_init_" ).append( type.name ).append( "( GLuint program )\n" );
	shader.source.append( "{\n" );
	shader.source.append( link );
	shader.source.append( "}\n\n" );

	shader.source.append( "static void opengl_vertex_input_layout_bind_" ).append( type.name ).append( "()\n" );
	shader.source.append( "{\n" );
	shader.source.append( bind );
	shader.source.append( "}\n\n" );

	return true;
}


void GeneratorGLSL::generate_texture( NodeTexture *node )
{
	// TODO: Multiple sampler states for each texture (like OpenGL)
	static bool generatedSampler = false;

	Texture &texture = parser.textures[node->textureID];
	const String &typeName = type_name( parser.variables[texture.variableID].typeID );
	const String &variableName = variable_name( texture.variableID );

	// Texture
	output.append( indent ).append( "uniform " );
	output.append( typeName ).append( " " ).append( variableName ).append( ";\n\n" );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}