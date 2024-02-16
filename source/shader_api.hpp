////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
// Shader API
//
// The sole purpose of this header is to provide IDE syntax highlighting for .shader files without the need for
// a custom plugin/language server. In short, this file defines C++ macros, structs, and functions that
// let us to treat .shader files as .cpp files such that IntelliSense and IDE syntax highlighting
// can piggyback off a typical, unmodified C++ language server.
//
// This header is NOT compiled.
//
// Upon "compilation" of .shader files in build.exe, the '#include <shader_api.hpp>' is redirected to
// source/build/shaders/to_<target>/shader_api.hpp. Those headers provide preprocessor macros to convert
// .shader intrinsic functions and built-in types (HLSL-like) to the target language equivalents.
//
// The shader compilation pipeline works as follows:
//     1. build.exe searches engine and project directories for .shader files
//     2. build.exe checks if a .shader file contains vertex_main(), fragment_main(), or compute_main()
//     3. build.exe runs clang/gcc preprocessor on the .shader file for C-like macros and #includes
//     4. build.exe parses the preprocessed .shader files and applies the target language translations
//     5. build.exe writes the finished .glsl/.hlsl/.metal files
//     6. build.exe compiles and packs the .glsl/.hlsl/.metal shaders into the project .bin
//

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define operator_overloads( type ) \
	template <typename T> type operator*( T other ); \
	template <typename T> type operator*=( T other ); \
	template <typename T> type operator/( T other ); \
	template <typename T> type operator/=( T other ); \
	template <typename T> type operator+( T other ); \
	template <typename T> type operator+=( T other ); \
	template <typename T> type operator-( T other ); \
	template <typename T> type operator-=( T other ); \
	template <typename T> bool operator==( T other ); \
	template <typename T> bool operator!=( T other );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Keywords

#define gfxbuffer struct
#define in In
#define out Out

#define SEMANTIC(x)
#define REGISTER(x)
#define RENDER_TARGET(x)

#define POSITION
#define TEXCOORD
#define COLOR
#define NORMAL
#define TANGENT
#define BINORMAL
#define DEPTH
#define VIEW
#define WORLD
#define INSTANCEID
#define BINDINDICES
#define PSIZE

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Base Types

using uint = unsigned int;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Vector 2D

#define vector_type_2( type ) \
	struct type##2 { \
		type##2() = default; \
		type##2( type x, type y ); \
		type##2( type v ); \
		operator_overloads( type##2 ); \
		type &operator[]( int index ); \
		union { \
			type x, y; \
			type r, g; \
		}; \
	};

vector_type_2( float );
vector_type_2( int );
vector_type_2( uint );
vector_type_2( bool );
vector_type_2( double );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Vector 3D

#define vector_type_3( type ) \
	struct type##3 { \
		type##3() = default; \
		type##3( type x, type y, type z = 0 ); \
		type##3( type v ); \
		operator_overloads( type##3 ); \
		type &operator[]( int index ); \
		union { \
			type x, y, z; \
			type r, g, b; \
			type##2 xy; \
			type##2 rg; \
			type##2 yz; \
			type##2 gb; \
		}; \
	};

vector_type_3( float );
vector_type_3( int );
vector_type_3( uint );
vector_type_3( bool );
vector_type_3( double );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Vector 4D

#define vector_type_4( type ) \
	struct type##4 { \
		type##4() = default; \
		type##4( type x, type y, type z = 0, type w = 0 ); \
		type##4( type v ); \
		operator_overloads( type##4 ); \
		type &operator[]( int index ); \
		union { \
			type x, y, z, w; \
			type r, g, b, a; \
			type##2 xy; \
			type##2 rg; \
			type##2 yz; \
			type##2 gb; \
			type##2 zw; \
			type##2 ba; \
			type##3 xyz; \
			type##3 rgb; \
			type##3 yzw; \
			type##3 gba; \
		}; \
	};

vector_type_4( float );
vector_type_4( int );
vector_type_4( uint );
vector_type_4( bool );
vector_type_4( double );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Matrix 2x2

struct float2x2
{
	float2x2() = default;
	float2x2( float x1y1, float x2y1,
	          float x1y2, float x2y2 );
	operator_overloads( float2x2 );
	float2 &operator[]( int index );
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Matrix 3x3

struct float3x3
{
	float3x3() = default;
	float3x3( float x1y1, float x2y1, float x3y1,
	          float x1y2, float x2y2, float x3y2,
	          float x1y3, float x2y3, float x3y3 );
	operator_overloads( float3x3 );
	float3 &operator[]( int index );
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Matrix 4x4

struct float4x4
{
	float4x4() = default;
	float4x4( float x1y1, float x2y1, float x3y1, float x4y1,
	          float x1y2, float x2y2, float x3y2, float x4y2,
			  float x1y3, float x2y3, float x3y3, float x4y3,
	          float x1y4, float x2y4, float x3y4, float x4y4 );
	operator_overloads( float4x4 );
	float4 &operator[]( int index );
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Texures & Samplers

struct _texture2D;

#define Texture2D _texture2D;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Bindings

//template <typename A, typename B> auto mul( A a, B b ) { return a * b; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 0

template <typename A, typename B> struct pair { A a; B b; };

/*
template <typename T> auto abort(T x);
template <typename T> auto abs(T x);
template <typename T> auto acos(T x);
template <typename T> bool all(T x);
template <typename T> void AllMemoryBarrier();
template <typename T> void AllMemoryBarrierWithGroupSync();
template <typename T> bool any(T x);
//template <typename T> double asdouble(T x);
template <typename T> float asfloat(T x); // implicit in GLSL
template <typename T> auto asin(T x);
template <typename T> int asint(T x); // implicit in GLSL
//template <typename T> unsigned int asuint(T x);
template <typename T> auto atan(T x);
template <typename T> auto atan2(T x, T y);
template <typename T> auto ceil(T x);
//template <typename T> bool CheckAccessFullyMapped(T x);
template <typename T> auto clamp(T x, T min, T max);
template <typename T> void clip(T x);
template <typename T> auto cos(T x);
template <typename T> auto cosh(T x);
template <typename T> int countbits(T x);
template <typename T> auto cross(T x, T y);
template <typename T> auto D3DCOLORtoUBYTE4(T x);
template <typename T> auto ddx(T x);
template <typename T> auto ddx_coarse(T x);
template <typename T> auto ddx_fine(T x);
template <typename T> auto ddy(T x);
template <typename T> auto ddy_coarse(T x);
template <typename T> auto ddy_fine(T x);
template <typename T> auto degrees(T x);
template <typename T> auto determinant(T m);
template <typename T> void DeviceMemoryBarrier();
template <typename T> void DeviceMemoryBarrierWithGroupSync();
template <typename T> float distance(T x, T y);
template <typename T> auto dot(T x, T y);
template <typename T> auto dst(T x, T y);
//template <typename T> void errorf(T x);
template <typename T> void EvaluateAttributeCentroid(T x);
template <typename T> void EvaluateAttributeAtSample(T x, int sampleIndex);
template <typename T> void EvaluateAttributeSnapped(T x);
template <typename T> auto exp(T x);
template <typename T> auto exp2(T x);
template <typename T> float f16tof32(T x);
template <typename T> auto f32tof16(T x);
template <typename T> auto faceforward(T n, T i, T ng);
template <typename T> int firstbithigh(T x);
template <typename T> int firstbitlow(T x);
template <typename T> auto floor(T x);
//template <typename T> double fma(T a, T b, T c);
template <typename T> auto fmod(T x, T y);
template <typename T> auto frac(T x);
template <typename T> pair<T, int> frexp(T x);
template <typename T> auto fwidth(T x);
//template <typename T> int GetRenderTargetSampleCount(T x);
//template <typename T> auto GetRenderTargetSamplePosition(T x);
template <typename T> void GroupMemoryBarrier();
template <typename T> void GroupMemoryBarrierWithGroupSync();
template <typename T> auto InterlockedAdd(T dest, T value);
template <typename T> auto InterlockedAnd(T dest, T value);
template <typename T> auto InterlockedCompareExchange(T dest, T compare, T value);
template <typename T> auto InterlockedCompareStore(T dest, T compare, T value);
template <typename T> auto InterlockedExchange(T dest, T value);
template <typename T> auto InterlockedMax(T dest, T value);
template <typename T> auto InterlockedMin(T dest, T value);
template <typename T> auto InterlockedOr(T dest, T value);
template <typename T> auto InterlockedXor(T dest, T value);
template <typename T> bool isfinite(T x);
template <typename T> bool isinf(T x);
template <typename T> bool isnan(T x);
template <typename T> auto ldexp(T x, int exp);
template <typename T> float length(T v);
template <typename T> auto lerp(T x, T y, float s);
template <typename T> auto lit(T n_dot_l, T n_dot_h, T m);
template <typename T> auto log(T x);
template <typename T> auto log10(T x);
template <typename T> auto log2(T x);
template <typename T> auto mad(T a, T b, T c);
template <typename T> auto max(T x, T y);
template <typename T> auto min(T x, T y);
template <typename T> pair<T, T> modf(T x);
template <typename T> auto msad4(T reference, T source);
template <typename T> auto mul(T x, T y);
template <typename T> auto noise(T x);
template <typename T> auto normalize(T x);
template <typename T> auto pow(T x, T y);
//template <typename T> void printf(T message);
template <typename T> void Process2DQuadTessFactorsAvg(T x);
template <typename T> void Process2DQuadTessFactorsMax(T x);
template <typename T> void Process2DQuadTessFactorsMin(T x);
template <typename T> void ProcessIsolineTessFactors(T x);
template <typename T> void ProcessQuadTessFactorsAvg(T x);
template <typename T> void ProcessQuadTessFactorsMax(T x);
template <typename T> void ProcessQuadTessFactorsMin(T x);
template <typename T> void ProcessTriTessFactorsAvg(T x);
template <typename T> void ProcessTriTessFactorsMax(T x);
template <typename T> void ProcessTriTessFactorsMin(T x);
template <typename T> auto radians(T x);
template <typename T> auto rcp(T x);
template <typename T> auto reflect(T i, T n);
template <typename T> auto refract(T i, T n, float eta);
template <typename T> auto reversebits(T x);
template <typename T> auto round(T x);
template <typename T> float rsqrt(T x);
template <typename T> auto saturate(T x);
template <typename T> int sign(T x);
template <typename T> auto sin(T x);
template <typename T> pair<T, T> sincos(T x);
template <typename T> auto sinh(T x);
template <typename T> auto smoothstep(T min, T max, T x);
template <typename T> auto sqrt(T x);
template <typename T> auto step(T a, T x);
template <typename T> auto tan(T x);
template <typename T> auto tanh(T x);
template <typename T> auto tex1D(T s, T t);
template <typename T> auto tex1D(T s, T t, T ddx, T ddy);
template <typename T> auto tex1Dbias(T s, T bias);
template <typename T> auto tex1Dgrad(T s, T ddx, T ddy);
template <typename T> auto tex1Dlod(T s, T lod);
template <typename T> auto tex1Dproj(T s, T t);
template <typename T> auto tex2D(T s, T t);
template <typename T> auto tex2D(T s, T t, T ddx, T ddy);
template <typename T> auto tex2Dbias(T s, T t, T bias);
template <typename T> auto tex2Dgrad(T s, T t, T ddx, T ddy);
template <typename T> auto tex2Dlod(T s, T t, T lod);
template <typename T> auto tex2Dproj(T s, T t);
template <typename T> auto tex3D(T s, T t);
template <typename T> auto tex3D(T s, T t, T ddx, T ddy);
template <typename T> auto tex3Dbias(T s, T t, T bias);
template <typename T> auto tex3Dgrad(T s, T t, T ddx, T ddy);
template <typename T> auto tex3Dlod(T s, T t, T lod);
template <typename T> auto tex3Dproj(T s, T t);
template <typename T> auto texCUBE(T s, T t);
template <typename T> auto texCUBE(T s, T t, T ddx, T ddy);
template <typename T> auto texCUBEbias(T s, T t, T bias);
template <typename T> auto texCUBEgrad(T s, T t, T ddx, T ddy);
template <typename T> auto texCUBElod(T s, T t, T lod);
template <typename T> auto texCUBEproj(T s, T t);
template <typename T> auto transpose(T m);
template <typename T> auto trunc(T x);
*/

template <typename T> auto abort(T condition); // No direct equivalent in GLSL
template <typename T> auto abs(T x); // abs(x)
template <typename T> auto acos(T x); // acos(x)
template <typename T> bool all(T condition); // all(condition)
template <typename T> void AllMemoryBarrier(); // memoryBarrier()
template <typename T> void AllMemoryBarrierWithGroupSync(); // memoryBarrierShared()
template <typename T> bool any(T condition); // any(condition)
template <typename T> float asfloat(T x); // Implicit in GLSL
template <typename T> auto asin(T x); // asin(x)
template <typename T> int asint(T x); // Implicit in GLSL
template <typename T> auto atan(T x); // atan(x)
template <typename T> auto atan2(T x, T y); // atan(y, x)
template <typename T> auto ceil(T x); // ceil(x)
template <typename T> auto clamp(T x, T min, T max); // clamp(x, min, max)
template <typename T> void clip(T x); // discard
template <typename T> auto cos(T x); // cos(x)
template <typename T> auto cosh(T x); // cosh(x)
template <typename T> int countbits(T x); // bitCount(x)
template <typename T> auto cross(T vector1, T vector2); // cross(vector1, vector2)
template <typename T> auto D3DCOLORtoUBYTE4(T x); // No direct equivalent in GLSL
template <typename T> auto ddx(T x); // dFdx(x)
template <typename T> auto ddx_coarse(T x); // dFdxCoarse(x)
template <typename T> auto ddx_fine(T x); // dFdxFine(x)
template <typename T> auto ddy(T x); // dFdy(x)
template <typename T> auto ddy_coarse(T x); // dFdyCoarse(x)
template <typename T> auto ddy_fine(T x); // dFdyFine(x)
template <typename T> auto degrees(T radians); // degrees(radians)
template <typename T> auto determinant(T matrix); // determinant(matrix)
template <typename T> void DeviceMemoryBarrier(); // memoryBarrier()
template <typename T> void DeviceMemoryBarrierWithGroupSync(); // memoryBarrierShared()
template <typename T> float distance(T point1, T point2); // distance(point1, point2)
template <typename T> auto dot(T vector1, T vector2); // dot(vector1, vector2)
template <typename T> auto dst(T vector1, T vector2); // distance(vector1, vector2)
template <typename T> void EvaluateAttributeCentroid(T x); // centroid
template <typename T> void EvaluateAttributeAtSample(T x, int sampleIndex); // sample
template <typename T> void EvaluateAttributeSnapped(T x); // No direct equivalent in GLSL
template <typename T> auto exp(T x); // exp(x)
template <typename T> auto exp2(T x); // exp2(x)
template <typename T> float f16tof32(T x); // No direct equivalent in GLSL
template <typename T> auto f32tof16(T x); // No direct equivalent in GLSL
template <typename T> auto faceforward(T normal, T incident, T ng); // faceforward(normal, incident, ng)
template <typename T> int firstbithigh(T x); // findMSB(x)
template <typename T> int firstbitlow(T x); // findLSB(x)
template <typename T> auto floor(T x); // floor(x)
template <typename T> auto fmod(T x, T y); // mod(x, y)
template <typename T> auto frac(T x); // fract(x)
template <typename T> pair<T, int> frexp(T x); // frexp(x)
template <typename T> auto fwidth(T x); // fwidth(x)
template <typename T> void GroupMemoryBarrier(); // memoryBarrier()
template <typename T> void GroupMemoryBarrierWithGroupSync(); // memoryBarrierShared()
template <typename T> auto InterlockedAdd(T dest, T value); // atomicAdd(dest, value)
template <typename T> auto InterlockedAnd(T dest, T value); // atomicAnd(dest, value)
template <typename T> auto InterlockedCompareExchange(T dest, T compare, T value); // atomicCompSwap(dest, compare, value)
template <typename T> auto InterlockedCompareStore(T dest, T compare, T value); // atomicCompSwap(dest, compare, value)
template <typename T> auto InterlockedExchange(T dest, T value); // atomicExchange(dest, value)
template <typename T> auto InterlockedMax(T dest, T value); // atomicMax(dest, value)
template <typename T> auto InterlockedMin(T dest, T value); // atomicMin(dest, value)
template <typename T> auto InterlockedOr(T dest, T value); // atomicOr(dest, value)
template <typename T> auto InterlockedXor(T dest, T value); // atomicXor(dest, value)
template <typename T> bool isfinite(T x); // isfinite(x)
template <typename T> bool isinf(T x); // isinf(x)
template <typename T> bool isnan(T x); // isnan(x)
template <typename T> auto ldexp(T x, int exp); // ldexp(x, exp)
template <typename T> float length(T vector); // length(vector)
template <typename T> auto lerp(T x, T y, float s); // mix(x, y, s)
template <typename T> auto lit(T n_dot_l, T n_dot_h, T m); // No direct equivalent in GLSL
template <typename T> auto log(T x); // log(x)
template <typename T> auto log10(T x); // log10(x)
template <typename T> auto log2(T x); // log2(x)
template <typename T> auto mad(T a, T b, T c); // a * b + c
template <typename T> auto max(T x, T y); // max(x, y)
template <typename T> auto min(T x, T y); // min(x, y)
template <typename T> pair<T, T> modf(T x); // modf(x)
template <typename T> auto msad4(T reference, T source); // No direct equivalent in GLSL
template <typename A, typename B> auto mul(A x, B y); // x * y
template <typename T> auto noise(T x); // noise(x)
template <typename T> auto normalize(T x); // normalize(x)
template <typename T> auto pow(T x, T y); // pow(x, y)
template <typename T> void Process2DQuadTessFactorsAvg(T x); // No direct equivalent in GLSL
template <typename T> void Process2DQuadTessFactorsMax(T x); // No direct equivalent in GLSL
template <typename T> void Process2DQuadTessFactorsMin(T x); // No direct equivalent in GLSL
template <typename T> void ProcessIsolineTessFactors(T x); // No direct equivalent in GLSL
template <typename T> void ProcessQuadTessFactorsAvg(T x); // No direct equivalent in GLSL
template <typename T> void ProcessQuadTessFactorsMax(T x); // No direct equivalent in GLSL
template <typename T> void ProcessQuadTessFactorsMin(T x); // No direct equivalent in GLSL
template <typename T> void ProcessTriTessFactorsAvg(T x); // No direct equivalent in GLSL
template <typename T> void ProcessTriTessFactorsMax(T x); // No direct equivalent in GLSL
template <typename T> void ProcessTriTessFactorsMin(T x); // No direct equivalent in GLSL
template <typename T> auto radians(T degrees); // radians(degrees)
template <typename T> auto rcp(T x); // 1.0 / x
template <typename T> auto reflect(T incident, T normal); // reflect(incident, normal)
template <typename T> auto refract(T incident, T normal, float eta); // refract(incident, normal, eta)
template <typename T> auto reversebits(T x); // bitfieldReverse(x)
template <typename T> auto round(T x); // round(x)
template <typename T> float rsqrt(T x); // inversesqrt(x)
template <typename T> auto saturate(T x); // saturate(x)
template <typename T> int sign(T x); // sign(x)
template <typename T> auto sin(T x); // sin(x)
template <typename T> pair<T, T> sincos(T x); // vec2(sin(x), cos(x))
template <typename T> auto sinh(T x); // sinh(x)
template <typename T> auto smoothstep(T min, T max, T x); // smoothstep(min, max, x)
template <typename T> auto sqrt(T x); // sqrt(x)
template <typename T> auto step(T threshold, T x); // step(threshold, x)
template <typename T> auto tan(T x); // tan(x)
template <typename T> auto tanh(T x); // tanh(x)
template <typename T> auto tex1D(T s, T t); // texture(sampler1D, s)
template <typename T> auto tex1D(T s, T t, T ddx, T ddy); // textureGrad(sampler1D, s, dPdx, dPdy)
template <typename T> auto tex1Dbias(T s, T bias); // texture(sampler1D, s, bias)
template <typename T> auto tex1Dgrad(T s, T ddx, T ddy); // textureGrad(sampler1D, s, dPdx, dPdy)
template <typename T> auto tex1Dlod(T s, T lod); // textureLod(sampler1D, s, lod)
template <typename T> auto tex1Dproj(T s, T t); // textureProj(sampler1D, vec4(s, t, 0, 1))
template <typename T> auto tex2D(T s, T t); // texture(sampler2D, vec2(s, t))
template <typename T> auto tex2D(T s, T t, T ddx, T ddy); // textureGrad(sampler2D, vec2(s, t), dPdx, dPdy)
template <typename T> auto tex2Dbias(T s, T t, T bias); // texture(sampler2D, vec2(s, t), bias)
template <typename T> auto tex2Dgrad(T s, T t, T ddx, T ddy); // textureGrad(sampler2D, vec2(s, t), dPdx, dPdy)
template <typename T> auto tex2Dlod(T s, T t, T lod); // textureLod(sampler2D, vec2(s, t), lod)
template <typename T> auto tex2Dproj(T s, T t); // textureProj(sampler2D, vec4(s, t, 0, 1))
template <typename T> auto tex3D(T s, T t); // texture(sampler3D, vec3(s, t, r))
template <typename T> auto tex3D(T s, T t, T ddx, T ddy); // textureGrad(sampler3D, vec3(s, t, r), dPdx, dPdy)
template <typename T> auto tex3Dbias(T s, T t, T bias); // texture(sampler3D, vec3(s, t, r), bias)
template <typename T> auto tex3Dgrad(T s, T t, T ddx, T ddy); // textureGrad(sampler3D, vec3(s, t, r), dPdx, dPdy)
template <typename T> auto tex3Dlod(T s, T t, T lod); // textureLod(sampler3D, vec3(s, t, r), lod)
template <typename T> auto tex3Dproj(T s, T t); // textureProj(sampler3D, vec4(s, t, r, 1))
template <typename T> auto texCUBE(T s, T t); // texture(samplerCube, vec3(s, t, r))
template <typename T> auto texCUBE(T s, T t, T ddx, T ddy); // textureGrad(samplerCube, vec3(s, t, r), dPdx, dPdy)
template <typename T> auto texCUBEbias(T s, T t, T bias); // texture(samplerCube, vec3(s, t, r), bias)
template <typename T> auto texCUBEgrad(T s, T t, T ddx, T ddy); // textureGrad(samplerCube, vec3(s, t, r), dPdx, dPdy)
template <typename T> auto texCUBElod(T s, T t, T lod); // textureLod(samplerCube, vec3(s, t, r), lod)
template <typename T> auto texCUBEproj(T s, T t); // textureProj(samplerCube, vec4(s, t, r, 1))
template <typename T> auto transpose(T matrix); // transpose(matrix)
template <typename T> auto trunc(T x); // trunc(x)


/*
abort	Terminates the current draw or dispatch call being executed.	4
abs	Absolute value (per component).	1¹
acos	Returns the arccosine of each component of x.	1¹
all	Test if all components of x are nonzero.	1¹
AllMemoryBarrier	Blocks execution of all threads in a group until all memory accesses have been completed.	5
AllMemoryBarrierWithGroupSync	Blocks execution of all threads in a group until all memory accesses have been completed and all threads in the group have reached this call.	5
any	Test if any component of x is nonzero.	1¹
asdouble	Reinterprets a cast value into a double.	5
asfloat	Convert the input type to a float.	4
asin	Returns the arcsine of each component of x.	1¹
asint	Convert the input type to an integer.	4
asuint	Reinterprets the bit pattern of a 64-bit type to a uint.	5
asuint	Convert the input type to an unsigned integer.	4
atan	Returns the arctangent of x.	1¹
atan2	Returns the arctangent of two values (x,y).	1¹
ceil	Returns the smallest integer which is greater than or equal to x.	1¹
CheckAccessFullyMapped	Determines whether all values from a Sample or Load operation accessed mapped tiles in a tiled resource.	5
clamp	Clamps x to the range [min, max].	1¹
clip	Discards the current pixel, if any component of x is less than zero.	1¹
cos	Returns the cosine of x.	1¹
cosh	Returns the hyperbolic cosine of x.	1¹
countbits	Counts the number of bits (per component) in the input integer.	5
cross	Returns the cross product of two 3D vectors.	1¹
D3DCOLORtoUBYTE4	Swizzles and scales components of the 4D vector xto compensate for the lack of UBYTE4 support in some hardware.	1¹
ddx	Returns the partial derivative of x with respect to the screen-space x-coordinate.	2¹
ddx_coarse	Computes a low precision partial derivative with respect to the screen-space x-coordinate.	5
ddx_fine	Computes a high precision partial derivative with respect to the screen-space x-coordinate.	5
ddy	Returns the partial derivative of x with respect to the screen-space y-coordinate.	2¹
ddy_coarse	Computes a low precision partial derivative with respect to the screen-space y-coordinate.	5
ddy_fine	Computes a high precision partial derivative with respect to the screen-space y-coordinate.	5
degrees	Converts x from radians to degrees.	1¹
determinant	Returns the determinant of the square matrix m.	1¹
DeviceMemoryBarrier	Blocks execution of all threads in a group until all device memory accesses have been completed.	5
DeviceMemoryBarrierWithGroupSync	Blocks execution of all threads in a group until all device memory accesses have been completed and all threads in the group have reached this call.	5
distance	Returns the distance between two points.	1¹
dot	Returns the dot product of two vectors.	1
dst	Calculates a distance vector.	5
errorf	Submits an error message to the information queue.	4
EvaluateAttributeCentroid	Evaluates at the pixel centroid.	5
EvaluateAttributeAtSample	Evaluates at the indexed sample location.	5
EvaluateAttributeSnapped	Evaluates at the pixel centroid with an offset.	5
exp	Returns the base-e exponent.	1¹
exp2	Base 2 exponent (per component).	1¹
f16tof32	Converts the float16 stored in the low-half of the uint to a float.	5
f32tof16	Converts an input into a float16 type.	5
faceforward	Returns -n * sign(dot(i, ng)).	1¹
firstbithigh	Gets the location of the first set bit starting from the highest order bit and working downward, per component.	5
firstbitlow	Returns the location of the first set bit starting from the lowest order bit and working upward, per component.	5
floor	Returns the greatest integer which is less than or equal to x.	1¹
fma	Returns the double-precision fused multiply-addition of a * b + c.	5
fmod	Returns the floating point remainder of x/y.	1¹
frac	Returns the fractional part of x.	1¹
frexp	Returns the mantissa and exponent of x.	2¹
fwidth	Returns abs(ddx(x)) + abs(ddy(x))	2¹
GetRenderTargetSampleCount	Returns the number of render-target samples.	4
GetRenderTargetSamplePosition	Returns a sample position (x,y) for a given sample index.	4
GroupMemoryBarrier	Blocks execution of all threads in a group until all group shared accesses have been completed.	5
GroupMemoryBarrierWithGroupSync	Blocks execution of all threads in a group until all group shared accesses have been completed and all threads in the group have reached this call.	5
InterlockedAdd	Performs a guaranteed atomic add of value to the dest resource variable.	5
InterlockedAnd	Performs a guaranteed atomic and.	5
InterlockedCompareExchange	Atomically compares the input to the comparison value and exchanges the result.	5
InterlockedCompareStore	Atomically compares the input to the comparison value.	5
InterlockedExchange	Assigns value to dest and returns the original value.	5
InterlockedMax	Performs a guaranteed atomic max.	5
InterlockedMin	Performs a guaranteed atomic min.	5
InterlockedOr	Performs a guaranteed atomic or.	5
InterlockedXor	Performs a guaranteed atomic xor.	5
isfinite	Returns true if x is finite, false otherwise.	1¹
isinf	Returns true if x is +INF or -INF, false otherwise.	1¹
isnan	Returns true if x is NAN or QNAN, false otherwise.	1¹
ldexp	Returns x * 2exp	1¹
length	Returns the length of the vector v.	1¹
lerp	Returns x + s(y - x).	1¹
lit	Returns a lighting vector (ambient, diffuse, specular, 1)	1¹
log	Returns the base-e logarithm of x.	1¹
log10	Returns the base-10 logarithm of x.	1¹
log2	Returns the base-2 logarithm of x.	1¹
mad	Performs an arithmetic multiply/add operation on three values.	5
max	Selects the greater of x and y.	1¹
min	Selects the lesser of x and y.	1¹
modf	Splits the value x into fractional and integer parts.	1¹
msad4	Compares a 4-byte reference value and an 8-byte source value and accumulates a vector of 4 sums.	5
mul	Performs matrix multiplication using x and y.	1
noise	Generates a random value using the Perlin-noise algorithm.	1¹
normalize	Returns a normalized vector.	1¹
pow	Returns xy.	1¹
printf	Submits a custom shader message to the information queue.	4
Process2DQuadTessFactorsAvg	Generates the corrected tessellation factors for a quad patch.	5
Process2DQuadTessFactorsMax	Generates the corrected tessellation factors for a quad patch.	5
Process2DQuadTessFactorsMin	Generates the corrected tessellation factors for a quad patch.	5
ProcessIsolineTessFactors	Generates the rounded tessellation factors for an isoline.	5
ProcessQuadTessFactorsAvg	Generates the corrected tessellation factors for a quad patch.	5
ProcessQuadTessFactorsMax	Generates the corrected tessellation factors for a quad patch.	5
ProcessQuadTessFactorsMin	Generates the corrected tessellation factors for a quad patch.	5
ProcessTriTessFactorsAvg	Generates the corrected tessellation factors for a tri patch.	5
ProcessTriTessFactorsMax	Generates the corrected tessellation factors for a tri patch.	5
ProcessTriTessFactorsMin	Generates the corrected tessellation factors for a tri patch.	5
radians	Converts x from degrees to radians.	1
rcp	Calculates a fast, approximate, per-component reciprocal.	5
reflect	Returns a reflection vector.	1
refract	Returns the refraction vector.	1¹
reversebits	Reverses the order of the bits, per component.	5
round	Rounds x to the nearest integer	1¹
rsqrt	Returns 1 / sqrt(x)	1¹
saturate	Clamps x to the range [0, 1]	1
sign	Computes the sign of x.	1¹
sin	Returns the sine of x	1¹
sincos	Returns the sine and cosine of x.	1¹
sinh	Returns the hyperbolic sine of x	1¹
smoothstep	Returns a smooth Hermite interpolation between 0 and 1.	1¹
sqrt	Square root (per component)	1¹
step	Returns (x >= a) ? 1 : 0	1¹
tan	Returns the tangent of x	1¹
tanh	Returns the hyperbolic tangent of x	1¹
tex1D(s, t)	1D texture lookup.	1
tex1D(s, t, ddx, ddy)	1D texture lookup.	2¹
tex1Dbias	1D texture lookup with bias.	2¹
tex1Dgrad	1D texture lookup with a gradient.	2¹
tex1Dlod	1D texture lookup with LOD.	3¹
tex1Dproj	1D texture lookup with projective divide.	2¹
tex2D(s, t)	2D texture lookup.	1¹
tex2D(s, t, ddx, ddy)	2D texture lookup.	2¹
tex2Dbias	2D texture lookup with bias.	2¹
tex2Dgrad	2D texture lookup with a gradient.	2¹
tex2Dlod	2D texture lookup with LOD.	3
tex2Dproj	2D texture lookup with projective divide.	2¹
tex3D(s, t)	3D texture lookup.	1¹
tex3D(s, t, ddx, ddy)	3D texture lookup.	2¹
tex3Dbias	3D texture lookup with bias.	2¹
tex3Dgrad	3D texture lookup with a gradient.	2¹
tex3Dlod	3D texture lookup with LOD.	3¹
tex3Dproj	3D texture lookup with projective divide.	2¹
texCUBE(s, t)	Cube texture lookup.	1¹
texCUBE(s, t, ddx, ddy)	Cube texture lookup.	2¹
texCUBEbias	Cube texture lookup with bias.	2¹
texCUBEgrad	Cube texture lookup with a gradient.	2¹
texCUBElod	Cube texture lookup with LOD.	3¹
texCUBEproj	Cube texture lookup with projective divide.	2¹
transpose	Returns the transpose of the matrix m.	1
trunc	Truncates floating-point value(s) to integer value(s)	1
 */

#endif