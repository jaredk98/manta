#include <manta/thread.hpp>

#include <debug.hpp>

#include <vendor/windows.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void thread::sleep( u32 milliseconds )
{
	Sleep( milliseconds );
}


ThreadID thread::id()
{
	return ThreadID( GetCurrentThreadId() );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Thread::Thread( ThreadFunction function )
{
	// Create the thread
	handle = CreateThread( nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>( function ), nullptr, 0, nullptr );
	ErrorIf( handle == nullptr, "WIN: Failed to create thread!" );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void Mutex::init()
{
	InitializeCriticalSection( &mutex );
}


void Mutex::free()
{
	DeleteCriticalSection( &mutex );
}


void Mutex::lock()
{
	EnterCriticalSection( &mutex );
}


void Mutex::unlock()
{
	LeaveCriticalSection( &mutex );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Condition::init()
{
	InitializeConditionVariable( &condition );
}


void Condition::free()
{
	// Do nothing
}


void Condition::sleep( Mutex &mutex )
{
	SleepConditionVariableCS( &condition, &mutex.mutex, 0xFFFFFFFF );
}


void Condition::wake()
{
	WakeConditionVariable( &condition );
}