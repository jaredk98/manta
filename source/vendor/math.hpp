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
			extern float  cosf(float);
			extern double acos(double);
			extern float  acosf(float);
			extern double sin(double);
			extern float  sinf(float);
			extern double asin(double);
			extern float  asinf(float);
			extern double tan(double);
			extern float  tanf(float);
			extern double atan(double);
			extern float  atanf(float);
			extern double atan2(double, double);
			extern float  atan2f(float, float);

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

			// inline float cosf(float x) { return static_cast<float>(cos(x)); }
			// inline float sinf(float x) { return static_cast<float>(sin(x)); }

			// // #pragma intrinsic(cosf)
			// // #pragma intrinsic(sinf)
		}

		inline double abs(double x) { return fabs(x); }
		inline float  abs(float  x) { return static_cast<float>(fabs(x)); }
	#else
		inline double cos    (double  x)           { return __builtin_cos(x); }
		inline float  cosf   (float   x)           { return __builtin_cosf(x); }
		inline double acos   (double  x)           { return __builtin_acos(x); }
		inline float  acosf  (float   x)           { return __builtin_acosf(x); }
		inline double sin    (double  x)           { return __builtin_sin(x); }
		inline float  sinf   (float   x)           { return __builtin_sinf(x); }
		inline float  asinf  (float   x)           { return __builtin_asinf(x); }
		inline float  tan    (double  x)           { return __builtin_tan(x); }
		inline float  tanf   (float   x)           { return __builtin_tanf(x); }
		inline float  atan   (double  x)           { return __builtin_atan(x); }
		inline float  atanf  (float   x)           { return __builtin_atanf(x); }
		inline float  atan2  (double  x, double y) { return __builtin_atan2(x, y); }
		inline float  atan2f (float   x, float  y) { return __builtin_atan2f(x, y); }
		inline float  ceilf  (float   x)           { return __builtin_ceilf(x); }
		inline float  coshf  (float   x)           { return __builtin_coshf(x); }
		inline float  expf   (float   x)           { return __builtin_expf(x); }
		inline double fabs   (double  x)           { return __builtin_fabs(x); }
		inline double ceil   (double  x)           { return __builtin_ceil(x); }
		inline double floor  (double  x)           { return __builtin_floor(x); }
		inline double fmod   (double  x, double y) { return __builtin_fmod(x, y); }
		inline float  floorf (float   x)           { return __builtin_floorf(x); }
		inline double round  (double  x)           { return __builtin_round(x); }
		inline double sqrt   (double  x)           { return __builtin_sqrt(x); }
		inline float  sqrtf  (float   x)           { return __builtin_sqrtf(x); }
		inline float  powf   (float   x, float  y) { return __builtin_powf(x, y); }
		inline double pow    (double  x, double y) { return __builtin_pow(x, y); }
		inline double abs    (double  x)           { return __builtin_fabs(x); }
		inline float  abs    (float   x)           { return __builtin_fabsf(x); }
		inline double frexp  (double x, int *y)    { return __builtin_frexp(x,y); }

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