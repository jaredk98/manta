#include <manta/time.hpp>

#include <config.hpp>
#include <manta/thread.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace iTime
{
	// Implementations: manta/backend/time/...
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Time
{
	// See manta/backend/time/...
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Frame
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
		if( Frame::fpsLimit == 0 ) { return; }

		const double remaining = ( 1.0 / Frame::fpsLimit ) - ( Frame::timeEnd - Frame::timeStart );
		const int sleep = static_cast<int>( remaining * 1000.0 ) - FPS_MARGIN; // miliseconds

		// Sleep & Busy-wait
		if( sleep > 0 ) { Thread::sleep( sleep ); }
		while( Time::value() < Frame::timeEnd + remaining );
	}

	void start()
	{
		// Time
		const double timePrevious = timeStart;
		timeStart = Time::value();
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
		timeEnd = Time::value();
		regulate();
	}
}
