#pragma once

#include <types.hpp>
#include <debug.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <int N> struct ObjectHandle { };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define __INTERNAL_OBJECT_DEFINITION_BEGIN() namespace iObjects {
#define __INTERNAL_OBJECT_DEFINITION_END() /* namespace */ }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Object
{
public:
	Object() : alive( 0 ), type( 0 ), generation( 0 ), bucketID( 0 ), index( 0 ) { }
	Object( u16 type, u16 generation, u16 bucket, u16 index ) :
		alive( 0 ),
		type( type ),
		generation( generation ),
		bucketID( bucket ),
		index( index ) { }

	u16 alive : 1;  // alive flag
	u16 type : 15;  // object type (id)
	u16 generation; // age in ObjectBucket
	u16 bucketID;   // index into ObjectContext bucket array
	u16 index;      // object's index within ObjectBucket

public:
	template <int N> ObjectHandle<N> handle( const class ObjectContext &context ) const; // impl: objects.generated.cpp
	bool operator==( const Object &other ) const { return equals( other ); }

	u64 id() const
	{
		u64 result = 0;
		result |= static_cast<u64>( alive ) << 63;
		result |= static_cast<u64>( type ) << 48;
		result |= static_cast<u64>( generation ) << 32;
		result |= static_cast<u64>( bucketID ) << 16;
		result |= static_cast<u64>( index );
		return result;
	}

private:
	inline bool equals( const Object &other ) const
	{
		return ( other.alive == alive &&
				 other.type == type &&
				 other.generation == generation &&
				 other.bucketID == bucketID &&
				 other.index == index );
	}
};

#define NULL_OBJECT { }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <objects.generated.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ObjectContext
{
public:
	bool init();
	void free();

//private:
	bool grow();

	struct ObjectBucket
	{
		ObjectBucket( ObjectContext &context ) : context( context ) { }

		ObjectContext &context; // parent ObjectContext
		byte *data = nullptr;   // data buffer pointer
		u16 bucketIDNext = 0;   // index of next ObjectBucket in ObjectContext
		u16 bucketID = 0;       // index of this ObjectBucket in ObjectContext
		u16 current = 0;        // current insertion index
		u16 type = 0;           // object type
		u16 bottom = 0;         // lowest 'alive' index
		u16 top = 0;            // highest 'alive' index

		// Objects
		Object new_object( const bool defaultConstructor );
		bool delete_object( const u16 index, const u16 generation );
		byte *get_object( const u16 index, const u16 generation ) const;

		// Memory
		bool init( const u16 type );
		void free();
		void clear();
	};

	struct ObjectBucketIterator
	{
		ObjectContext &context; // parent ObjectContext
		byte *ptr = nullptr;    // pointer to current object instance data
		u16 type : 15;          // object type to iterate over
		u16 polymorphic : 1;    // if true, interate child types too
 		u16 bucketID = 0;       // current ObjectBucket index
		u16 index = 0;          // index within current ObjectBucket

		ObjectBucketIterator( ObjectContext &context, const u16 type, const bool polymorphic ) :
			context { context },
			ptr { nullptr },
			type { type },
			polymorphic { polymorphic },
			bucketID { type },
			index { 0 } { find_first(); } // begin() & end()

		void find_object( const u32 startIndex );
		inline void find_first() { find_object( 0 ); }
		inline void find_next() { find_object( index + 1 ); }
	};

	ObjectBucket *buckets = nullptr;    // ObjectBucket array (dynamic)
public:
	u16 bucketCache[OBJECT_TYPE_COUNT]; // most recent buckets touched by object create/destroy
	u16 capacity = 0;                   // number of allocated ObjectBucket slots
	u16 current = 0;                    // current ObjectBucket insertion index

	u32 objectCountTotal = 0;
	i32 objectCountType[OBJECT_TYPE_COUNT];

	u16 new_bucket( const u16 type );

	friend Object; // so Object::handle() can access ObjectContext::object_pointer()
	byte *object_pointer( const Object &object ) const;

	Object create_default_constructor( const u16 type );
	Object create_manual_constructor( const u16 type );
	ObjectBucket *new_object_bucket( const u16 type );

	inline Object create( const u16 type )
	{
		// Create object with default constructor
		return create_default_constructor( type );
	}

	template <int N, typename... Args> Object create( Args... args )
	{
		// Create objects with manual constructor
		iObjects::object_constructor<N, Args...>::construct( args... );
		return create_manual_constructor( N );
	}

	bool destroy( Object &object );
	void destroy_all();
	void destroy_all_type( const u16 type );

	inline bool exists( const Object &object ) const { return object_pointer( object ) != nullptr; }

	inline u32 count( const u16 type ) { Assert( type > 0 && type < OBJECT_TYPE_COUNT ); return objectCountType[type]; }
	inline u32 count_all() const { return objectCountTotal; }

	template <int N> struct ObjectIterator
	{
		ObjectBucketIterator itr;
		ObjectIterator( ObjectContext &context, const u16 type, const bool polymorphic ) : itr( { context, type, polymorphic } ) { }

		ObjectIterator<N> begin() { return { itr.context, itr.type, static_cast<bool>( itr.polymorphic ) }; }
		ObjectIterator<N> end() { return { itr.context, 0, static_cast<bool>( itr.polymorphic ) }; }

		bool operator!=( const ObjectIterator<N> &other ) const { return itr.ptr != other.itr.ptr; }
		ObjectIterator<N> &operator++() { itr.find_next(); return *this; }
		ObjectHandle<N> operator*() const { return { itr.ptr }; }
	};

	template <int N> ObjectIterator<N> objects( const bool includeChildren = false ) { Assert( buckets != nullptr ); return { *this, N, includeChildren }; }
	template <int N> ObjectHandle<N> handle( const Object &object ) const { return { object_pointer( object ) }; }

public:
	void event_create();
	void event_destroy();
	void event_initialize();
	void event_frame_start( const Delta delta );
	void event_frame_end( const Delta delta );
	void event_step_custom( const Delta delta );
	void event_step_gui( const Delta delta );
	void event_step( const Delta delta );
	void event_draw_custom( const Delta delta );
	void event_draw_gui( const Delta delta );
	void event_draw( const Delta delta );
	void event_sleep( const Delta delta );
	void event_wake( const Delta delta );
	void event_flag( const u64 code );
	void event_partition( void *ptr );
	void event_ui_mask( void *ptr );
	void event_save( byte *buffer );
	void event_load( byte *buffer );
	void event_network_send( byte *buffer );
	void event_network_receive( byte *buffer );
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Macro API:

	// Loop over all instances of a specified object type
	#define foreach_object( objectContext, objectType, handle ) \
		for( ObjectHandle<objectType> handle : objectContext.objects<objectType>( false ) )

	// Loop over all instances of a specified object type and derived child types
	#define foreach_object_polymorphic( objectContext, objectType, handle ) \
		for( ObjectHandle<objectType> handle : objectContext.objects<objectType>( true ) )
