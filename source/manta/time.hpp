#pragma once

#include <types.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace iTime
{
	extern bool init();
	extern bool free();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Time
{
	extern double value();
	extern u64 seed();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Timer
{
	Timer() { start(); }

	double timeStart = 0.0;
	double timeEnd = 0.0;

	void start() { timeStart = Time::value(); }
	void stop() { timeEnd = Time::value(); }

	double elapsed_s()
	{
		stop();
		return ( timeEnd - timeStart );
	}

	double elapsed_ms()
	{
		stop();
		return ( timeEnd - timeStart ) * 1000.0;
	}

	double elapsed_us()
	{
		stop();
		return ( timeEnd - timeStart ) * 1000.0 * 1000.0;
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Frame
{
	extern u32 fps;
	extern u32 fpsLimit;
	extern Delta delta;
	extern bool tickSecond;
	extern bool tickFrame;

	extern void start();
	extern void end();
}