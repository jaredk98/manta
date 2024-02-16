#pragma once

#include <manta/list.hpp>
#include <manta/string.hpp>

#define TOKEN_SIZE 256


static inline bool is_delimiter( const char c )
{
	return ( c == ' ' || c == '\n' || c == '\0' );
}


class Tokenizer
{
public:
	ScopedList<FixedString<TOKEN_SIZE>> tokens;

	void tokenize( const char *buffer, const u64 size = UINT32_MAX )
	{
		FixedString<TOKEN_SIZE> token; // New token
		const char *t = nullptr;       // Start of current token
		const char *c = buffer;        // Current position
		tokens.clear();

		while( *c != '\0' && c - buffer < size )
		{
			if( is_delimiter( *c ) || c - t == TOKEN_SIZE )
			{
				if( t != nullptr )
				{
					token[c - t] = '\0'; // Null terminate token
					t = nullptr;
					tokens.add( token );
				}
			}
			else
			{
				if( t == nullptr ) { t = c; }
				token[c - t] = *c;
			}

			c++;
		}

		if( t != nullptr )
		{
			token[c - t] = '\0'; // Null terminate token
			tokens.add( token );
		}
	}
};
