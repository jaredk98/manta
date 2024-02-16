#include <manta/vector.hpp>
#include <manta/math.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

vec2i::vec2i( const int _x, const int _y )
{
	x = _x;
	y = _y;
}

vec2i::vec2i( const vec2i &v )
{
	x = v.x;
	y = v.y;
}

vec2i::vec2i( const vec2f &v )
{
	x = static_cast<int>( v.x );
	y = static_cast<int>( v.y );
}

vec2i::vec2i( const vec2d &v )
{
	x = static_cast<int>( v.x );
	y = static_cast<int>( v.y );
}

vec2i & vec2i::set( const int _x, const int _y )
{
	x = _x;
	y = _y;
	return *this;
}

vec2i & vec2i::set( const vec2i &v )
{
	x = v.x;
	y = v.y;
	return *this;
}

vec2i & vec2i::operator=( vec2f &v )       { return set( static_cast<int>( v.x ), static_cast<int>( v.y ) ); }
vec2i & vec2i::operator=( const vec2f &v ) { return set( static_cast<int>( v.x ), static_cast<int>( v.y ) ); }
vec2i & vec2i::operator=( vec2d &v )       { return set( static_cast<int>( v.x ), static_cast<int>( v.y ) ); }
vec2i & vec2i::operator=( const vec2d &v ) { return set( static_cast<int>( v.x ), static_cast<int>( v.y ) ); }

vec2i & vec2i::add( const int _x, const int _y )
{
	x += _x;
	y += _y;
	return *this;
}

vec2i & vec2i::add( const vec2i &v )
{
	x += v.x;
	y += v.y;
	return *this;
}

vec2i & vec2i::operator+( vec2f &v )       { return add( static_cast<int>( v.x ), static_cast<int>( v.y ) ); }
vec2i & vec2i::operator+( const vec2f &v ) { return add( static_cast<int>( v.x ), static_cast<int>( v.y ) ); }
vec2i & vec2i::operator+( vec2d &v )       { return add( static_cast<int>( v.x ), static_cast<int>( v.y ) ); }
vec2i & vec2i::operator+( const vec2d &v ) { return add( static_cast<int>( v.x ), static_cast<int>( v.y ) ); }

vec2i & vec2i::sub( const int _x, const int _y )
{
	x -= _x;
	y -= _y;
	return *this;
}

vec2i & vec2i::sub( const vec2i &v )
{
	x -= v.x;
	y -= v.y;
	return *this;
}

vec2i & vec2i::operator-( vec2f &v )       { return sub( static_cast<int>( v.x ), static_cast<int>( v.y ) ); }
vec2i & vec2i::operator-( const vec2f &v ) { return sub( static_cast<int>( v.x ), static_cast<int>( v.y ) ); }
vec2i & vec2i::operator-( vec2d &v )       { return sub( static_cast<int>( v.x ), static_cast<int>( v.y ) ); }
vec2i & vec2i::operator-( const vec2d &v ) { return sub( static_cast<int>( v.x ), static_cast<int>( v.y ) ); }

vec2i & vec2i::mul( const int s )
{
	x *= s;
	y *= s;
	return *this;
}

vec2i & vec2i::mul( const float s )
{
	x = static_cast<int>( x * s );
	y = static_cast<int>( y * s );
	return *this;
}

vec2i & vec2i::mul( const int _x, const int _y )
{
	x *= _x;
	y *= _y;
	return *this;
}

vec2i & vec2i::mul( const float _x, const float _y )
{
	x = static_cast<int>( x * _x );
	y = static_cast<int>( y * _y );
	return *this;
}

vec2i & vec2i::mul( const vec2i &v )
{
	x *= v.x;
	y *= v.y;
	return *this;
}

vec2i & vec2i::operator*( const float s )  { return mul( s ); }
vec2i & vec2i::operator*( const double s ) { return mul( static_cast<float>( s ) ); }

vec2i & vec2i::operator*( vec2f &v )       { return mul( v.x, v.y ); }
vec2i & vec2i::operator*( const vec2f &v ) { return mul( v.x, v.y ); }
vec2i & vec2i::operator*( vec2d &v )       { return mul( static_cast<float>( v.x ), static_cast<float>( v.y ) ); }
vec2i & vec2i::operator*( const vec2d &v ) { return mul( static_cast<float>( v.x ), static_cast<float>( v.y ) ); }

vec2i & vec2i::div( const int s )
{
	x /= s;
	y /= s;
	return *this;
}

vec2i & vec2i::div( const float s )
{
	x = static_cast<int>( x / s );
	y = static_cast<int>( y / s );
	return *this;
}

vec2i & vec2i::div( const int _x, const int _y )
{
	x /= x;
	y /= y;
	return *this;
}

vec2i & vec2i::div( const float _x, const float _y )
{
	x = static_cast<int>( x / _x );
	y = static_cast<int>( y / _y );
	return *this;
}

vec2i & vec2i::div( const vec2i &v )
{
	x /= v.x;
	y /= v.y;
	return *this;
}

vec2i & vec2i::operator/( const float s )  { return div( s ); }
vec2i & vec2i::operator/( const double s ) { return div( static_cast<float>( s ) ); }

vec2i & vec2i::operator/( vec2f &v )       { return div( v.x, v.y ); }
vec2i & vec2i::operator/( const vec2f &v ) { return div( v.x, v.y ); }
vec2i & vec2i::operator/( vec2d &v )       { return div( static_cast<float>( v.x ), static_cast<float>( v.y ) ); }
vec2i & vec2i::operator/( const vec2d &v ) { return div( static_cast<float>( v.x ), static_cast<float>( v.y ) ); }

vec2i & vec2i::normalize()
{
	const float len = sqrtf( static_cast<float>( x * x + y * y ) );
	x = static_cast<int>( x / len );
	y = static_cast<int>( y / len );
	return *this;
}

vec2i & vec2i::rotate( const float angle )
{
	const float s = sinf( -angle * ( 3.141592f / 180.0f ) );
	const float c = cosf( -angle * ( 3.141592f / 180.0f ) );
	const float tX = x * c - y * s;
	const float tY = x * s + y * c;
	x = static_cast<int>( tX );
	y = static_cast<int>( tY );
	return *this;
}

vec2i & vec2i::rotate_origin( const int _x, const int _y, const float angle )
{
	const float s = sinf( -angle * ( 3.141592f / 180.0f ) );
	const float c = cosf( -angle * ( 3.141592f / 180.0f ) );
	x -= _x;
	y -= _y;
	const float tX = x * c - y * s;
	const float tY = x * s + y * c;
	x = static_cast<int>( tX ) + _x;
	y = static_cast<int>( tY ) + _y;
	return *this;
}

int vec2i::length() { return static_cast<int>( sqrtf( static_cast<float>( x * x + y * y ) ) ); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

vec2f::vec2f( const float _x, const float _y )
{
	x = _x;
	y = _y;
}

vec2f::vec2f( const vec2f &v )
{
	x = v.x;
	y = v.y;
}

vec2f::vec2f( const vec2i &v )
{
	x = static_cast<float>( v.x );
	y = static_cast<float>( v.y );
}

vec2f::vec2f( const vec2d &v )
{
	x = static_cast<float>( v.x );
	y = static_cast<float>( v.y );
}

vec2f & vec2f::set( const float _x, const float _y )
{
	x = _x;
	y = _y;
	return *this;
}

vec2f & vec2f::set( const vec2f &v )
{
	x = v.x;
	y = v.y;
	return *this;
}

vec2f & vec2f::operator=( vec2i &v )       { return set( static_cast<float>( v.x ), static_cast<float>( v.y ) ); }
vec2f & vec2f::operator=( const vec2i &v ) { return set( static_cast<float>( v.x ), static_cast<float>( v.y ) ); }
vec2f & vec2f::operator=( vec2d &v )       { return set( static_cast<float>( v.x ), static_cast<float>( v.y ) ); }
vec2f & vec2f::operator=( const vec2d &v ) { return set( static_cast<float>( v.x ), static_cast<float>( v.y ) ); }

vec2f & vec2f::add( const float _x, const float _y )
{
	x += _x;
	y += _y;
	return *this;
}

vec2f & vec2f::add( const vec2f &v )
{
	this->x += v.x;
	this->y += v.y;
	return *this;
}

vec2f & vec2f::operator+( vec2i &v )       { return add( static_cast<float>( v.x ), static_cast<float>( v.y ) ); }
vec2f & vec2f::operator+( const vec2i &v ) { return add( static_cast<float>( v.x ), static_cast<float>( v.y ) ); }
vec2f & vec2f::operator+( vec2d &v )       { return add( static_cast<float>( v.x ), static_cast<float>( v.y ) ); }
vec2f & vec2f::operator+( const vec2d &v ) { return add( static_cast<float>( v.x ), static_cast<float>( v.y ) ); }

vec2f & vec2f::sub( const float _x, const float _y )
{
	x -= _x;
	y -= _y;
	return *this;
}

vec2f & vec2f::sub( const vec2f &v )
{
	x -= v.x;
	y -= v.y;
	return *this;
}

vec2f & vec2f::operator-( vec2i &v )       { return sub( static_cast<float>( v.x ), static_cast<float>( v.y ) ); }
vec2f & vec2f::operator-( const vec2i &v ) { return sub( static_cast<float>( v.x ), static_cast<float>( v.y ) ); }
vec2f & vec2f::operator-( vec2d &v )       { return sub( static_cast<float>( v.x ), static_cast<float>( v.y ) ); }
vec2f & vec2f::operator-( const vec2d &v ) { return sub( static_cast<float>( v.x ), static_cast<float>( v.y ) ); }

vec2f & vec2f::mul( const float s )
{
	x *= s;
	y *= s;
	return *this;
}

vec2f & vec2f::mul( const float _x, const float _y )
{
	x *= _x;
	y *= _y;
	return *this;
}

vec2f & vec2f::mul( const vec2f &v )
{
	x *= v.x;
	y *= v.y;
	return *this;
}

vec2f & vec2f::operator*( const int s )      { return mul( static_cast<float>( s ) ); }
vec2f & vec2f::operator*( const double s )   { return mul( static_cast<float>( s ) ); }

vec2f & vec2f::operator*( vec2i &v )       { return mul( static_cast<float>( v.x ), static_cast<float>( v.y ) ); }
vec2f & vec2f::operator*( const vec2i &v ) { return mul( static_cast<float>( v.x ), static_cast<float>( v.y ) ); }
vec2f & vec2f::operator*( vec2d &v )       { return mul( static_cast<float>( v.x ), static_cast<float>( v.y ) ); }
vec2f & vec2f::operator*( const vec2d &v ) { return mul( static_cast<float>( v.x ), static_cast<float>( v.y ) ); }

vec2f & vec2f::div( const float s )
{
	x /= s;
	y /= s;
	return *this;
}

vec2f & vec2f::div( const float _x, const float _y )
{
	x /= _x;
	y /= _y;
	return *this;
}

vec2f & vec2f::div( const vec2f &v )
{
	x /= v.x;
	y /= v.y;
	return *this;
}

vec2f & vec2f::operator/( const int s )      { return div( static_cast<float>( s ) ); }
vec2f & vec2f::operator/( const double s )   { return div( static_cast<float>( s ) ); }

vec2f & vec2f::operator/( vec2i &v )       { return div( static_cast<float>( v.x ), static_cast<float>( v.y ) ); }
vec2f & vec2f::operator/( const vec2i &v ) { return div( static_cast<float>( v.x ), static_cast<float>( v.y ) ); }
vec2f & vec2f::operator/( vec2d &v )       { return div( static_cast<float>( v.x ), static_cast<float>( v.y ) ); }
vec2f & vec2f::operator/( const vec2d &v ) { return div( static_cast<float>( v.x ), static_cast<float>( v.y ) ); }

vec2f & vec2f::normalize()
{
	const float len = sqrtf( x * x + y * y );
	x /= len;
	y /= len;
	return *this;
}

vec2f & vec2f::rotate( const float angle )
{
	const float s = sinf( -angle * ( 3.141592f / 180.0f ) );
	const float c = cosf( -angle * ( 3.141592f / 180.0f ) );
	const float tX = x * c - y * s;
	const float tY = x * s + y * c;
	x = tX;
	y = tY;
	return *this;
}

vec2f & vec2f::rotate_origin( const float _x, const float _y, const float angle )
{
	const float s = sinf( -angle * ( 3.141592f / 180.0f ) );
	const float c = cosf( -angle * ( 3.141592f / 180.0f ) );
	x -= _x;
	y -= _y;
	const float tX = x * c - y * s;
	const float tY = x * s + y * c;
	x = tX + _x;
	y = tY + _y;
	return *this;
}

float vec2f::length() { return sqrtf( x * x + y * y ); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

vec2d::vec2d( const double _x, const double _y )
{
	x = _x;
	y = _y;
}

vec2d::vec2d( const vec2d &v )
{
	x = v.x;
	y = v.y;
}

vec2d::vec2d( const vec2i &v )
{
	x = static_cast<double>( v.x );
	y = static_cast<double>( v.y );
}

vec2d::vec2d( const vec2f &v )
{
	x = static_cast<double>( v.x );
	y = static_cast<double>( v.y );
}

vec2d & vec2d::set( const double _x, const double _y )
{
	x = _x;
	y = _y;
	return *this;
}

vec2d & vec2d::set( const vec2d &v )
{
	x = v.x;
	y = v.y;
	return *this;
}

vec2d & vec2d::operator=( vec2i &v )       { return set( static_cast<double>( v.x ), static_cast<double>( v.y ) ); }
vec2d & vec2d::operator=( const vec2i &v ) { return set( static_cast<double>( v.x ), static_cast<double>( v.y ) ); }
vec2d & vec2d::operator=( vec2f &v )       { return set( static_cast<double>( v.x ), static_cast<double>( v.y ) ); }
vec2d & vec2d::operator=( const vec2f &v ) { return set( static_cast<double>( v.x ), static_cast<double>( v.y ) ); }

vec2d & vec2d::add( const double _x, const double _y )
{
	x += _x;
	y += _y;
	return *this;
}

vec2d & vec2d::add( const vec2d &v )
{
	x += v.x;
	y += v.y;
	return *this;
}

vec2d & vec2d::operator+( vec2i &v )       { return add( static_cast<double>( v.x ), static_cast<double>( v.y ) ); }
vec2d & vec2d::operator+( const vec2i &v ) { return add( static_cast<double>( v.x ), static_cast<double>( v.y ) ); }
vec2d & vec2d::operator+( vec2f &v )       { return add( static_cast<double>( v.x ), static_cast<double>( v.y ) ); }
vec2d & vec2d::operator+( const vec2f &v ) { return add( static_cast<double>( v.x ), static_cast<double>( v.y ) ); }

vec2d & vec2d::sub( const double _x, const double _y )
{
	x -= _x;
	y -= _y;
	return *this;
}

vec2d & vec2d::sub( const vec2d &v )
{
	x -= v.x;
	y -= v.y;
	return *this;
}

vec2d & vec2d::operator-( vec2i &v )       { return sub( static_cast<double>( v.x ), static_cast<double>( v.y ) ); }
vec2d & vec2d::operator-( const vec2i &v ) { return sub( static_cast<double>( v.x ), static_cast<double>( v.y ) ); }
vec2d & vec2d::operator-( vec2f &v )       { return sub( static_cast<double>( v.x ), static_cast<double>( v.y ) ); }
vec2d & vec2d::operator-( const vec2f &v ) { return sub( static_cast<double>( v.x ), static_cast<double>( v.y ) ); }

vec2d & vec2d::mul( const double s )
{
	x *= s;
	y *= s;
	return *this;
}

vec2d & vec2d::mul( const double _x, const double _y )
{
	x *= _x;
	y *= _y;
	return *this;
}

vec2d & vec2d::mul( const vec2d &v )
{
	x *= v.x;
	y *= v.y;
	return *this;
}

vec2d & vec2d::operator*( const int s )      { return mul( static_cast<double>( s ) ); }
vec2d & vec2d::operator*( const float s )    { return mul( static_cast<double>( s ) ); }

vec2d & vec2d::operator*( vec2i &v )       { return mul( static_cast<double>( v.x ), static_cast<double>( v.y ) ); }
vec2d & vec2d::operator*( const vec2i &v ) { return mul( static_cast<double>( v.x ), static_cast<double>( v.y ) ); }
vec2d & vec2d::operator*( vec2f &v )       { return mul( static_cast<double>( v.x ), static_cast<double>( v.y ) ); }
vec2d & vec2d::operator*( const vec2f &v ) { return mul( static_cast<double>( v.x ), static_cast<double>( v.y ) ); }

vec2d & vec2d::div( const double s )
{
	x /= s;
	y /= s;
	return *this;
}

vec2d & vec2d::div( const double _x, const double _y )
{
	x /= _x;
	y /= _y;
	return *this;
}

vec2d & vec2d::div( const vec2d &v )
{
	x /= v.x;
	y /= v.y;
	return *this;
}

vec2d & vec2d::operator/( const int s )      { return div( static_cast<double>( s ) ); }
vec2d & vec2d::operator/( const float s )    { return div( static_cast<double>( s ) ); }

vec2d & vec2d::operator/( vec2i &v )       { return div( static_cast<double>( v.x ), static_cast<double>( v.y ) ); }
vec2d & vec2d::operator/( const vec2i &v ) { return div( static_cast<double>( v.x ), static_cast<double>( v.y ) ); }
vec2d & vec2d::operator/( vec2f &v )       { return div( static_cast<double>( v.x ), static_cast<double>( v.y ) ); }
vec2d & vec2d::operator/( const vec2f &v ) { return div( static_cast<double>( v.x ), static_cast<double>( v.y ) ); }

vec2d & vec2d::normalize()
{
	const double len = sqrt( x * x + y * y );
	x /= len;
	y /= len;
	return *this;
}

vec2d & vec2d::rotate( const float angle )
{
	const double s = sin( -angle * ( 3.141592f / 180.0f ) );
	const double c = cos( -angle * ( 3.141592f / 180.0f ) );
	const double tX = x * c - y * s;
	const double tY = x * s + y * c;
	x = tX;
	y = tY;
	return *this;
}

vec2d & vec2d::rotate_origin( const double _x, const double _y, const float angle )
{
	const double s = sin( -angle * ( 3.141592f / 180.0f ) );
	const double c = cos( -angle * ( 3.141592f / 180.0f ) );
	x -= _x;
	y -= _y;
	const double tX = x * c - y * s;
	const double tY = x * s + y * c;
	x = tX + _x;
	y = tY + _y;
	return *this;
}

double vec2d::length() { return sqrt( x * x + y * y ); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

rayi::rayi( const int posX, const int posY, const int dirX, const int dirY )
{
	pos.set( posX, posY );
	dir.set( dirX, dirY );
}

rayi::rayi( const rayi &v )
{
	pos.set( v.pos );
	dir.set( v.dir );
}

rayi::rayi( const rayf &v )
{
	pos.set( v.pos );
	dir.set( v.dir );
}

rayi::rayi( const rayd &v )
{
	pos.set( v.pos );
	dir.set( v.dir );
}

rayi & rayi::set( const int posX, const int posY, const int dirX, const int dirY )
{
	pos.set( posX, posY );
	dir.set( dirX, dirY );
	return *this;
}

rayi & rayi::set( const rayi &v )
{
	pos.set( v.pos );
	dir.set( v.dir );
	return *this;
}

rayi & rayi::set( const rayf &v )
{
	pos.set( v.pos );
	dir.set( v.dir );
	return *this;
}

rayi & rayi::set( const rayd &v )
{
	pos.set( v.pos );
	dir.set( v.dir );
	return *this;
}

rayi & rayi::normalize()
{
	dir.normalize();
	return *this;
}

rayi & rayi::rotate( const float angle )
{
	dir.rotate( angle );
	return *this;
}

rayi & rayi::rotate_origin( const int x, const int y, const float angle )
{
	pos.rotate_origin( x, y, angle );
	return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

rayf::rayf( const float posX, const float posY, const float dirX, const float dirY )
{
	pos.set( posX, posY );
	dir.set( dirX, dirY );
}

rayf::rayf( const rayi &v )
{
	pos.set( v.pos );
	dir.set( v.dir );
}

rayf::rayf( const rayf &v )
{
	pos.set( v.pos );
	dir.set( v.dir );
}

rayf::rayf( const rayd &v )
{
	pos.set( v.pos );
	dir.set( v.dir );
}

rayf & rayf::set( const float posX, const float posY, const float dirX, const float dirY )
{
	pos.set( posX, posY );
	dir.set( dirX, dirY );
	return *this;
}

rayf & rayf::set( const rayi &v )
{
	pos.set( v.pos );
	dir.set( v.dir );
	return *this;
}

rayf & rayf::set( const rayf &v )
{
	pos.set( v.pos );
	dir.set( v.dir );
	return *this;
}

rayf & rayf::set( const rayd &v )
{
	pos.set( v.pos );
	dir.set( v.dir );
	return *this;
}

rayf & rayf::normalize()
{
	dir.normalize();
	return *this;
}

rayf & rayf::rotate( const float angle )
{
	dir.rotate( angle );
	return *this;
}

rayf & rayf::rotate_origin( const float x, const float y, const float angle )
{
	pos.rotate_origin( x, y, angle );
	return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

rayd::rayd( const double posX, const double posY, const double dirX, const double dirY )
{
	pos.set( posX, posY );
	dir.set( dirX, dirY );
}

rayd::rayd( const rayi &v )
{
	pos.set( v.pos );
	dir.set( v.dir );
}

rayd::rayd( const rayf &v )
{
	pos.set( v.pos );
	dir.set( v.dir );
}

rayd::rayd( const rayd &v )
{
	pos.set( v.pos );
	dir.set( v.dir );
}

rayd & rayd::set( const double posX, const double posY, const double dirX, const double dirY )
{
	pos.set( posX, posY );
	dir.set( dirX, dirY );
	return *this;
}

rayd & rayd::set( const rayi &v )
{
	pos.set( v.pos );
	dir.set( v.dir );
	return *this;
}

rayd & rayd::set( const rayf &v )
{
	pos.set( v.pos );
	dir.set( v.dir );
	return *this;
}

rayd & rayd::set( const rayd &v )
{
	pos.set( v.pos );
	dir.set( v.dir );
	return *this;
}

rayd & rayd::normalize()
{
	dir.normalize();
	return *this;
}

rayd & rayd::rotate( const float angle )
{
	dir.rotate( angle );
	return *this;
}

rayd & rayd::rotate_origin( const double x, const double y, const float angle )
{
	pos.rotate_origin( x, y, angle );
	return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

recti::recti( const int _x1, const int _y1, const int _x2, const int _y2 )
{
	x1 = _x1;
	y1 = _y1;
	x2 = _x2;
	y2 = _y2;
}

recti::recti( const recti &r )
{
	x1 = r.x1;
	y1 = r.y1;
	x2 = r.x2;
	y2 = r.y2;
}

recti::recti( const rectf &r )
{
	x1 = static_cast<int>( r.x1 );
	y1 = static_cast<int>( r.y1 );
	x2 = static_cast<int>( r.x2 );
	y2 = static_cast<int>( r.y2 );
}

recti::recti( const rectd &r )
{
	x1 = static_cast<int>( r.x1 );
	y1 = static_cast<int>( r.y1 );
	x2 = static_cast<int>( r.x2 );
	y2 = static_cast<int>( r.y2 );
}


recti & recti::set( const int _x1, const int _y1, const int _x2, const int _y2 )
{
	x1 = _x1;
	y1 = _y1;
	x2 = _x2;
	y2 = _y2;
	return *this;
}

recti & recti::set( const recti &r )
{
	x1 = r.x1;
	y1 = r.y1;
	x2 = r.x2;
	y2 = r.y2;
	return *this;
}

recti & recti::operator=( rectf &v )       { return set( static_cast<int>( v.x1 ), static_cast<int>( v.y1 ), static_cast<int>( v.x2 ), static_cast<int>( v.y2 ) ); }
recti & recti::operator=( const rectf &v ) { return set( static_cast<int>( v.x1 ), static_cast<int>( v.y1 ), static_cast<int>( v.x2 ), static_cast<int>( v.y2 ) ); }
recti & recti::operator=( rectd &v )       { return set( static_cast<int>( v.x1 ), static_cast<int>( v.y1 ), static_cast<int>( v.x2 ), static_cast<int>( v.y2 ) ); }
recti & recti::operator=( const rectd &v ) { return set( static_cast<int>( v.x1 ), static_cast<int>( v.y1 ), static_cast<int>( v.x2 ), static_cast<int>( v.y2 ) ); }

bool recti::intersects( const rectf &r )
	{ return ( static_cast<float>( x1 ) <= r.x2 && static_cast<float>( x2 ) >= r.x1 && static_cast<float>( y1 ) <= r.y2 && static_cast<float>( y2 ) >= r.y1 ); }

bool recti::intersects( const rectd &r )
	{ return ( static_cast<double>( x1 ) <= r.x2 && static_cast<double>( x2 ) >= r.x1 && static_cast<double>( y1 ) <= r.y2 && static_cast<double>( y2 ) >= r.y1 ); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

rectf::rectf( const float _x1, const float _y1, const float _x2, const float _y2 )
{
	x1 = _x1;
	y1 = _y1;
	x2 = _x2;
	y2 = _y2;
}

rectf::rectf( const rectf &r )
{
	x1 = r.x1;
	y1 = r.y1;
	x2 = r.x2;
	y2 = r.y2;
}

rectf::rectf( const recti &r )
{
	x1 = static_cast<float>( r.x1 );
	y1 = static_cast<float>( r.y1 );
	x2 = static_cast<float>( r.x2 );
	y2 = static_cast<float>( r.y2 );
}

rectf::rectf( const rectd &r )
{
	x1 = static_cast<float>( r.x1 );
	y1 = static_cast<float>( r.y1 );
	x2 = static_cast<float>( r.x2 );
	y2 = static_cast<float>( r.y2 );
}

rectf & rectf::set( const float _x1, const float _y1, const float _x2, const float _y2 )
{
	x1 = _x1;
	y1 = _y1;
	x2 = _x2;
	y2 = _y2;
	return *this;
}

rectf & rectf::set( const rectf &r )
{
	x1 = r.x1;
	y1 = r.y1;
	x2 = r.x2;
	y2 = r.y2;
	return *this;
}

rectf & rectf::operator=( recti &v )       { return set( static_cast<float>( v.x1 ), static_cast<float>( v.y1 ), static_cast<float>( v.x2 ), static_cast<float>( v.y2 ) ); }
rectf & rectf::operator=( const recti &v ) { return set( static_cast<float>( v.x1 ), static_cast<float>( v.y1 ), static_cast<float>( v.x2 ), static_cast<float>( v.y2 ) ); }
rectf & rectf::operator=( rectd &v )       { return set( static_cast<float>( v.x1 ), static_cast<float>( v.y1 ), static_cast<float>( v.x2 ), static_cast<float>( v.y2 ) ); }
rectf & rectf::operator=( const rectd &v ) { return set( static_cast<float>( v.x1 ), static_cast<float>( v.y1 ), static_cast<float>( v.x2 ), static_cast<float>( v.y2 ) ); }

bool rectf::intersects( const recti &r )
	{ return ( x1 <= static_cast<float>( r.x2 ) && x2 >= static_cast<float>( r.x1 ) && y1 <= static_cast<float>( r.y2 ) && y2 >= static_cast<float>( r.y1 ) ); }

bool rectf::intersects( const rectd &r )
	{ return ( static_cast<double>( x1 ) <= r.x2 && static_cast<double>( x2 ) >= r.x1 && static_cast<double>( y1 ) <= r.y2 && static_cast<double>( y2 ) >= r.y1 ); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

rectd::rectd( const double _x1, const double _y1, const double _x2, const double _y2 )
{
	x1 = _x1;
	y1 = _y1;
	x2 = _x2;
	y2 = _y2;
}

rectd::rectd( const rectd &r )
{
	x1 = r.x1;
	y1 = r.y1;
	x2 = r.x2;
	y2 = r.y2;
}

rectd::rectd( const recti &r )
{
	x1 = static_cast<double>( r.x1 );
	y1 = static_cast<double>( r.y1 );
	x2 = static_cast<double>( r.x2 );
	y2 = static_cast<double>( r.y2 );
}

rectd::rectd( const rectf &r )
{
	x1 = static_cast<double>( r.x1 );
	y1 = static_cast<double>( r.y1 );
	x2 = static_cast<double>( r.x2 );
	y2 = static_cast<double>( r.y2 );
}

rectd & rectd::set( const double _x1, const double _y1, const double _x2, const double _y2 )
{
	x1 = _x1;
	y1 = _y1;
	x2 = _x2;
	y2 = _y2;
	return *this;
}

rectd & rectd::set( const rectd &r )
{
	x1 = r.x1;
	y1 = r.y1;
	x2 = r.x2;
	y2 = r.y2;
	return *this;
}

rectd & rectd::operator=( recti &v )       { return set( static_cast<double>( v.x1 ), static_cast<double>( v.y1 ), static_cast<double>( v.x2 ), static_cast<double>( v.y2 ) ); }
rectd & rectd::operator=( const recti &v ) { return set( static_cast<double>( v.x1 ), static_cast<double>( v.y1 ), static_cast<double>( v.x2 ), static_cast<double>( v.y2 ) ); }
rectd & rectd::operator=( rectf &v )       { return set( static_cast<double>( v.x1 ), static_cast<double>( v.y1 ), static_cast<double>( v.x2 ), static_cast<double>( v.y2 ) ); }
rectd & rectd::operator=( const rectf &v ) { return set( static_cast<double>( v.x1 ), static_cast<double>( v.y1 ), static_cast<double>( v.x2 ), static_cast<double>( v.y2 ) ); }

bool rectd::intersects( const recti &r )
	{ return ( x1 <= static_cast<double>( r.x2 ) && x2 >= static_cast<double>( r.x1 ) && y1 <= static_cast<double>( r.y2 ) && y2 >= static_cast<double>( r.y1 ) ); }

bool rectd::intersects( const rectf &r )
	{ return ( static_cast<double>( x1 ) <= r.x2 && static_cast<double>( x2 ) >= r.x1 && static_cast<double>( y1 ) <= r.y2 && static_cast<double>( y2 ) >= r.y1 ); }
