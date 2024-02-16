#pragma once
#include <vendor/config.hpp>

#if USE_OFFICIAL_HEADERS
	#include <vendor/conflicts.hpp>
		#include <math.h>
	#include <vendor/conflicts.hpp>
#else
	#if PIPELINE_COMPILER_MSVC
		// MSVC
		extern "C"
		{
			extern double cos(double);
			extern double acos(double);
			extern double sin(double);
			extern float  cosf(float);
			extern float  sinf(float);
			extern float  acosf(float);
			extern float  asinf(float);
			extern float  atan2f(float, float);
			extern float  atanf(float);
			extern float  ceilf(float);
			extern float  coshf(float);
			extern float  expf(float);
			extern double fabs(double);
			extern double ceil(double);
			extern double floor(double);
			extern double fmod(double, double);
			extern float  floorf(float);
			extern double round(double);
			extern double sqrt(double);
			extern float  sqrtf(float);
			extern float  powf(float, float);
			extern double pow(double, double);
			extern int    abs(int);
			extern double frexp(double, int *);

			// double cos(double);
			// double sin(double);

			// #pragma function(cosf)
			// #pragma function(sinf)

			// static inline float cosf(float x) { return static_cast<float>(cos(x)); }
			// static inline float sinf(float x) { return static_cast<float>(sin(x)); }

			// // #pragma intrinsic(cosf)
			// // #pragma intrinsic(sinf)
		}

		static inline double abs(double x) { return fabs(x); }
		static inline float  abs(float  x) { return static_cast<float>(fabs(x)); }
	#else
		static inline double cos    (double  x)           { return __builtin_cos(x); }
		static inline double acos   (double  x)           { return __builtin_acos(x); }
		static inline double sin    (double  x)           { return __builtin_sin(x); }
		static inline float  cosf   (float   x)           { return __builtin_cosf(x); }
		static inline float  sinf   (float   x)           { return __builtin_sinf(x); }
		static inline float  acosf  (float   x)           { return __builtin_acosf(x); }
		static inline float  asinf  (float   x)           { return __builtin_asinf(x); }
		static inline float  atan2f (float   x, float y)  { return __builtin_atan2f(x, y); }
		static inline float  atanf  (float   x)           { return __builtin_atanf(x); }
		static inline float  ceilf  (float   x)           { return __builtin_ceilf(x); }
		static inline float  coshf  (float   x)           { return __builtin_coshf(x); }
		static inline float  expf   (float   x)           { return __builtin_expf(x); }
		static inline double fabs   (double  x)           { return __builtin_fabs(x); }
		static inline double ceil   (double  x)           { return __builtin_ceil(x); }
		static inline double floor  (double  x)           { return __builtin_floor(x); }
		static inline double fmod   (double  x, double y) { return __builtin_fmod(x, y); }
		static inline float  floorf (float   x)           { return __builtin_floorf(x); }
		static inline double round  (double  x)           { return __builtin_round(x); }
		static inline double sqrt   (double  x)           { return __builtin_sqrt(x); }
		static inline float  sqrtf  (float   x)           { return __builtin_sqrtf(x); }
		static inline float  powf   (float   x, float y)  { return __builtin_powf(x, y); }
		static inline double pow    (double  x, double y) { return __builtin_pow(x, y); }
		static inline double abs    (double  x)           { return __builtin_fabs(x); }
		static inline float  abs    (float   x)           { return __builtin_fabsf(x); }
		static inline double frexp  (double x, int *y)    { return __builtin_frexp(x,y); }

		// TODO: Why are the compilers so mad about this single function?
		#if defined(__clang__)
			extern "C" int abs(int);
		#elif defined(__GNUC__)
			extern "C" int abs(int) noexcept;
		#else
			extern "C" int abs(int);
		#endif
	#endif
#endif