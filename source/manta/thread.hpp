#pragma once

#include <config.hpp>
#include <types.hpp>

#include <vendor/vendor.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if THREAD_WINDOWS
	// Windows
	#define THREAD_FUNCTION( name ) unsigned int STD_CALL name( void * )
	using ThreadFunction = unsigned int (STD_CALL *)( void * );
	#include <vendor/windows.hpp>
#elif THREAD_POSIX
	// POSIX
	#define THREAD_FUNCTION( name ) void * name( void * )
	using ThreadFunction = void *(*)( void * );
	#include <vendor/pthread.hpp>
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ThreadID
{
#if THREAD_WINDOWS
	DWORD id;
	ThreadID( const DWORD id ) { this->id = id; }
	bool operator==( const ThreadID &other ) const { return ( id == other.id ); }
#elif THREAD_POSIX
	pthread_t id;
	ThreadID( const pthread_t id ) { this->id = id; }
	bool operator==( const ThreadID &other ) const { return ( id == other.id ); }
#else
	ThreadID( const pthread_t id ) { this->id = id; }
	bool operator==( const ThreadID &other ) const { return true; }
#endif
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace thread
{
	extern void sleep( u32 milliseconds );
	extern struct ThreadID id();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Thread
{
	Thread( ThreadFunction function );
	void *handle = nullptr;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Mutex
{
#if THREAD_WINDOWS
	CRITICAL_SECTION mutex;
#elif THREAD_POSIX
	pthread_mutex_t mutex;
#endif

	void init();
	void free();
	void lock();
	void unlock();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Condition
{
#if THREAD_WINDOWS
	CONDITION_VARIABLE condition;
#elif THREAD_POSIX
	pthread_cond_t condition;
#endif

	void init();
	void free();
	void sleep( Mutex &mutex );
	void wake();
};