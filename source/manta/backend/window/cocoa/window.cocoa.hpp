#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __OBJC__
	#include <vendor/stddef.hpp>
	#import <Cocoa/Cocoa.h>

	namespace iWindow
	{
		extern NSView *view;
	}
#else
	static_assert( false, "Not Objective-C!" );
#endif