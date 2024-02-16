#pragma once


#include <vendor/stdio.hpp>


template <typename T> inline T &min( T &a, T &b )
{
	return a < b ? a : b;
}


//#include <chrono>

class Timer
{
public:
	//Timer() : start_time( std::chrono::high_resolution_clock::now() ) { }

	float elapsed_s()
	{
		//auto end_time = std::chrono::high_resolution_clock::now();
		//auto duration = std::chrono::duration_cast<std::chrono::microseconds>( end_time - start_time );
		//return static_cast<float>( duration.count() / 1000.0f / 1000.0f );
		return 0.0f;
	}

	float elapsed_ms()
	{
		//auto end_time = std::chrono::high_resolution_clock::now();
		//auto duration = std::chrono::duration_cast<std::chrono::microseconds>( end_time - start_time );
		//return static_cast<float>( duration.count() / 1000.0f );
		return 0.0f;
	}

	float elapsed_us()
	{
		//auto end_time = std::chrono::high_resolution_clock::now();
		//auto duration = std::chrono::duration_cast<std::chrono::microseconds>( end_time - start_time );
		//return static_cast<float>( duration.count() );
		return 0.0f;
	}
private:
   // std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
};