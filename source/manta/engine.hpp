#pragma once

#include <config.hpp>
#include <types.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ProjectCallbacks
{
public:
	ProjectCallbacks( bool ( *init )( int, char ** ),
						bool ( *free )(),
						void ( *update )( const Delta ) ) : callback_init( init ),
													        callback_free( free ),
													        callback_update( update ) { }

	inline bool init( int argc, char **argv ) const
	{
		return callback_init != nullptr ? callback_init( argc, argv ) : true;
	}

	inline bool free() const
	{
		return callback_free != nullptr ? callback_free() : true;
	}

	inline void update( const Delta delta ) const
	{
		if( callback_update == nullptr ) { return; }
		callback_update( delta );
	}

private:
	bool ( *callback_init )( int, char ** ); // bool init( int argc, char **argv );
	bool ( *callback_free )(); // bool free();
	void ( *callback_update )( const Delta ); // void update( Delta delta );
};


namespace engine
{
	extern int main( int argc, char **argv, const ProjectCallbacks &project );
	extern void exit();
	extern void reboot();
}