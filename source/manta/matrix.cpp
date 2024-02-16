#include <manta/matrix.hpp>

#include <manta/math.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void matrix_build_zeros( Matrix &mat )
{
	for( int i = 0; i < 16; i++ )
	{
		mat.data[i] = 0.0f;
	}
}


void matrix_build_identity( Matrix &mat )
{
	matrix_build_zeros( mat );
	mat.data[0]  = 1.0f;
	mat.data[5]  = 1.0f;
	mat.data[10] = 1.0f;
	mat.data[15] = 1.0f;
}


void matrix_build_scaling( Matrix &mat, const float xscale, const float yscale, const float zscale )
{
	matrix_build_zeros( mat );
	mat.data[0]  = xscale;
	mat.data[5]  = yscale;
	mat.data[10] = zscale;
}


void matrix_build_translation( Matrix &mat, const float xtranslation, const float ytranslation, const float ztranslation )
{
	matrix_build_identity(mat);
	mat.data[12] = xtranslation;
	mat.data[13] = ytranslation;
	mat.data[14] = ztranslation;
}


void matrix_build_rotation_x( Matrix &mat, float angle_degrees )
{
	matrix_build_zeros( mat );
	angle_degrees = degtorad( angle_degrees );
	float s = static_cast<float>( sinf( angle_degrees ) );
	float c = static_cast<float>( cosf( angle_degrees ) );

	mat.data[5]  = c;
	mat.data[6]  = -s;
	mat.data[9]  = s;
	mat.data[10] = c;
}


void matrix_build_rotation_y( Matrix &mat, float angle_degrees )
{
	matrix_build_zeros();
	angle_degrees = degtorad( angle_degrees );
	float s = static_cast<float>( sinf( angle_degrees ) );
	float c = static_cast<float>( cosf( angle_degrees ) );

	mat.data[0]  =  c;
	mat.data[2]  =  s;
	mat.data[8]  = -s;
	mat.data[10] =  c;
}


void matrix_build_rotation_z( Matrix &mat, float angle_degrees )
{
	matrix_build_zeros( mat );
	angle_degrees = degtorad( angle_degrees );
	float s = static_cast<float>( sinf( angle_degrees ) );
	float c = static_cast<float>( cosf( angle_degrees ) );

	mat.data[0] =  c;
	mat.data[1] = -s;
	mat.data[4] =  s;
	mat.data[5] =  c;
}


void matrix_transpose( const Matrix mat, Matrix &result )
{
	result.data[0]  = mat.data[0];
	result.data[1]  = mat.data[4];
	result.data[2]  = mat.data[8];
	result.data[3]  = mat.data[12];
	result.data[4]  = mat.data[1];
	result.data[5]  = mat.data[5];
	result.data[6]  = mat.data[9];
	result.data[7]  = mat.data[13];
	result.data[8]  = mat.data[2];
	result.data[9]  = mat.data[6];
	result.data[10] = mat.data[10];
	result.data[11] = mat.data[14];
	result.data[12] = mat.data[3];
	result.data[13] = mat.data[7];
	result.data[14] = mat.data[11];
	result.data[15] = mat.data[15];
}


void matrix_add( const Matrix mat1, const Matrix &mat2, Matrix &result )
{
	for(int i = 0; i < 16; i++) result.data[i] = mat1.data[i] + mat2.data[i];
}


void matrix_subtract( const Matrix mat1, const Matrix &mat2, Matrix &result )
{
	for (int i = 0; i < 16; i++) result.data[i] = mat1.data[i] - mat2.data[i];
}


void matrix_multiply_scalar( Matrix &mat, const float scalar )
{
	for (int i = 0; i < 16; i++) mat.data[i] *= scalar;
}


void matrix_multiply(const Matrix mat1, const Matrix &mat2, Matrix &result)
{
	result.data[0]  = mat1.data[0] * mat2.data[0]  + mat1.data[4] * mat2.data[1]  + mat1.data[8]  * mat2.data[2]  + mat1.data[12] * mat2.data[3];
	result.data[4]  = mat1.data[0] * mat2.data[4]  + mat1.data[4] * mat2.data[5]  + mat1.data[8]  * mat2.data[6]  + mat1.data[12] * mat2.data[7];
	result.data[8]  = mat1.data[0] * mat2.data[8]  + mat1.data[4] * mat2.data[9]  + mat1.data[8]  * mat2.data[10] + mat1.data[12] * mat2.data[11];
	result.data[12] = mat1.data[0] * mat2.data[12] + mat1.data[4] * mat2.data[13] + mat1.data[8]  * mat2.data[14] + mat1.data[12] * mat2.data[15];
	result.data[1]  = mat1.data[1] * mat2.data[0]  + mat1.data[5] * mat2.data[1]  + mat1.data[9]  * mat2.data[2]  + mat1.data[13] * mat2.data[3];
	result.data[5]  = mat1.data[1] * mat2.data[4]  + mat1.data[5] * mat2.data[5]  + mat1.data[9]  * mat2.data[6]  + mat1.data[13] * mat2.data[7];
	result.data[9]  = mat1.data[1] * mat2.data[8]  + mat1.data[5] * mat2.data[9]  + mat1.data[9]  * mat2.data[10] + mat1.data[13] * mat2.data[11];
	result.data[13] = mat1.data[1] * mat2.data[12] + mat1.data[5] * mat2.data[13] + mat1.data[9]  * mat2.data[14] + mat1.data[13] * mat2.data[15];
	result.data[2]  = mat1.data[2] * mat2.data[0]  + mat1.data[6] * mat2.data[1]  + mat1.data[10] * mat2.data[2]  + mat1.data[14] * mat2.data[3];
	result.data[6]  = mat1.data[2] * mat2.data[4]  + mat1.data[6] * mat2.data[5]  + mat1.data[10] * mat2.data[6]  + mat1.data[14] * mat2.data[7];
	result.data[10] = mat1.data[2] * mat2.data[8]  + mat1.data[6] * mat2.data[9]  + mat1.data[10] * mat2.data[10] + mat1.data[14] * mat2.data[11];
	result.data[14] = mat1.data[2] * mat2.data[12] + mat1.data[6] * mat2.data[13] + mat1.data[10] * mat2.data[14] + mat1.data[14] * mat2.data[15];
	result.data[3]  = mat1.data[3] * mat2.data[0]  + mat1.data[7] * mat2.data[1]  + mat1.data[11] * mat2.data[2]  + mat1.data[15] * mat2.data[3];
	result.data[7]  = mat1.data[3] * mat2.data[4]  + mat1.data[7] * mat2.data[5]  + mat1.data[11] * mat2.data[6]  + mat1.data[15] * mat2.data[7];
	result.data[11] = mat1.data[3] * mat2.data[8]  + mat1.data[7] * mat2.data[9]  + mat1.data[11] * mat2.data[10] + mat1.data[15] * mat2.data[11];
	result.data[15] = mat1.data[3] * mat2.data[12] + mat1.data[7] * mat2.data[13] + mat1.data[11] * mat2.data[14] + mat1.data[15] * mat2.data[15];
};


bool matrix_inverse( const Matrix &mat, Matrix &result )
{
	Matrix inv;
	float det;

	inv.data[0] = mat.data[5]  * mat.data[10] * mat.data[15] -
				  mat.data[5]  * mat.data[11] * mat.data[14] -
				  mat.data[9]  * mat.data[6]  * mat.data[15] +
				  mat.data[9]  * mat.data[7]  * mat.data[14] +
				  mat.data[13] * mat.data[6]  * mat.data[11] -
				  mat.data[13] * mat.data[7]  * mat.data[10];

	inv.data[4] = -mat.data[4]  * mat.data[10] * mat.data[15] +
				   mat.data[4]  * mat.data[11] * mat.data[14] +
				   mat.data[8]  * mat.data[6]  * mat.data[15] -
				   mat.data[8]  * mat.data[7]  * mat.data[14] -
				   mat.data[12] * mat.data[6]  * mat.data[11] +
				   mat.data[12] * mat.data[7]  * mat.data[10];

	inv.data[8] = mat.data[4]  * mat.data[9]  * mat.data[15] -
				  mat.data[4]  * mat.data[11] * mat.data[13] -
				  mat.data[8]  * mat.data[5]  * mat.data[15] +
				  mat.data[8]  * mat.data[7]  * mat.data[13] +
				  mat.data[12] * mat.data[5]  * mat.data[11] -
				  mat.data[12] * mat.data[7]  * mat.data[9];

	inv.data[12] = -mat.data[4]  * mat.data[9]  * mat.data[14] +
				    mat.data[4]  * mat.data[10] * mat.data[13] +
				    mat.data[8]  * mat.data[5]  * mat.data[14] -
				    mat.data[8]  * mat.data[6]  * mat.data[13] -
				    mat.data[12] * mat.data[5]  * mat.data[10] +
				    mat.data[12] * mat.data[6]  * mat.data[9];

	inv.data[1] = -mat.data[1]  * mat.data[10] * mat.data[15] +
				   mat.data[1]  * mat.data[11] * mat.data[14] +
				   mat.data[9]  * mat.data[2]  * mat.data[15] -
				   mat.data[9]  * mat.data[3]  * mat.data[14] -
				   mat.data[13] * mat.data[2]  * mat.data[11] +
				   mat.data[13] * mat.data[3]  * mat.data[10];

	inv.data[5] = mat.data[0]  * mat.data[10] * mat.data[15] -
				  mat.data[0]  * mat.data[11] * mat.data[14] -
				  mat.data[8]  * mat.data[2]  * mat.data[15] +
				  mat.data[8]  * mat.data[3]  * mat.data[14] +
				  mat.data[12] * mat.data[2]  * mat.data[11] -
				  mat.data[12] * mat.data[3]  * mat.data[10];

	inv.data[9] = -mat.data[0]  * mat.data[9]  * mat.data[15] +
				   mat.data[0]  * mat.data[11] * mat.data[13] +
				   mat.data[8]  * mat.data[1]  * mat.data[15] -
				   mat.data[8]  * mat.data[3]  * mat.data[13] -
				   mat.data[12] * mat.data[1]  * mat.data[11] +
				   mat.data[12] * mat.data[3]  * mat.data[9];

	inv.data[13] = mat.data[0]  * mat.data[9]  * mat.data[14] -
				   mat.data[0]  * mat.data[10] * mat.data[13] -
				   mat.data[8]  * mat.data[1]  * mat.data[14] +
				   mat.data[8]  * mat.data[2]  * mat.data[13] +
				   mat.data[12] * mat.data[1]  * mat.data[10] -
				   mat.data[12] * mat.data[2]  * mat.data[9];

	inv.data[2] = mat.data[1]  * mat.data[6] * mat.data[15] -
				  mat.data[1]  * mat.data[7] * mat.data[14] -
				  mat.data[5]  * mat.data[2] * mat.data[15] +
				  mat.data[5]  * mat.data[3] * mat.data[14] +
				  mat.data[13] * mat.data[2] * mat.data[7] -
				  mat.data[13] * mat.data[3] * mat.data[6];

	inv.data[6] = -mat.data[0]  * mat.data[6] * mat.data[15] +
				   mat.data[0]  * mat.data[7] * mat.data[14] +
				   mat.data[4]  * mat.data[2] * mat.data[15] -
				   mat.data[4]  * mat.data[3] * mat.data[14] -
				   mat.data[12] * mat.data[2] * mat.data[7] +
				   mat.data[12] * mat.data[3] * mat.data[6];

	inv.data[10] = mat.data[0]  * mat.data[5] * mat.data[15] -
				   mat.data[0]  * mat.data[7] * mat.data[13] -
				   mat.data[4]  * mat.data[1] * mat.data[15] +
				   mat.data[4]  * mat.data[3] * mat.data[13] +
				   mat.data[12] * mat.data[1] * mat.data[7] -
				   mat.data[12] * mat.data[3] * mat.data[5];

	inv.data[14] = -mat.data[0]  * mat.data[5] * mat.data[14] +
				    mat.data[0]  * mat.data[6] * mat.data[13] +
				    mat.data[4]  * mat.data[1] * mat.data[14] -
				    mat.data[4]  * mat.data[2] * mat.data[13] -
				    mat.data[12] * mat.data[1] * mat.data[6] +
				    mat.data[12] * mat.data[2] * mat.data[5];

	inv.data[3] = -mat.data[1] * mat.data[6] * mat.data[11] +
				   mat.data[1] * mat.data[7] * mat.data[10] +
				   mat.data[5] * mat.data[2] * mat.data[11] -
				   mat.data[5] * mat.data[3] * mat.data[10] -
				   mat.data[9] * mat.data[2] * mat.data[7] +
				   mat.data[9] * mat.data[3] * mat.data[6];

	inv.data[7] = mat.data[0] * mat.data[6] * mat.data[11] -
				  mat.data[0] * mat.data[7] * mat.data[10] -
				  mat.data[4] * mat.data[2] * mat.data[11] +
				  mat.data[4] * mat.data[3] * mat.data[10] +
				  mat.data[8] * mat.data[2] * mat.data[7] -
				  mat.data[8] * mat.data[3] * mat.data[6];

	inv.data[11] = -mat.data[0] * mat.data[5] * mat.data[11] +
				    mat.data[0] * mat.data[7] * mat.data[9] +
				    mat.data[4] * mat.data[1] * mat.data[11] -
				    mat.data[4] * mat.data[3] * mat.data[9] -
				    mat.data[8] * mat.data[1] * mat.data[7] +
				    mat.data[8] * mat.data[3] * mat.data[5];

	inv.data[15] = mat.data[0] * mat.data[5] * mat.data[10] -
				   mat.data[0] * mat.data[6] * mat.data[9] -
				   mat.data[4] * mat.data[1] * mat.data[10] +
				   mat.data[4] * mat.data[2] * mat.data[9] +
				   mat.data[8] * mat.data[1] * mat.data[6] -
				   mat.data[8] * mat.data[2] * mat.data[5];

	det = mat.data[0] * inv.data[0] + mat.data[1] * inv.data[4] + mat.data[2] * inv.data[8] + mat.data[3] * inv.data[12];
	if( det == 0.0f ) { return false; }

	det = 1.0f / det;
	for( int i = 0; i < 16; i++ ) result.data[i] = inv.data[i] * det;

	return true;
}


void matrix_multiply_vec2f( const Matrix &mat, const vec2f vec, vec2f &result )
{
	result.x = mat.data[0] * vec.x + mat.data[4] * vec.y + mat.data[12];
	result.y = mat.data[1] * vec.x + mat.data[5] * vec.y + mat.data[13];
}


void matrix_build_mvp_orthographic( Matrix &mat, float left, float right, float bottom, float top, float znear, float zfar )
{
	mat.data[0]  = 2.0f / ( right - left );
	mat.data[1]  = 0.0f;
	mat.data[2]  = 0.0f;
	mat.data[3]  = 0.0f;
	mat.data[4]  = 0.0f;
	mat.data[5]  = 2.0f / ( top - bottom );
	mat.data[6]  = 0.0f;
	mat.data[7]  = 0.0f;
	mat.data[8]  = 0.0f;
	mat.data[9]  = 0.0f;
	mat.data[10] = -2.0f / ( zfar - znear );
	mat.data[11] = 0.0f;
	mat.data[12] = -( right + left ) / ( right - left );
	mat.data[13] = -( top + bottom ) / ( top - bottom );
	mat.data[14] = -( zfar + znear ) / ( zfar - znear );

	mat.data[15] = 1.0f;
}


void matrix_build_transform_view( Matrix &mat, const float x, float const y, float const zoom )
{
	mat = matrix_build_identity();
	matrix_multiply( mat, matrix_build_scaling( zoom, zoom, 0.0f ), mat );
	matrix_multiply( mat, matrix_build_translation( -x, -y, 0.0f ), mat );
}


bool matrix_equal( const Matrix &matA, const Matrix &matB )
{
	return (
		matA.data[0]  == matB.data[0]  &&
		matA.data[1]  == matB.data[1]  &&
		matA.data[2]  == matB.data[2]  &&
		matA.data[3]  == matB.data[3]  &&
		matA.data[4]  == matB.data[4]  &&
		matA.data[5]  == matB.data[5]  &&
		matA.data[6]  == matB.data[6]  &&
		matA.data[7]  == matB.data[7]  &&
		matA.data[8]  == matB.data[8]  &&
		matA.data[9]  == matB.data[9]  &&
		matA.data[10] == matB.data[10] &&
		matA.data[11] == matB.data[11] &&
		matA.data[12] == matB.data[12] &&
		matA.data[13] == matB.data[13] &&
		matA.data[14] == matB.data[14] &&
		matA.data[15] == matB.data[15]
	);
}