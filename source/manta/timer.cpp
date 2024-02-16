#include <manta/timer.hpp>

#include <config.hpp>
#include <manta/thread.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace itimer
{
	// Implementations: manta/backend/time/...
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace timer
{
	// See manta/backend/time/...
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace frame
{
	u32 fps = 0;
	u32 fpsLimit = FPS_LIMIT;
	Delta delta = 0.0;
	bool tickFrame = false;
	bool tickSecond = false;

	static u32 fpsCounter = 0;
	static double timeStart = 0.0;
	static double timeEnd = 0.0;
	static double tickSecondTimer = 0.0;
	static double tickFrameTimer = 0.0;

	static void regulate()
	{
		// Uncapped FPS?
		if( frame::fpsLimit == 0 ) { return; }

		const double remaining = ( 1.0 / frame::fpsLimit ) - ( frame::timeEnd - frame::timeStart );
		const int sleep = static_cast<int>( remaining * 1000.0 ) - FPS_MARGIN; // miliseconds

		// Sleep & Busy-wait
		if( sleep > 0 ) { thread::sleep( sleep ); }
		while( timer::value() < frame::timeEnd + remaining );
	}

	void start()
	{
		// Time
		const double timePrevious = timeStart;
		timeStart = timer::value();
		delta = ( timeStart - timePrevious );

		// Reset ticks
		tickSecond = false;
		tickFrame = false;

		// Update FPS
		tickSecondTimer += delta;
		tickFrameTimer += delta;
		fpsCounter++;

		// tickSecond & fps
		if( tickSecondTimer >= 1.0 )
		{
			tickSecond = true;
			tickSecondTimer = 0.0;

			fps = fpsCounter;
			fpsCounter = 0;
		}

		// tickFrame
		if( tickFrameTimer >= ( 1.0 / DELTA_TIME_FRAMERATE ) )
		{
			tickFrame = true;
			tickFrameTimer = 0.0;
		}
	}

	void end()
	{
		timeEnd = timer::value();
		regulate();
	}
}
