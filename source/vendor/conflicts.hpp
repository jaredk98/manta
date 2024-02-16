// For compile times, there are optional defines 'OFFICIAL_HEADERS' and
// 'USE_OFFICIAL_HEADERS' that wrap around C headers with our own implentations
//
// Doing so significantly reduces the size and complexity of headers,
// which in turn reduces preprocessing and compile times
//
// Since we're already wrapping C headers in this way, it also allows us to
// circumvent global namespace conflicts such as stdlib's monopoly of 'random'

#ifndef CONFLICT_GUARD
#define CONFLICT_GUARD

	// stdlib.h
	#define random CONFLICT_random

	// xlib.h
	#define Font CONFLICT_Font

	// ?
	#define min CONFLICT_min
	#define max CONFLICT_max

#else

	// stdlib.h
	#undef random

	// xlib.h
	#undef Font

	// ?
	#undef min
	#undef max

#undef CONFLICT_GUARD
#endif