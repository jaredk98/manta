#pragma once

#include <types.hpp>
#include <vendor/math.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define PI ( 3.14159265358979f )
#define DEG2RAD ( PI / 180.0f )
#define RAD2DEG ( 180.0f / PI )

#define ALIGN_16( value ) ( ( static_cast<int>( value ) >> 4 ) << 4 )
#define ALIGN_16_CEIL( value ) ( ( ( static_cast<int>( value ) >> 4 ) + 1 ) << 4 )
#define ALIGN_POWER( value, power ) ( ( static_cast<int>( value ) >> power ) << power )
#define ALIGN_POWER_CEIL( value, power ) ( ( ( static_cast<int>( value ) >> power ) + 1 ) << power )

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Matrix;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline float fast_fmod(float x, float y)
{
	return x - ( static_cast<int>( x / y ) ) * y;
}


inline int fast_floor( float x )
{
	int    i = static_cast<int>( x );
	return i - ( i > x );
}


inline u32 ceilpow2( u32 v )
{
	v -= 1;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v += 1;
	return v;
}


extern void fast_sin_cos( const float angle, float &sin, float &cos );


inline float degtorad( float degree )
{
	return degree * ( PI / 180.0f );
}


inline float radtodeg( float radian )
{
	return radian * ( 180.0f / PI );
}


template <typename T> inline T lengthdir_x( T dist, float angle )
{
	return static_cast<T>( static_cast<float>( dist ) * cosf( degtorad( angle ) ) );
}


template <typename T> inline T lengthdir_y( T dist, float angle )
{
	return static_cast<T>( static_cast<float>( dist ) * sinf( degtorad( angle ) ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T> inline T min( const T val1, const T val2 )
{
	return ( val1 < val2 ) ? val1 : val2;
}


template <typename T> inline T max( const T val1, const T val2 )
{
	return ( val1 > val2 ) ? val1 : val2;
}


template <typename T> inline T clamp( T value, const T min, const T max )
{
	if( value < min ) { value = min; }
	if( value > max ) { value = max; }
	return value;
}


template <typename T> inline T wrap( const T &value, const T &min_value, const T &max_value )
{
	return ( value > max_value ? min_value : ( value < min_value ? max_value : value ) );
}


template <typename T> inline void wrap( const T &value, const T &min_value, const T &max_value, T &out_value )
{
	out_value = ( value > max_value ? min_value : ( value < min_value ? max_value : value ) );
}


template <typename T> inline T lerp( const T value1, const T value2, const float amount )
{
	if( amount <= 0.0f ) { return value1; }
	if( amount >= 1.0f ) { return value2; }
	return static_cast<T>( value1 + ( value2 - value1 ) * amount );
}


template <typename T> inline void lerp( const T &value1, const T value2, const float amount, T &result )
{
	if( amount <= 0.0f ) { result = value1; return; }
	if( amount >= 1.0f ) { result = value2; return; }
	result = static_cast<T>( value1 + ( value2 - value1 ) * amount );
}


template <typename T> inline bool tween( T &variable, const T target, const T factor, const T delta, const float threshold = 0.25f )
{
	if( fabs( static_cast<double>( variable - target ) ) < static_cast<double>( threshold ) )
	{
		variable = target;
		return false;
	}
	else
	{
		variable += static_cast<T>( static_cast<float>( target - variable ) / max( static_cast<float>( factor / delta ), 1.0f ) );
		return true;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern bool matrix_equal( const Matrix &a, const Matrix &b );

extern Matrix matrix_transpose( const Matrix &m );

extern Matrix matrix_inverse( const Matrix &m );

extern Matrix matrix_multiply( const Matrix &a, const Matrix &b );

extern Matrix matrix_multiply_scalar( const Matrix &a, const float scalar );

extern Matrix matrix_add( const Matrix &a, const Matrix &b );

extern Matrix matrix_sub( const Matrix &a, const Matrix &b );

extern Matrix matrix_build_zeros();

extern Matrix matrix_build_identity();

extern Matrix matrix_build_scaling( const float xscale, const float yscale, const float zscale );

extern Matrix matrix_build_translation( const float xtrans, const float ytrans, const float ztrans );

extern Matrix matrix_build_rotation_x( const float rad );

extern Matrix matrix_build_rotation_y( const float rad );

extern Matrix matrix_build_rotation_z( const float rad );

extern Matrix matrix_build_orthographic( float left, float right, float top, float bottom, float znear, float zfar );

extern Matrix matrix_build_perspective( const float fov, const float aspect, const float znear, const float zfar );

extern Matrix matrix_build_lookat( const float x, const float y, const float z, const float xto, const float yto, const float zto,
                                   const float xup, const float yup, const float zup );

extern Matrix matrix_build_ndc( const float width, const float height );

struct Matrix
{
	float data[16];

	Matrix()
	{
		for( int i = 0; i < 16; i++ ) { data[i] = 0.0f; }
	}

	Matrix( const Matrix &m )
	{
		for( int i = 0; i < 16; i++ ) { data[i] = m.data[i]; }
	}

	float &operator[]( const int index ) { return data[index]; }
	float operator[]( const int index ) const { return data[index]; }
	bool operator==( const Matrix &m ) { return matrix_equal( *this, m ); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
struct Vector2D
{
	using VectorType = Vector2D<T>;
	T x, y;

	Vector2D( T x = 0, T y = 0 ) : x{ x }, y{ y } { }
	Vector2D( const VectorType &v ) : x{ v.x }, y{ v.y } { }
	VectorType &operator=( const VectorType &v ) { x = v.x; y = v.y; return *this; }

	/* Automatically cast vector types (if valid component cast exists) */
	template <typename S> Vector2D( Vector2D<S> &v ) :
		x{ static_cast<T>( v.x ) }, y{ static_cast<T>( v.y ) } { }
	template <typename S> Vector2D( S x, S y ) :
		x{ static_cast<T>( x ) }, y{ static_cast<T>( y ) } { }

	template <typename S> VectorType &operator=( const Vector2D<S> &v )
	{
		x = static_cast<T>( v.x );
		y = static_cast<T>( v.y );
		return *this;
	}

	inline T dot( const VectorType &v ) const
	{
		return x * v.x + y * v.y;
	}

	inline T length() const
	{
		return static_cast<T>( sqrt( static_cast<double>( x * x + y * y ) ) );
	}

	inline T length_sqr() const
	{
		return x * x + y * y;
	}

	inline VectorType &add( const VectorType &v )
	{
		x += v.x;
		y += v.y;
		return *this;
	}

	inline VectorType &sub( const VectorType &v )
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}

	inline VectorType &multiply( const VectorType &v )
	{
		x *= v.x;
		y *= v.y;
		return *this;
	}

	inline VectorType &multiply( const double s )
	{
		x = static_cast<T>( x * s );
		y = static_cast<T>( y * s );
		return *this;
	}

    inline VectorType &multiply( const Matrix &matrix )
	{
		const float h[4] =
		{
			static_cast<float>( x ),
			static_cast<float>( y ),
			0.0f,
			1.0f
		};

		float r[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

		for( int i = 0; i < 16; ++i ) { r[i % 4] += matrix[i] * h[i / 4]; }

		x = static_cast<T>( r[0] / r[3] );
		y = static_cast<T>( r[1] / r[3] );
		return *this;
    }

	inline VectorType &divide( const VectorType &v )
	{
		x /= v.x;
		y /= v.y;
		return *this;
	}

	inline VectorType &divide( const double s )
	{
		x = static_cast<T>( x / s );
		y = static_cast<T>( y / s );
		return *this;
	}

	inline T cross( const VectorType &v ) const
	{
		return x * v.y - y * v.x;
	}

	inline VectorType &normalize()
	{
		const double l = static_cast<double>( this->length() );
		this->divide( l );
		return *this;
	}

	inline VectorType &project( const VectorType &onto )
	{
		const T d = this->dot( onto );
		const T l = onto.length_sqr();
		this->x = static_cast<T>( ( d / static_cast<double>( l ) ) * onto.x );
		this->y = static_cast<T>( ( d / static_cast<double>( l ) ) * onto.y );
		return *this;
	}

	inline VectorType &rotate( const double angle, const VectorType &origin = { } )
	{
		const T tX = x - origin.x;
		const T tY = y - origin.y;
		const double cT = cos( angle * DEG2RAD );
		const double sT = sin( angle * DEG2RAD );
		x = static_cast<T>( tX * cT - tY * sT + origin.x );
		y = static_cast<T>( tX * sT + tY * cT + origin.y );
		return *this;
	}

	inline VectorType &reflect( const VectorType &normal )
	{
		VectorType n { normal };
		this->sub( n.multiply( this->dot( normal ) * 2.0 ) );
		return *this;
	}

	inline VectorType &lerp( const VectorType &v, const double amount )
	{
		x = static_cast<T>( x + ( v.x - x ) * amount );
		y = static_cast<T>( y + ( v.y - y ) * amount );
		return *this;
	}

	VectorType operator+( const VectorType &v ) const { VectorType r { *this }; r.add( v );      return r; }
	VectorType operator-( const VectorType &v ) const { VectorType r { *this }; r.sub( v );      return r; }
	VectorType operator*( const VectorType &v ) const { VectorType r { *this }; r.multiply( v ); return r; }
	VectorType operator*( const double s )      const { VectorType r { *this }; r.multiply( s ); return r; }
	VectorType operator/( const VectorType &v ) const { VectorType r { *this }; r.divide( v );   return r; }
	VectorType operator/( const double s )      const { VectorType r { *this }; r.divide( s );   return r; }

	VectorType &operator+=( const VectorType &v ) { this->add( v );      return *this; }
	VectorType &operator-=( const VectorType &v ) { this->sub( v );      return *this; }
	VectorType &operator*=( const VectorType &v ) { this->multiply( v ); return *this; }
	VectorType &operator*=( const double s )      { this->multiply( s ); return *this; }
	VectorType &operator/=( const VectorType &v ) { this->divide( v );   return *this; }
	VectorType &operator/=( const double s )      { this->divide( s );   return *this; }

	bool operator==( const VectorType &v ) const { return x == v.x && y == v.y; }
};


#define VECTOR_TYPE_2D( Vec, T )                                                                             \
	using Vec = Vector2D<T>;                                                                                 \
	inline bool Vec##_equal( const Vec &a, const Vec &b )      { return a == b; }                            \
	inline    T Vec##_dot( const Vec &a, const Vec &b )        { return a.dot( b ); }                        \
	inline    T Vec##_length( const Vec &v )                   { return v.length(); }                        \
	inline    T Vec##_length_sqr( const Vec &v )               { return v.length_sqr(); }                    \
	inline  Vec Vec##_add( const Vec &a, const Vec &b )        { Vec r { a }; r.add( b ); return r; }        \
	inline  Vec Vec##_sub( const Vec &a, const Vec &b )        { Vec r { a }; r.sub( b ); return r; }        \
	inline  Vec Vec##_multiply( const Vec &a, const Vec &b )   { Vec r { a }; r.multiply( b ); return r; }   \
	inline  Vec Vec##_divide( const Vec &a, const Vec &b )     { Vec r { a }; r.divide( b ); return r; }     \
	inline  Vec Vec##_multiply( const Vec &v, const double s ) { Vec r { v }; r.multiply( s ); return r; }   \
	inline  Vec Vec##_divide( const Vec &v, const double s )   { Vec r { v }; r.divide( s ); return r; }     \
	inline    T Vec##_cross( const Vec &a, const Vec &b )      { return a.cross( b ); }                      \
	inline  Vec Vec##_normalize( const Vec &v )                { Vec r { v }; r.normalize(); return r; }     \
	inline  Vec Vec##_project( const Vec &v, const Vec &onto ) { Vec r { v }; r.project( onto ); return r; } \
	inline  Vec Vec##_rotate( const Vec &v, const float angle, const Vec &origin = { } )                     \
		{ Vec r { v }; r.rotate( angle, origin ); return r; }                                                \
	inline  Vec Vec##_reflect( const Vec &v, const Vec &normal )                                             \
		{ Vec r { v }; r.reflect( normal ); return r; }                                                      \
	inline Vec Vec##_lerp( const Vec &a, const Vec &b, const float amount )                                  \
		{ Vec r { a }; r.lerp( b, amount ); return r; }                                                      \
	inline   T Vec##_distance( const Vec &a, const Vec &b )                                                  \
		{ Vec r { b.x - a.x, b.y - a.y }; return r.length(); }                                               \
	inline   T Vec##_distance_sqr( const Vec &a, const Vec &b )                                              \
		{ Vec r { b.x - a.x, b.y - a.y }; return r.length_sqr(); }


VECTOR_TYPE_2D( i8v2, i8 )
VECTOR_TYPE_2D( u8v2, u8 )
VECTOR_TYPE_2D( i16v2, i16 )
VECTOR_TYPE_2D( u16v2, u16 )
VECTOR_TYPE_2D( i32v2, i32 )
VECTOR_TYPE_2D( u32v2, u32 )
VECTOR_TYPE_2D( i64v2, i64 )
VECTOR_TYPE_2D( u64v2, u64 )
VECTOR_TYPE_2D( boolv2, bool )
VECTOR_TYPE_2D( floatv2, float )
VECTOR_TYPE_2D( doublev2, double )

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
struct Vector3D
{
	using VectorType = Vector3D<T>;
	T x, y, z;

	Vector3D( T x = 0, T y = 0, T z = 0 ) : x{ x }, y{ y }, z{ z } { }
	Vector3D( const VectorType &v ) : x{ v.x }, y{ v.y }, z{ v.z } { }
	VectorType &operator=( const VectorType &v ) { x = v.x; y = v.y; z = v.z; return *this; }

	/* Automatically cast vector types (if valid component cast exists) */
	template <typename S> Vector3D( Vector3D<S> &v ) :
		x{ static_cast<T>( v.x ) }, y{ static_cast<T>( v.y ) }, z{ static_cast<T>( v.z ) } { }
	template <typename S> Vector3D( S x, S y, S z ) :
		x{ static_cast<T>( x ) }, y{ static_cast<T>( y ) }, z{ static_cast<T>( z ) } { }

	template <typename S> VectorType &operator=( const Vector3D<S> &v )
	{
		x = static_cast<T>( v.x );
		y = static_cast<T>( v.y );
		z = static_cast<T>( v.z );
		return *this;
	}

	inline Vector2D<T> xy() const { return Vector2D { x, y }; }
	inline Vector2D<T> yz() const { return Vector2D { y, z }; }

	inline T dot( const VectorType &v ) const
	{
		return x * v.x + y * v.y + z * v.z;
	}

	inline T length() const
	{
		return static_cast<T>( sqrt( static_cast<double>( x * x + y * y + z * z ) ) );
	}

	inline T length_sqr() const
	{
		return x * x + y * y + z * z;
	}

	inline VectorType &add( const VectorType &v )
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	inline VectorType &sub( const VectorType &v )
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	inline VectorType &multiply( const VectorType &v )
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}

	inline VectorType &multiply( const double s )
	{
		x = static_cast<T>( x * s );
		y = static_cast<T>( y * s );
		z = static_cast<T>( z * s );
		return *this;
	}

	inline VectorType &multiply( const Matrix &matrix )
	{
		const float h[4] =
		{
			static_cast<float>( x ),
			static_cast<float>( y ),
			static_cast<float>( z ),
			1.0f
		};

		float r[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

		for( int i = 0; i < 16; ++i ) { r[i % 4] += matrix[i] * h[i / 4]; }

		x = static_cast<T>( r[0] / r[3] );
		y = static_cast<T>( r[1] / r[3] );
		z = static_cast<T>( r[2] / r[3] );
		return *this;
	}

	inline VectorType &divide( const VectorType &v )
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
		return *this;
	}

	inline VectorType &divide( const double s )
	{
		x = static_cast<T>( x / s );
		y = static_cast<T>( y / s );
		z = static_cast<T>( z / s );
		return *this;
	}

	inline VectorType &cross( const VectorType &v )
	{
		VectorType r;
		r.x = y * v.z - z * v.y;
		r.y = z * v.x - x * v.z;
		r.z = x * v.y - y * v.x;
		*this = r;
		return *this;
	}

	inline VectorType &normalize()
	{
		const double l = static_cast<double>( this->length() );
		this->divide( l );
		return *this;
	}

	inline VectorType &project( const VectorType &onto )
	{
		const T d = this->dot( onto );
		const T l = onto.length_sqr();
		this->x = static_cast<T>( ( d / static_cast<double>( l ) ) * onto.x );
		this->y = static_cast<T>( ( d / static_cast<double>( l ) ) * onto.y );
		this->z = static_cast<T>( ( d / static_cast<double>( l ) ) * onto.z );
		return *this;
	}

	inline VectorType &rotate_x( const double angle, const VectorType &origin = {} )
	{
		const double tY = static_cast<double>( y ) - origin.y;
		const double tZ = static_cast<double>( z ) - origin.z;
		const double cT = cos( angle * DEG2RAD );
		const double sT = sin( angle * DEG2RAD );
		y = static_cast<T>( tY * cT - tZ * sT + origin.y );
		z = static_cast<T>( tY * sT + tZ * cT + origin.z );
		return *this;
	}

	inline VectorType &rotate_y( const double angle, const VectorType &origin = {} )
	{
		const double tX = static_cast<double>( x )- origin.x;
		const double tZ = static_cast<double>( z ) - origin.z;
		const double cT = cos( angle * DEG2RAD );
		const double sT = sin( angle * DEG2RAD );
		x = static_cast<T>( tX * cT + tZ * sT + origin.x );
		z = static_cast<T>( -tX * sT + tZ * cT + origin.z );
		return *this;
	}

	inline VectorType &rotate_z( const double angle, const VectorType &origin = {} )
	{
		const double tX = static_cast<double>( x ) - origin.x;
		const double tY = static_cast<double>( y ) - origin.y;
		const double cT = cos( angle * DEG2RAD );
		const double sT = sin( angle * DEG2RAD );
		x = static_cast<T>( tX * cT - tY * sT + origin.x );
		y = static_cast<T>( tX * sT + tY * cT + origin.y );
		return *this;
	}

	inline VectorType &reflect( const VectorType &normal )
	{
		VectorType n { normal };
		this->sub( n.multiply( this->dot( normal ) * 2.0 ) );
		return *this;
	}

	inline VectorType &lerp( const VectorType &v, const double amount )
	{
		x = static_cast<T>( x + ( v.x - x ) * amount );
		y = static_cast<T>( y + ( v.y - y ) * amount );
		z = static_cast<T>( z + ( v.z - z ) * amount );
		return *this;
	}

	VectorType operator+( const VectorType &v ) const { VectorType r { *this }; r.add( v );      return r; }
	VectorType operator-( const VectorType &v ) const { VectorType r { *this }; r.sub( v );      return r; }
	VectorType operator*( const VectorType &v ) const { VectorType r { *this }; r.multiply( v ); return r; }
	VectorType operator*( const double s )      const { VectorType r { *this }; r.multiply( s ); return r; }
	VectorType operator/( const VectorType &v ) const { VectorType r { *this }; r.divide( v );   return r; }
	VectorType operator/( const double s )      const { VectorType r { *this }; r.divide( s );   return r; }

	VectorType &operator+=( const VectorType &v ) { this->add( v );      return *this; }
	VectorType &operator-=( const VectorType &v ) { this->sub( v );      return *this; }
	VectorType &operator*=( const VectorType &v ) { this->multiply( v ); return *this; }
	VectorType &operator*=( const double s )      { this->multiply( s ); return *this; }
	VectorType &operator/=( const VectorType &v ) { this->divide( v );   return *this; }
	VectorType &operator/=( const double s )      { this->divide( s );   return *this; }

	bool operator==( const VectorType &v ) const { return x == v.x && y == v.y && z == v.z; }
};


#define VECTOR_TYPE_3D( Vec, T )                                                                             \
	using Vec = Vector3D<T>;                                                                                 \
	inline bool Vec##_equal( const Vec &a, const Vec &b )      { return a == b; }                            \
	inline    T Vec##_dot( const Vec &a, const Vec &b )        { return a.dot( b ); }                        \
	inline    T Vec##_length( const Vec &v )                   { return v.length(); }                        \
	inline    T Vec##_length_sqr( const Vec &v )               { return v.length_sqr(); }                    \
	inline  Vec Vec##_add( const Vec &a, const Vec &b )        { Vec r { a }; r.add( b ); return r; }        \
	inline  Vec Vec##_sub( const Vec &a, const Vec &b )        { Vec r { a }; r.sub( b ); return r; }        \
	inline  Vec Vec##_multiply( const Vec &a, const Vec &b )   { Vec r { a }; r.multiply( b ); return r; }   \
	inline  Vec Vec##_divide( const Vec &a, const Vec &b )     { Vec r { a }; r.divide( b ); return r; }     \
	inline  Vec Vec##_multiply( const Vec &v, const double s ) { Vec r { v }; r.multiply( s ); return r; }   \
	inline  Vec Vec##_divide( const Vec &v, const double s )   { Vec r { v }; r.divide( s ); return r; }     \
	inline  Vec Vec##_cross( const Vec &a, const Vec &b )      { Vec r { a }; r.cross( b ); return r; }      \
	inline  Vec Vec##_normalize( const Vec &v )                { Vec r { v }; r.normalize(); return r; }     \
	inline  Vec Vec##_project( const Vec &v, const Vec &onto ) { Vec r { v }; r.project( onto ); return r; } \
	inline  Vec Vec##_rotate_x( const Vec &v, const float angle, const Vec &origin = { } )                   \
		{ Vec r { v }; r.rotate_x( angle, origin ); return r; }                                              \
	inline  Vec Vec##_rotate_y( const Vec &v, const float angle, const Vec &origin = { } )                   \
		{ Vec r { v }; r.rotate_y( angle, origin ); return r; }                                              \
	inline  Vec Vec##_rotate_z( const Vec &v, const float angle, const Vec &origin = { } )                   \
		{ Vec r { v }; r.rotate_z( angle, origin ); return r; }                                              \
	inline  Vec Vec##_reflect( const Vec &v, const Vec &normal )                                             \
		{ Vec r { v }; r.reflect( normal ); return r; }                                                      \
	inline Vec Vec##_lerp( const Vec &a, const Vec &b, const float amount )                                  \
		{ Vec r { a }; r.lerp( b, amount ); return r; }                                                      \
	inline   T Vec##_distance( const Vec &a, const Vec &b )                                                  \
		{ Vec r { b.x - a.x, b.y - a.y, b.z - a.z }; return r.length(); }                                    \
	inline   T Vec##_distance_sqr( const Vec &a, const Vec &b )                                              \
		{ Vec r { b.x - a.x, b.y - a.y, b.z - a.z }; return r.length_sqr(); }


VECTOR_TYPE_3D( i8v3, i8 )
VECTOR_TYPE_3D( u8v3, u8 )
VECTOR_TYPE_3D( i16v3, i16 )
VECTOR_TYPE_3D( u16v3, u16 )
VECTOR_TYPE_3D( i32v3, i32 )
VECTOR_TYPE_3D( u32v3, u32 )
VECTOR_TYPE_3D( i64v3, i64 )
VECTOR_TYPE_3D( u64v3, u64 )
VECTOR_TYPE_3D( boolv3, bool );
VECTOR_TYPE_3D( floatv3, float )
VECTOR_TYPE_3D( doublev3, double )

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
struct Vector4D
{
	using VectorType = Vector4D<T>;
	T x, y, z, w;

	Vector4D( T x = 0, T y = 0, T z = 0, T w = 0 ) : x{ x }, y{ y }, z{ z }, w{ w } { }
	Vector4D( const VectorType &v ) : x{ v.x }, y{ v.y }, z{ v.z }, w{ v.w } { }
	VectorType &operator=( const VectorType &v ) { x = v.x; y = v.y; z = v.z; w = v.w; return *this; }

	/* Automatically cast vector types (if valid component cast exists) */
	template <typename S> Vector4D( Vector4D<S> &v ) :
		x{ static_cast<T>( v.x ) }, y{ static_cast<T>( v.y ) }, z{ static_cast<T>( v.z ) }, w{ static_cast<T>( v.w ) } { }
	template <typename S> Vector4D( S x, S y, S z, S w ) :
		x{ static_cast<T>( x ) }, y{ static_cast<T>( y ) }, z{ static_cast<T>( z ) }, w{ static_cast<T>( w ) } { }

	template <typename S> VectorType &operator=( const Vector3D<S> &v )
	{
		x = static_cast<T>( v.x );
		y = static_cast<T>( v.y );
		z = static_cast<T>( v.z );
		w = static_cast<T>( v.w );
		return *this;
	}

	inline Vector2D<T> xy()  const { return Vector2D<T>{ x, y }; }
	inline Vector2D<T> yz()  const { return Vector2D<T>{ y, z }; }
	inline Vector2D<T> zw()  const { return Vector2D<T>{ z, w }; }
	inline Vector3D<T> xyz() const { return Vector3D<T>{ x, y, z }; }
	inline Vector3D<T> yzw() const { return Vector3D<T>{ y, z, w }; }

	inline T dot( const VectorType &v ) const
	{
		return x * v.x + y * v.y + z * v.z + w * v.w;
	}

	inline T length() const
	{
		return static_cast<T>( sqrt( static_cast<double>( x * x + y * y + z * z + w * w ) ) );
	}

	inline T length_sqr() const
	{
		return x * x + y * y + z * z + w * w;
	}

	inline VectorType &add( const VectorType &v )
	{
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
		return *this;
	}

	inline VectorType &sub( const VectorType &v )
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;
		return *this;
	}

	inline VectorType &multiply( const VectorType &v )
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		w *= v.w;
		return *this;
	}

	inline VectorType &multiply( const double s )
	{
		x = static_cast<T>( x * s );
		y = static_cast<T>( y * s );
		z = static_cast<T>( z * s );
		w = static_cast<T>( w * s );
		return *this;
	}

	inline VectorType &multiply( const Matrix &matrix )
	{
		const float h[4] =
		{
			static_cast<float>( x ),
			static_cast<float>( y ),
			static_cast<float>( z ),
			static_cast<float>( w ),
		};

		float r[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

		for( int i = 0; i < 16; ++i ) { r[i % 4] += matrix[i] * h[i / 4]; }

		x = static_cast<T>( r[0] / r[3] );
		y = static_cast<T>( r[1] / r[3] );
		z = static_cast<T>( r[2] / r[3] );
		w = static_cast<T>( r[3] );
		return *this;
	}

	inline VectorType &divide( const VectorType &v )
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
		w /= v.w;
		return *this;
	}

	inline VectorType &divide( const double s )
	{
		x = static_cast<T>( x / s );
		y = static_cast<T>( y / s );
		z = static_cast<T>( z / s );
		w = static_cast<T>( w / s );
		return *this;
	}

	inline VectorType &cross( const VectorType &v )
	{
		VectorType r;
		r.x = y * v.z - z * v.y;
		r.y = z * v.x - x * v.z;
		r.z = x * v.y - y * v.x;
		// TODO: cross Vector4D
		*this = r;
		return *this;
	}

	inline VectorType &normalize()
	{
		const double l = static_cast<double>( this->length() );
		this->divide( l );
		return *this;
	}

	inline VectorType &project( const VectorType &onto )
	{
		const T d = this->dot( onto );
		const T l = onto.length_sqr();
		this->x = static_cast<T>( ( d / static_cast<double>( l ) ) * onto.x );
		this->y = static_cast<T>( ( d / static_cast<double>( l ) ) * onto.y );
		this->z = static_cast<T>( ( d / static_cast<double>( l ) ) * onto.z );
		this->w = static_cast<T>( ( d / static_cast<double>( l ) ) * onto.w );
		return *this;
	}

	inline VectorType &rotate_x( const double angle, const VectorType &origin = {} )
	{
		const double tY = static_cast<double>( y ) - origin.y;
		const double tZ = static_cast<double>( z ) - origin.z;
		const double cT = cos( angle * DEG2RAD );
		const double sT = sin( angle * DEG2RAD );
		y = static_cast<T>( tY * cT - tZ * sT + origin.y );
		z = static_cast<T>( tY * sT + tZ * cT + origin.z );
		return *this;
	}

	inline VectorType &rotate_y( const double angle, const VectorType &origin = {} )
	{
		const double tX = static_cast<double>( x )- origin.x;
		const double tZ = static_cast<double>( z ) - origin.z;
		const double cT = cos( angle * DEG2RAD );
		const double sT = sin( angle * DEG2RAD );
		x = static_cast<T>( tX * cT + tZ * sT + origin.x );
		z = static_cast<T>( -tX * sT + tZ * cT + origin.z );
		return *this;
	}

	inline VectorType &rotate_z( const double angle, const VectorType &origin = {} )
	{
		const double tX = static_cast<double>( x ) - origin.x;
		const double tY = static_cast<double>( y ) - origin.y;
		const double cT = cos( angle * DEG2RAD );
		const double sT = sin( angle * DEG2RAD );
		x = static_cast<T>( tX * cT - tY * sT + origin.x );
		y = static_cast<T>( tX * sT + tY * cT + origin.y );
		return *this;
	}

	inline VectorType &reflect( const VectorType &normal )
	{
		VectorType n { normal };
		this->sub( n.multiply( this->dot( normal ) * 2.0 ) );
		return *this;
	}

	inline VectorType &lerp( const VectorType &v, const double amount )
	{
		x = static_cast<T>( x + ( v.x - x ) * amount );
		y = static_cast<T>( y + ( v.y - y ) * amount );
		z = static_cast<T>( z + ( v.z - z ) * amount );
		w = static_cast<T>( w + ( v.w - w ) * amount );
		return *this;
	}

	VectorType operator+( const VectorType &v ) const { VectorType r { *this }; r.add( v );      return r; }
	VectorType operator-( const VectorType &v ) const { VectorType r { *this }; r.sub( v );      return r; }
	VectorType operator*( const VectorType &v ) const { VectorType r { *this }; r.multiply( v ); return r; }
	VectorType operator*( const double s )      const { VectorType r { *this }; r.multiply( s ); return r; }
	VectorType operator/( const VectorType &v ) const { VectorType r { *this }; r.divide( v );   return r; }
	VectorType operator/( const double s )      const { VectorType r { *this }; r.divide( s );   return r; }

	VectorType &operator+=( const VectorType &v ) { this->add( v );      return *this; }
	VectorType &operator-=( const VectorType &v ) { this->sub( v );      return *this; }
	VectorType &operator*=( const VectorType &v ) { this->multiply( v ); return *this; }
	VectorType &operator*=( const double s )      { this->multiply( s ); return *this; }
	VectorType &operator/=( const VectorType &v ) { this->divide( v );   return *this; }
	VectorType &operator/=( const double s )      { this->divide( s );   return *this; }

	bool operator==( const VectorType &v ) const { return x == v.x && y == v.y && z == v.z && w == v.w; }
};


#define VECTOR_TYPE_4D( Vec, T )                                                                             \
	using Vec = Vector4D<T>;                                                                                 \
	inline bool Vec##_equal( const Vec &a, const Vec &b )      { return a == b; }                            \
	inline    T Vec##_dot( const Vec &a, const Vec &b )        { return a.dot( b ); }                        \
	inline    T Vec##_length( const Vec &v )                   { return v.length(); }                        \
	inline    T Vec##_length_sqr( const Vec &v )               { return v.length_sqr(); }                    \
	inline  Vec Vec##_add( const Vec &a, const Vec &b )        { Vec r { a }; r.add( b ); return r; }        \
	inline  Vec Vec##_sub( const Vec &a, const Vec &b )        { Vec r { a }; r.sub( b ); return r; }        \
	inline  Vec Vec##_multiply( const Vec &a, const Vec &b )   { Vec r { a }; r.multiply( b ); return r; }   \
	inline  Vec Vec##_divide( const Vec &a, const Vec &b )     { Vec r { a }; r.divide( b ); return r; }     \
	inline  Vec Vec##_multiply( const Vec &v, const double s ) { Vec r { v }; r.multiply( s ); return r; }   \
	inline  Vec Vec##_divide( const Vec &v, const double s )   { Vec r { v }; r.divide( s ); return r; }     \
	inline  Vec Vec##_cross( const Vec &a, const Vec &b )      { Vec r { a }; r.cross( b ); return r; }      \
	inline  Vec Vec##_normalize( const Vec &v )                { Vec r { v }; r.normalize(); return r; }     \
	inline  Vec Vec##_project( const Vec &v, const Vec &onto ) { Vec r { v }; r.project( onto ); return r; } \
	inline  Vec Vec##_rotate_x( const Vec &v, const float angle, const Vec &origin = { } )                   \
		{ Vec r { v }; r.rotate_x( angle, origin ); return r; }                                              \
	inline  Vec Vec##_rotate_y( const Vec &v, const float angle, const Vec &origin = { } )                   \
		{ Vec r { v }; r.rotate_y( angle, origin ); return r; }                                              \
	inline  Vec Vec##_rotate_z( const Vec &v, const float angle, const Vec &origin = { } )                   \
		{ Vec r { v }; r.rotate_z( angle, origin ); return r; }                                              \
	inline  Vec Vec##_reflect( const Vec &v, const Vec &normal )                                             \
		{ Vec r { v }; r.reflect( normal ); return r; }                                                      \
	inline Vec Vec##_lerp( const Vec &a, const Vec &b, const float amount )                                  \
		{ Vec r { a }; r.lerp( b, amount ); return r; }                                                      \
	inline   T Vec##_distance( const Vec &a, const Vec &b )                                                  \
		{ Vec r { b.x - a.x, b.y - a.y, b.z - a.z, b.w - a.w }; return r.length(); }                         \
	inline   T Vec##_distance_sqr( const Vec &a, const Vec &b )                                              \
		{ Vec r { b.x - a.x, b.y - a.y, b.z - a.z, b.w - a.w }; return r.length_sqr(); }


VECTOR_TYPE_4D( i8v4, i8 )
VECTOR_TYPE_4D( u8v4, u8 )
VECTOR_TYPE_4D( i16v4, i16 )
VECTOR_TYPE_4D( u16v4, u16 )
VECTOR_TYPE_4D( i32v4, i32 )
VECTOR_TYPE_4D( u32v4, u32 )
VECTOR_TYPE_4D( i64v4, i64 )
VECTOR_TYPE_4D( u64v4, u64 )
VECTOR_TYPE_4D( boolv4, bool )
VECTOR_TYPE_4D( floatv4, float )
VECTOR_TYPE_4D( doublev4, double )

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline bool point_in_rect( const int px, const int py, const int x1, const int y1, const int x2, const int y2 )
	{ return ( px >= x1 && px <= x2 && py >= y1 && py <= y2 ); }

inline bool point_in_rect( const float px, const float py, const float x1, const float y1, const float x2, const float y2 )
	{ return ( px >= x1 && px <= x2 && py >= y1 && py <= y2 ); }

inline bool point_in_rect( const double px, const double py, const double x1, const double y1, const double x2, const double y2 )
	{ return ( px >= x1 && px <= x2 && py >= y1 && py <= y2 ); }