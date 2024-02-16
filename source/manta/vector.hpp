#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct vec2i;
struct vec2f;
struct vec2d;

struct rayi;
struct rayf;
struct rayd;

struct recti;
struct rectf;
struct rectd;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct vec2i
{
	int x, y;

	vec2i( const int x = 0, const int y = 0 );
	vec2i( const vec2i &v );
	vec2i( const vec2f &v );
	vec2i( const vec2d &v );

	vec2i & set( const int x, const int y );
	vec2i & set( const vec2i &v );

	inline vec2i & operator=(       vec2i &v ) { return set( v ); }
	inline vec2i & operator=( const vec2i &v ) { return set( v ); }
		   vec2i & operator=(       vec2f &v );
		   vec2i & operator=( const vec2f &v );
		   vec2i & operator=(       vec2d &v );
		   vec2i & operator=( const vec2d &v );

	vec2i & add( const int x, const int y );
	vec2i & add( const vec2i &v );

	inline vec2i & operator+(       vec2i &v ) { return add( v ); }
	inline vec2i & operator+( const vec2i &v ) { return add( v ); }
		   vec2i & operator+(       vec2f &v );
		   vec2i & operator+( const vec2f &v );
		   vec2i & operator+(       vec2d &v );
		   vec2i & operator+( const vec2d &v );

	vec2i & sub( const int x, const int y );
	vec2i & sub( const vec2i &v );

	inline vec2i & operator-(       vec2i &v ) { return sub( v ); }
	inline vec2i & operator-( const vec2i &v ) { return sub( v ); }
		   vec2i & operator-(       vec2f &v );
		   vec2i & operator-( const vec2f &v );
		   vec2i & operator-(       vec2d &v );
		   vec2i & operator-( const vec2d &v );

	vec2i & mul( const int s );
	vec2i & mul( const float s );
	vec2i & mul( const int x, const int y );
	vec2i & mul( const float x, const float y );
	vec2i & mul( const vec2i &v );

	inline vec2i & operator*( const int s )    { return mul( s ); }
		   vec2i & operator*( const float s );
		   vec2i & operator*( const double s );

	inline vec2i & operator*(       vec2i &v ) { return mul( v ); }
	inline vec2i & operator*( const vec2i &v ) { return mul( v ); }
		   vec2i & operator*(       vec2f &v );
		   vec2i & operator*( const vec2f &v );
		   vec2i & operator*(       vec2d &v );
		   vec2i & operator*( const vec2d &v );

	vec2i & div( const int s );
	vec2i & div( const float s );
	vec2i & div( const int x, const int y );
	vec2i & div( const float x, const float y );
	vec2i & div( const vec2i &v );

	inline vec2i & operator/( const int s )    { return div( s ); }
		   vec2i & operator/( const float s );
		   vec2i & operator/( const double s );

	inline vec2i & operator/(       vec2i &v ) { return div( v ); }
	inline vec2i & operator/( const vec2i &v ) { return div( v ); }
		   vec2i & operator/(       vec2f &v );
		   vec2i & operator/( const vec2f &v );
		   vec2i & operator/(       vec2d &v );
		   vec2i & operator/( const vec2d &v );

	vec2i & normalize();

	vec2i & rotate( const float angle );

	vec2i & rotate_origin( const int x, const int y, const float angle );

	inline int length_sqr() { return ( x * x + y * y ); }

	int length();

	inline int dot( const vec2i &v ) { return static_cast<int>( this->x * v.x + this->y * v.y ); }

	inline int cross( const vec2i &v ) { return static_cast<int>( this->x * v.y - v.x * this->y ); }
};


struct vec2f
{
	float x, y;

	vec2f( const float x = 0.0f, const float y = 0.0f );
	vec2f( const vec2f &v );
	vec2f( const vec2i &v );
	vec2f( const vec2d &v );

	vec2f & set( const float x, const float y );
	vec2f & set( const vec2f &v );

	inline vec2f & operator=(       vec2f &v ) { return set( v ); }
	inline vec2f & operator=( const vec2f &v ) { return set( v ); }
		   vec2f & operator=(       vec2i &v );
		   vec2f & operator=( const vec2i &v );
		   vec2f & operator=(       vec2d &v );
		   vec2f & operator=( const vec2d &v );

	vec2f & add( const float x, const float y );
	vec2f & add( const vec2f &v );

	inline vec2f & operator+(       vec2f &v ) { return add( v ); }
	inline vec2f & operator+( const vec2f &v ) { return add( v ); }
		   vec2f & operator+(       vec2i &v );
		   vec2f & operator+( const vec2i &v );
		   vec2f & operator+(       vec2d &v );
		   vec2f & operator+( const vec2d &v );

	vec2f & sub( const float x, const float y );
	vec2f & sub( const vec2f &v );

	inline vec2f & operator-(       vec2f &v ) { return sub( v ); }
	inline vec2f & operator-( const vec2f &v ) { return sub( v ); }
		   vec2f & operator-(       vec2i &v );
		   vec2f & operator-( const vec2i &v );
		   vec2f & operator-(       vec2d &v );
		   vec2f & operator-( const vec2d &v );

	vec2f & mul( const float s );
	vec2f & mul( const float x, const float y );
	vec2f & mul( const vec2f &v );

	inline vec2f & operator*( const float s )  { return mul( s ); }
		   vec2f & operator*( const int s );
		   vec2f & operator*( const double s );

	inline vec2f & operator*(       vec2f &v ) { return mul( v ); }
	inline vec2f & operator*( const vec2f &v ) { return mul( v ); }
		   vec2f & operator*(       vec2i &v );
		   vec2f & operator*( const vec2i &v );
		   vec2f & operator*(       vec2d &v );
		   vec2f & operator*( const vec2d &v );

	vec2f & div( const float s );
	vec2f & div( const float x, const float y );
	vec2f & div( const vec2f &v );

	inline vec2f & operator/( const float s )  { return div( s ); }
		   vec2f & operator/( const int s );
		   vec2f & operator/( const double s );

	inline vec2f & operator/(       vec2f &v ) { return div( v ); }
	inline vec2f & operator/( const vec2f &v ) { return div( v ); }
		   vec2f & operator/(       vec2i &v );
		   vec2f & operator/( const vec2i &v );
		   vec2f & operator/(       vec2d &v );
		   vec2f & operator/( const vec2d &v );

	vec2f & normalize();

	vec2f & rotate( const float angle );

	vec2f & rotate_origin( const float x, const float y, const float angle );

	inline float length_sqr() { return ( x * x + y * y ); }

	float length();

	inline float dot( const vec2f &v ) { return ( this->x * v.x + this->y * v.y ); }

	inline float cross( const vec2f &v ) { return ( this->x * v.y - v.x * this->y ); }
};


struct vec2d
{
	double x, y;

	vec2d( const double x = 0.0, const double y = 0.0 );
	vec2d( const vec2d &v );
	vec2d( const vec2i &v );
	vec2d( const vec2f &v );

	vec2d & set( const double x, const double y );
	vec2d & set( const vec2d &v );

	inline vec2d & operator=(       vec2d &v ) { return set( v ); }
	inline vec2d & operator=( const vec2d &v ) { return set( v ); }
		   vec2d & operator=(       vec2i &v );
		   vec2d & operator=( const vec2i &v );
		   vec2d & operator=(       vec2f &v );
		   vec2d & operator=( const vec2f &v );

	vec2d & add( const double x, const double y );
	vec2d & add( const vec2d &v );

	inline vec2d & operator+(       vec2d &v ) { return add( v ); }
	inline vec2d & operator+( const vec2d &v ) { return add( v ); }
		   vec2d & operator+(       vec2i &v );
		   vec2d & operator+( const vec2i &v );
		   vec2d & operator+(       vec2f &v );
		   vec2d & operator+( const vec2f &v );

	vec2d & sub( const double x, const double y );
	vec2d & sub( const vec2d &v );

	inline vec2d & operator-(       vec2d &v ) { return sub( v ); }
	inline vec2d & operator-( const vec2d &v ) { return sub( v ); }
		   vec2d & operator-(       vec2i &v );
		   vec2d & operator-( const vec2i &v );
		   vec2d & operator-(       vec2f &v );
		   vec2d & operator-( const vec2f &v );

	vec2d & mul( const double s );
	vec2d & mul( const double x, const double y );
	vec2d & mul( const vec2d &v );

	inline vec2d & operator*( const double s ) { return mul( s ); }
		   vec2d & operator*( const int s );
		   vec2d & operator*( const float s );

	inline vec2d & operator*(       vec2d &v ) { return mul( v ); }
	inline vec2d & operator*( const vec2d &v ) { return mul( v ); }
		   vec2d & operator*(       vec2i &v );
		   vec2d & operator*( const vec2i &v );
		   vec2d & operator*(       vec2f &v );
		   vec2d & operator*( const vec2f &v );

	vec2d & div( const double s );
	vec2d & div( const double x, const double y );
	vec2d & div( const vec2d &v );

	inline vec2d & operator/( const double s ) { return div( s ); }
		   vec2d & operator/( const int s );
		   vec2d & operator/( const float s );

	inline vec2d & operator/(       vec2d &v ) { return div( v ); }
	inline vec2d & operator/( const vec2d &v ) { return div( v ); }
		   vec2d & operator/(       vec2i &v );
		   vec2d & operator/( const vec2i &v );
		   vec2d & operator/(       vec2f &v );
		   vec2d & operator/( const vec2f &v );

	vec2d & normalize();

	vec2d & rotate( const float angle );

	vec2d & rotate_origin( const double x, const double y, const float angle );

	inline double length_sqr() { return ( x * x + y * y ); }

	double length();

	inline double dot( const vec2d &v ) { return ( this->x * v.x + this->y * v.y ); }

	inline double cross( const vec2d &v ) { return ( this->x * v.y - v.x * this->y ); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static inline int vec2_length( vec2i &v )
	{ return v.length(); }

static inline int vec2_length( const int x, const int y )
	{ return vec2i( x, y ).length(); }

static inline int vec2_length( const int x1, const int y1, const int x2, const int y2 )
	{ return vec2i( x2 - x1, y2 - y1 ).length(); }

static inline float vec2_length( vec2f &v )
	{ return v.length(); }

static inline float vec2_length( const float x, const float y )
	{ return vec2f( x, y ).length(); }

static inline float vec2_length( const float x1, const float y1, const float x2, const float y2 )
	{ return vec2f( x2 - x1, y2 - y1 ).length(); }

static inline double vec2_length( vec2d &v )
	{ return v.length(); }

static inline double vec2_length( const double x, const double y )
	{ return vec2d( x, y ).length(); }

static inline double vec2_length( const double x1, const double y1, const double x2, const double y2 )
	{ return vec2d( x2 - x1, y2 - y1 ).length(); }


static inline int vec2_length_sqr( vec2i &v )
	{ return v.length_sqr(); }

static inline int vec2_length_sqr( const int x, const int y )
	{ return vec2i( x, y ).length_sqr(); }

static inline int vec2_length_sqr( const int x1, const int y1, const int x2, const int y2 )
	{ return vec2i( x2 - x1, y2 - y1 ).length_sqr(); }

static inline float vec2_length_sqr( vec2f &v )
	{ return v.length_sqr(); }

static inline float vec2_length_sqr( const float x, const float y )
	{ return vec2f( x, y ).length_sqr(); }

static inline float vec2_length_sqr( const float x1, const float y1, const float x2, const float y2 )
	{ return vec2f( x2 - x1, y2 - y1 ).length_sqr(); }

static inline double vec2_length_sqr( vec2d &v )
	{ return v.length_sqr(); }

static inline double vec2_length_sqr( const double x, const double y )
	{ return vec2d( x, y ).length_sqr(); }

static inline double vec2_length_sqr( const double x1, const double y1, const double x2, const double y2 )
	{ return vec2d( x2 - x1, y2 - y1 ).length_sqr(); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct rayi
{
	vec2i pos;
	vec2i dir;

	rayi( const int posX = 0, const int posY = 0, const int dirX = 0, const int dirY = 0 );
	rayi( const rayi &v );
	rayi( const rayf &v );
	rayi( const rayd &v );

	rayi & set( const int posX, const int posY, const int dirX, const int dirY );
	rayi & set( const rayi &v );
	rayi & set( const rayf &v );
	rayi & set( const rayd &v );

	inline rayi & operator=( rayi &v )       { return set( v ); }
	inline rayi & operator=( const rayi &v ) { return set( v ); }
	inline rayi & operator=( rayf &v )       { return set( v ); }
	inline rayi & operator=( const rayf &v ) { return set( v ); }
	inline rayi & operator=( rayd &v )       { return set( v ); }
	inline rayi & operator=( const rayd &v ) { return set( v ); }

	rayi & normalize();

	rayi & rotate( const float angle );

	rayi & rotate_origin( const int x, const int y, const float angle );

	inline int length_sqr() { return vec2_length_sqr( dir.x - pos.x, dir.y - pos.y ); }

	inline int length() { return vec2_length( dir.x - pos.x, dir.y - pos.y ); }

	inline int dot( const rayi &v ) { return this->dir.dot( v.dir ); }

	inline int cross( const rayi &v ) { return this->dir.cross( v.dir ); }

	// TODO: intersection
};


struct rayf
{
	vec2f pos;
	vec2f dir;

	rayf( const float posX = 0.0f, const float posY = 0.0f, const float dirX = 0.0f, const float dirY = 0.0f );
	rayf( const rayi &v );
	rayf( const rayf &v );
	rayf( const rayd &v );

	rayf & set( const float posX, const float posY, const float dirX, const float dirY );
	rayf & set( const rayi &v );
	rayf & set( const rayf &v );
	rayf & set( const rayd &v );

	inline rayf & operator=( rayi &v )       { return set( v ); }
	inline rayf & operator=( const rayi &v ) { return set( v ); }
	inline rayf & operator=(       rayf &v ) { return set( v ); }
	inline rayf & operator=( const rayf &v ) { return set( v ); }
	inline rayf & operator=(       rayd &v ) { return set( v ); }
	inline rayf & operator=( const rayd &v ) { return set( v ); }

	rayf & normalize();

	rayf & rotate( const float angle );

	rayf & rotate_origin( const float x, const float y, const float angle );

	inline float length_sqr() { return vec2_length_sqr( dir.x - pos.x, dir.y - pos.y ); }

	inline float length() { return vec2_length( dir.x - pos.x, dir.y - pos.y ); }

	inline float dot( const rayf &v ) { return this->dir.dot( v.dir ); }

	inline float cross( const rayf &v ) { return this->dir.cross( v.dir ); }

	// TODO: intersection
};


struct rayd
{
	vec2d pos;
	vec2d dir;

	rayd( const double posX = 0.0, const double posY = 0.0, const double dirX = 0.0, const double dirY = 0.0 );
	rayd( const rayi &v );
	rayd( const rayf &v );
	rayd( const rayd &v );

	rayd & set( const double posX, const double posY, const double dirX, const double dirY );
	rayd & set( const rayi &v );
	rayd & set( const rayf &v );
	rayd & set( const rayd &v );

	inline rayd & operator=( rayi &v )       { return set( v ); }
	inline rayd & operator=( const rayi &v ) { return set( v ); }
	inline rayd & operator=(       rayf &v ) { return set( v ); }
	inline rayd & operator=( const rayf &v ) { return set( v ); }
	inline rayd & operator=(       rayd &v ) { return set( v ); }
	inline rayd & operator=( const rayd &v ) { return set( v ); }

	rayd & normalize();

	rayd & rotate( const float angle );

	rayd & rotate_origin( const double x, const double y, const float angle );

	inline double length_sqr() { return vec2_length_sqr( dir.x - pos.x, dir.y - pos.y ); }

	inline double length() { return vec2_length( dir.x - pos.x, dir.y - pos.y ); }

	inline double dot( const rayd &v ) { return this->dir.dot( v.dir ); }

	inline double cross( const rayd &v ) { return this->dir.cross( v.dir ); }

	// TODO: intersection
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct recti
{
	int x1, y1;
	int x2, y2;

	recti( const int x1 = 0, const int y1 = 0, const int x2 = 0, const int y2 = 0 );
	recti( const recti &r );
	recti( const rectf &r );
	recti( const rectd &r );

	recti & set( const int x1, const int y1, const int x2, const int y2 );
	recti & set( const recti &r );

	inline recti & operator=(       recti &v ) { return set( v ); }
	inline recti & operator=( const recti &v ) { return set( v ); }
		   recti & operator=(       rectf &v );
		   recti & operator=( const rectf &v );
		   recti & operator=(       rectd &v );
		   recti & operator=( const rectd &v );

	inline int width() { return x2 - x1; }

	inline int height() { return y2 - y1; }

	inline int area() { return ( x2 - y1 ) * ( y2 - y1 ); }

	inline bool intersects( const recti &r ) { return ( x1 <= r.x2 && x2 >= r.x1 && y1 <= r.y2 && y2 >= r.y1 ); }
		   bool intersects( const rectf &r );
		   bool intersects( const rectd &r );
};


struct rectf
{
	float x1, y1;
	float x2, y2;

	rectf( const float x1 = 0.0f, const float y1 = 0.0f, const float x2 = 0.0f, const float y2 = 0.0f );
	rectf( const rectf &r );
	rectf( const recti &r );
	rectf( const rectd &r );

	rectf & set( const float x1, const float y1, const float x2, const float y2 );
	rectf & set( const rectf &r );

	inline rectf & operator=(       rectf &v ) { return set( v ); }
	inline rectf & operator=( const rectf &v ) { return set( v ); }
		   rectf & operator=(       recti &v );
		   rectf & operator=( const recti &v );
		   rectf & operator=(       rectd &v );
		   rectf & operator=( const rectd &v );

	inline float width() { return x2 - x1; }

	inline float height() { return y2 - y1; }

	inline float area() { return ( x2 - y1 ) * ( y2 - y1 ); }

	inline bool intersects( const rectf &r ) { return ( x1 <= r.x2 && x2 >= r.x1 && y1 <= r.y2 && y2 >= r.y1 ); }
		   bool intersects( const recti &r );
		   bool intersects( const rectd &r );
};


struct rectd
{
	double x1, y1;
	double x2, y2;

	rectd( const double x1 = 0.0f, const double y1 = 0.0f, const double x2 = 0.0f, const double y2 = 0.0f );
	rectd( const rectd &r );
	rectd( const recti &r );
	rectd( const rectf &r );

	rectd & set( const double x1, const double y1, const double x2, const double y2 );
	rectd & set( const rectd &r );

	inline rectd & operator=(       rectd &v ) { return set( v ); }
	inline rectd & operator=( const rectd &v ) { return set( v ); }
		   rectd & operator=(       recti &v );
		   rectd & operator=( const recti &v );
		   rectd & operator=(       rectf &v );
		   rectd & operator=( const rectf &v );

	inline double width() { return x2 - x1; }

	inline double height() { return y2 - y1; }

	inline double area() { return ( x2 - y1 ) * ( y2 - y1 ); }

	inline bool intersects( const rectd &r ) { return ( x1 <= r.x2 && x2 >= r.x1 && y1 <= r.y2 && y2 >= r.y1 ); }
		   bool intersects( const recti &r );
		   bool intersects( const rectf &r );
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline bool rect_intersect( recti &a, recti &b )
	{ return a.intersects( b ); }

inline bool rect_intersect( rectf &a, rectf &b )
	{ return a.intersects( b ); }

inline bool rect_intersect( rectd &a, rectd &b )
	{ return a.intersects( b ); }


static inline bool point_in_rect( const int px, const int py, const int x1, const int y1, const int x2, const int y2 )
	{ return ( px >= x1 && px <= x2 && py >= y1 && py <= y2 ); }

static inline bool point_in_rect( const vec2i &p, const int x1, const int y1, const int x2, const int y2 )
	{ return point_in_rect( p.x, p.y, x1, y1, x2, y2 ); }

static inline bool point_in_rect( const vec2i &p, const recti &rect )
	{ return point_in_rect( p.x, p.y, rect.x1, rect.y1, rect.x2, rect.y2 ); }

static inline bool point_in_rect( const int px, const int py, const recti &rect )
	{ return point_in_rect( px, py, rect.x1, rect.y1, rect.x2, rect.y2 ); }


static inline bool point_in_rect( const float px, const float py, const float x1, const float y1, const float x2, const float y2 )
	{ return ( px >= x1 && px <= x2 && py >= y1 && py <= y2 ); }

static inline bool point_in_rect( const vec2f &p, const float x1, const float y1, const float x2, const float y2 )
	{ return point_in_rect( p.x, p.y, x1, y1, x2, y2 ); }

static inline bool point_in_rect( const vec2f &p, const rectf &rect )
	{ return point_in_rect( p.x, p.y, rect.x1, rect.y1, rect.x2, rect.y2 ); }

static inline bool point_in_rect( const float px, const float py, const rectf &rect )
	{ return point_in_rect( px, py, rect.x1, rect.y1, rect.x2, rect.y2 ); }


static inline bool point_in_rect( const double px, const double py, const double x1, const double y1, const double x2, const double y2 )
	{ return ( px >= x1 && px <= x2 && py >= y1 && py <= y2 ); }

static inline bool point_in_rect( const vec2d &p, const double x1, const double y1, const double x2, const double y2 )
	{ return point_in_rect( p.x, p.y, x1, y1, x2, y2 ); }

static inline bool point_in_rect( const vec2d &p, const rectd &rect )
	{ return point_in_rect( p.x, p.y, rect.x1, rect.y1, rect.x2, rect.y2 ); }

static inline bool point_in_rect( const double px, const double py, const rectd &rect )
	{ return point_in_rect( px, py, rect.x1, rect.y1, rect.x2, rect.y2 ); }