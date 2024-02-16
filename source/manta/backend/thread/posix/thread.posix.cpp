#include <manta/thread.hpp>

#include <vendor/pthread.hpp>
#include <vendor/posix.hpp>

#include <debug.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void thread::sleep( u32 milliseconds )
{
	usleep( milliseconds * 1000 );
}


ThreadID thread::id()
{
	return ThreadID( pthread_self() );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Thread::Thread( ThreadFunction function )
{
	// Create the thread
	int result = pthread_create( reinterpret_cast<pthread_t *>( &handle ), nullptr, function, nullptr );
	ErrorIf( result != 0, "POSIX: Failed to create thread!" );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void Mutex::init()
{
	pthread_mutex_init( &mutex, 0 );
}


void Mutex::free()
{
	pthread_mutex_destroy( &mutex );
}


void Mutex::lock()
{
	pthread_mutex_lock( &mutex );
}


void Mutex::unlock()
{
	pthread_mutex_unlock( &mutex );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Condition::init()
{
	pthread_cond_init( &condition, 0 );
}


void Condition::free()
{
	// Do nothing
}


void Condition::sleep( Mutex &mutex )
{
	pthread_cond_wait( &condition, &mutex.mutex );
}


void Condition::wake()
{
	pthread_cond_signal( &condition );
}