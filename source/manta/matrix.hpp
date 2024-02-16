#pragma once

#include <manta/vector.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Matrix
{
	float data[16];
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern void matrix_build_zeros( Matrix &mat );

static inline Matrix matrix_build_zeros()
{
	Matrix mat;
	matrix_build_zeros( mat );
	return mat;
}


extern void matrix_build_identity( Matrix &mat );

static inline Matrix matrix_build_identity()
{
	Matrix mat;
	matrix_build_identity( mat );
	return mat;
}


extern void matrix_build_scaling( Matrix &mat, const float xscale, const float yscale, const float zscale );

inline Matrix matrix_build_scaling( const float xscale, const float yscale, const float zscale )
{
	Matrix mat;
	matrix_build_scaling( mat, xscale, yscale, zscale );
	return mat;
}


extern void matrix_build_translation( Matrix &mat, const float xtranslation, const float ytranslation, const float ztranslation );

inline Matrix matrix_build_translation( const float xtranslation, const float ytranslation, const float ztranslation )
{
	Matrix mat;
	matrix_build_translation( mat, xtranslation, ytranslation, ztranslation );
	return mat;
}


extern void matrix_build_rotation_x( Matrix &mat, float angle );

inline Matrix matrix_build_rotation_x( float angle )
{
	Matrix mat;
	matrix_build_rotation_x( mat, angle );
	return mat;
}


extern void matrix_build_rotation_y( Matrix &mat, float angle );

inline Matrix matrix_build_rotation_y( float angle )
{
	Matrix mat;
	matrix_build_rotation_y( mat, angle );
	return mat;
}


extern void matrix_build_rotation_z( Matrix &mat, float angle );

inline Matrix matrix_build_rotation_z( float angle )
{
	Matrix mat;
	matrix_build_rotation_z( mat, angle );
	return mat;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern void matrix_transpose( const Matrix mat, Matrix &result );

inline Matrix matrix_transpose( const Matrix &mat )
{
	Matrix result;
	matrix_transpose( mat, result );
	return result;
}


extern void matrix_multiply( const Matrix mat1, const Matrix &mat2, Matrix &result );

inline Matrix matrix_multiply( const Matrix &mat1, const Matrix &mat2 )
{
	Matrix result;
	matrix_multiply( mat1, mat2, result );
	return result;
}


extern bool matrix_inverse( const Matrix &mat, Matrix &result );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern void matrix_add( const Matrix mat1, const Matrix &mat2, Matrix &result );

inline Matrix matrix_add( const Matrix &mat1, const Matrix &mat2 )
{
	Matrix result;
	matrix_add( mat1, mat2, result );
	return result;
}


extern void matrix_subtract( const Matrix mat1, const Matrix &mat2, Matrix &result );

inline Matrix matrix_subtract( const Matrix &mat1, const Matrix &mat2 )
{
	Matrix result;
	matrix_subtract( mat1, mat2, result );
	return result;
}


extern void matrix_multiply_scalar( Matrix &mat, const float scalar );

inline Matrix matrix_multiply_scalar( const Matrix &mat, const float scalar )
{
	Matrix result = mat;
	matrix_multiply_scalar( result, scalar );
	return result;
}


extern void matrix_multiply_vec2f( const Matrix &mat, const vec2f vec, vec2f &result );

inline vec2f matrix_multiply_vec2f( const Matrix &mat, const vec2f &vec )
{
	vec2f result;
	matrix_multiply_vec2f( mat, vec, result );
	return result;
}


extern void matrix_build_mvp_orthographic( Matrix &mat, float left, float right, float bottom, float top, float znear, float zfar );

inline Matrix matrix_build_mvp_orthographic( float left, float right, float bottom, float top, float znear, float zfar )
{
	Matrix mat;
	matrix_build_mvp_orthographic( mat, left, right, bottom, top, znear, zfar );
	return mat;
}


extern void matrix_build_transform_view( Matrix &mat, const float x, float const y, float const zoom );

inline Matrix matrix_build_transform_view( const float x, float const y, float const zoom )
{
	Matrix result;
	matrix_build_transform_view( result, x, y, zoom );
	return result;
}


extern bool matrix_equal( const Matrix &mat1, const Matrix &mat2 );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////