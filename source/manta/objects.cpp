#include <manta/objects.hpp>

#include <debug.hpp>
#include <manta/memory.hpp>

#include <vendor/vendor.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define NULL_BUCKET ( 0 )
#define NULL_TYPE ( 0 )

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ObjectContext::init()
{
	// State
	capacity = OBJECT_TYPE_COUNT;
	current = OBJECT_TYPE_COUNT;

	// Allocate 'buckets' Buffer
	buckets = reinterpret_cast<ObjectBucket *>( memory_alloc( capacity * sizeof( ObjectBucket ) ) );
	if( buckets == nullptr ) { return false; }

	// Default-initialize ObjectBuckets for every object type
	const ObjectBucket bucketPrototype { *this };
	for( u16 i = capacity; i > 0; i-- )
	{
		const u16 bucketID = i - 1;
		ObjectBucket *bucket = &buckets[bucketID];
		memory_copy( bucket, &bucketPrototype, sizeof( ObjectBucket ) );
		bucket->bucketID = bucketID;
		bucket->type = bucketID;
		bucket->bucketIDNext = i == capacity ? NULL_BUCKET : i;
		bucketCache[bucketID] = bucketID;
	}

	// Object Instances
	memory_set( objectCountType, 0, sizeof( objectCountType ) );

	// Success
	return true;
}


void ObjectContext::free()
{
	// Already freed?
	if( buckets == nullptr ) { return; }

	// Destroy all objects
	destroy_all();

	// Free ObjectBucket's
	for( u16 bucketID = 0; bucketID < current; bucketID++ ) { buckets[bucketID].free(); }

	// Free 'buckets' buffer
	if( buckets != nullptr )
	{
		memory_free( buckets );
		buckets = nullptr;
	}

	// Reset state
	capacity = 0;
	current = 0;
}


bool ObjectContext::grow()
{
	// Calculate new capacity
	if( capacity == U16_MAX ) { return false; }
	u32 newCapacity = ( capacity == 0 ? 1 : capacity * 2 );
	newCapacity = ( newCapacity > U16_MAX ? U16_MAX : newCapacity );

	// Realloc 'buckets' buffer
	buckets = reinterpret_cast<ObjectBucket *>( memory_realloc( buckets, newCapacity * sizeof( ObjectBucket ) ) );
	if( buckets == nullptr ) { return false; }

	// Default initialize new section of buffer
	const ObjectBucket bucketPrototype { *this };
	for( u16 bucketID = capacity; bucketID < static_cast<u16>( newCapacity ); bucketID++ )
	{
		memory_copy( &buckets[bucketID], &bucketPrototype, sizeof( ObjectBucket ) );
	}

	// Update capacity
	capacity = static_cast<u16>( newCapacity );

	// Success
	return true;
}


u16 ObjectContext::new_bucket( const u16 type )
{
	// Grow buffer?
	if( current == capacity )
	{
		if( !grow() ) { return NULL_BUCKET; }
	}

	// Initialize new bucket
	if( !buckets[current].init( type ) ) { return NULL_BUCKET; }
	buckets[current].bucketID = current;

	// Success
	return current++;
}


ObjectContext::ObjectBucket *ObjectContext::new_object_bucket( const u16 type )
{
	Assert( type > NULL_TYPE && type < OBJECT_TYPE_COUNT );

	// Can we add more of this object?
	if( objectCountType[type] == iObjects::OBJECT_TYPE_MAX_COUNT[type] )
	{
		return nullptr;
	}

	// Lazy initialize first bucket for each object type
	ObjectBucket *bucket = &buckets[bucketCache[type]];
	if( bucket->data == nullptr ) { bucket->init( type ); }

	// Find first bucket with room
	for( ;; )
	{
		// Early out if the bucket still has capacity
		if( LIKELY( bucket->current < iObjects::OBJECT_TYPE_BUCKET_CAPACITY[type] ) ) { break; }

		// This bucket is full, but a 'next' bucket of our type already exists
		if( bucket->bucketIDNext != NULL_BUCKET && buckets[bucket->bucketIDNext].type == bucket->type )
		{
			bucket = &buckets[bucket->bucketIDNext];
			continue;
		}
		// No existing buckets with room, lets make a new bucket
		const u16 bucketIDPre = bucket->bucketID; // Store this since new_bucket can cause pointer invalidation (realloc)
		const u16 bucketIDNew = new_bucket( type );
		if( UNLIKELY( bucketIDNew == NULL_BUCKET ) ) { return nullptr; } // ObjectContext must be completely full of buckets
		bucket = &buckets[bucketIDNew];

		// Update bucket links
		bucket->bucketIDNext = buckets[bucketIDPre].bucketIDNext;
		buckets[bucketIDPre].bucketIDNext = bucketIDNew;

		// End loop
		break;
	}

	// Add object
	return bucket;
}


Object ObjectContext::create_default_constructor( const u16 type )
{
	ObjectBucket *bucket = new_object_bucket( type );
	if( UNLIKELY( bucket == nullptr ) ) { return NULL_OBJECT; }
	return bucket->new_object( true );
}


Object ObjectContext::create_manual_constructor( const u16 type )
{
	ObjectBucket *bucket = new_object_bucket( type );
	if( UNLIKELY( bucket == nullptr ) ) { return NULL_OBJECT; }
	return bucket->new_object( false );
}


bool ObjectContext::destroy( Object &object )
{
	// Fetch Bucket
	if( UNLIKELY( object.bucketID >= current ) ) { return false; }
	ObjectBucket *bucket = &buckets[ object.bucketID ];
	if( UNLIKELY( object.type == NULL_TYPE || bucket->type != object.type ) ) { return false; }
	if( UNLIKELY( bucket->data == nullptr ) ) { return false; }

	// Remove Object
	const bool success = bucket->delete_object( object.index, object.generation );
	return success;
}


void ObjectContext::destroy_all()
{
	// Free all buckets
	const ObjectBucket bucketPrototype { *this };
	for( u16 i = current; i > 0; i-- )
	{
		const u16 bucketID = i - 1;
		ObjectBucket *bucket = &buckets[bucketID];

		// Free ObjectBucket memory
		bucket->clear();
		bucket->free();

		// Reset ObjectBucket state
		memory_copy( bucket, &bucketPrototype, sizeof( ObjectBucket ) );
		bucket->bucketID = bucketID;
		bucket->bucketIDNext = i == capacity ? NULL_BUCKET : i;
		bucket->type = bucketID;
		bucketCache[bucketID] = bucketID;
	}

	// Reset current
	current = OBJECT_TYPE_COUNT;
}


void ObjectContext::destroy_all_type( const u16 type )
{
	// Free buckets
	for( u16 bucketID = 0; bucketID < current; bucketID++ )
	{
		if( buckets[bucketID].type != type ) { continue; }
		buckets[bucketID].clear();
	}
}


byte * ObjectContext::object_pointer( const Object &object ) const
{
	// Fetch Bucket
	Assert( buckets != nullptr );
	if( UNLIKELY( object.bucketID >= current ) ) { return nullptr; } // bucketID is 'newer' than newest ObjectBucket (should never happen)
	ObjectBucket *bucket = &buckets[ object.bucketID ];
	if( UNLIKELY( object.type == NULL_TYPE || bucket->type != object.type ) ) { return nullptr; } // invalid object type or Object<->ObjectBucket missmatch
	if( UNLIKELY( bucket->data == nullptr ) ) { return nullptr; } // ObjectBucket isn't initialized

	// Get Object Pointer
	return bucket->get_object( object.index, object.generation );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ObjectContext::ObjectBucket::init( const u16 type )
{
	// State
	this->type = type;
	this->current = 0;
	this->top = 0;
	this->bottom = 0;

	// Allocate Memory
	data = reinterpret_cast<byte *>( memory_alloc( iObjects::OBJECT_TYPE_BUCKET_CAPACITY[type] * iObjects::OBJECT_TYPE_SIZE[type] ) );
	if( data != nullptr ) { memory_set( data, 0, iObjects::OBJECT_TYPE_BUCKET_CAPACITY[type] * iObjects::OBJECT_TYPE_SIZE[type] ); return true; }
	return false;
}


void ObjectContext::ObjectBucket::free()
{
	// Free memory
	if( data == nullptr ) { return; }
	memory_free( data );
	data = nullptr;
}


void ObjectContext::ObjectBucket::clear()
{
	// Destroy objects
	if( data == nullptr ) { return; }
	for( u32 i = 0; i < top; i++ )
	{
		byte *const objectPtr = data + i * iObjects::OBJECT_TYPE_SIZE[type];
		delete_object( i, reinterpret_cast<iObjects::OBJECT_BASE_t *>( objectPtr )->id.generation );
	}
}


Object ObjectContext::ObjectBucket::new_object( const bool defaultConstructor )
{
	// At capacity?
	Assert( data != nullptr );
	if( UNLIKELY( current == iObjects::OBJECT_TYPE_BUCKET_CAPACITY[type] ) ) { return NULL_OBJECT; }

	// Object Constructor
	byte *const objectPtr = data + current * iObjects::OBJECT_TYPE_SIZE[type];
	iObjects::OBJECT_BASE_t *object = reinterpret_cast<iObjects::OBJECT_BASE_t *>( objectPtr );
	const u16 generation = object->id.generation + 1;
	const byte *const constructorTable = defaultConstructor ? iObjects::OBJECT_CTOR_DEFAULT_BUFFER : iObjects::OBJECT_CTOR_MANUAL_BUFFER;
	const byte *const constructor = constructorTable + iObjects::OBJECT_CTOR_BUFFER_OFFSET[type];
	memory_copy( object, constructor, iObjects::OBJECT_TYPE_SIZE[type] );

	// Set Object
	object->id = { type, generation, bucketID, current };
	object->id.alive = true;

	// Move current to next open slot
	while( ++current < iObjects::OBJECT_TYPE_BUCKET_CAPACITY[type] )
	{
		const byte *const objectPtr = data + current * iObjects::OBJECT_TYPE_SIZE[type];
		if( reinterpret_cast<const iObjects::OBJECT_BASE_t *>( objectPtr )->id.alive == false ) { break; }
	}

	// Update bottom & top
	bottom = ( current - 1 ) < bottom ? ( current - 1 ) : bottom;
	top = current > top ? current : top;

	// Increment Object Count
	context.objectCountType[type]++;
	context.objectCountTotal++;

	// Cache bucket
	context.bucketCache[type] = bucketID;

	// Create Event
	object->event_create();

	// Success
	return object->id;
}


bool ObjectContext::ObjectBucket::delete_object( const u16 index, const u16 generation )
{
	// Verify alive
	Assert( data != nullptr );
	Assert( index < iObjects::OBJECT_TYPE_BUCKET_CAPACITY[type] );
	byte *const objectPtr = data + index * iObjects::OBJECT_TYPE_SIZE[type];
	iObjects::OBJECT_BASE_t *object = reinterpret_cast<iObjects::OBJECT_BASE_t *>( objectPtr );
	if( UNLIKELY( object->id.alive == false ) ) { return false; } // object isn't alive
	if( UNLIKELY( object->id.generation != generation ) ) { return false; } // object doesn't match current object's generation

	// Destroy event
	object->event_destroy();

	// Mark dead
	object->id.alive = false;

	// Update current
	if( index < current ) { current = index; }

	// Update bottom
	if( index == bottom )
	{
		iObjects::OBJECT_BASE_t *objectBottom = reinterpret_cast<iObjects::OBJECT_BASE_t *>( data + bottom * iObjects::OBJECT_TYPE_SIZE[type] );
		while( bottom < iObjects::OBJECT_TYPE_BUCKET_CAPACITY[type] && objectBottom->id.alive == false )
		{
			bottom++;
			objectBottom = reinterpret_cast<iObjects::OBJECT_BASE_t *>( data + bottom * iObjects::OBJECT_TYPE_SIZE[type] );
		}
		if( bottom == iObjects::OBJECT_TYPE_BUCKET_CAPACITY[type] ) { bottom = 0; }
	}

	// Update top
	if( index == ( top - 1 ) )
	{
		iObjects::OBJECT_BASE_t *objectTop = reinterpret_cast<iObjects::OBJECT_BASE_t *>( data + ( top - 1 ) * iObjects::OBJECT_TYPE_SIZE[type] );
		while( top > 0 && objectTop->id.alive == false )
		{
			top--;
			objectTop = reinterpret_cast<iObjects::OBJECT_BASE_t *>( data + ( top - 1 ) * iObjects::OBJECT_TYPE_SIZE[type] );
		}
	}

	// Cache bucket
	context.bucketCache[type] = context.bucketCache[type] > bucketID ? bucketID : context.bucketCache[type];

	// Decerement Object Count
	context.objectCountType[type]--;
	context.objectCountTotal--;

	// Success
	return true;
}


byte * ObjectContext::ObjectBucket::get_object( const u16 index, const u16 generation ) const
{
	// Get Object Pointer
	Assert( data != nullptr );
	Assert( index < iObjects::OBJECT_TYPE_BUCKET_CAPACITY[type] );
	byte *objectPtr = data + index * iObjects::OBJECT_TYPE_SIZE[type];
	const iObjects::OBJECT_BASE_t *const object = reinterpret_cast<iObjects::OBJECT_BASE_t *>( objectPtr );

	// Error cases
	if( UNLIKELY( object->id.alive == false ) ) { return nullptr; } // object slot isn't alive
	if( UNLIKELY( object->id.generation != generation ) ) { return nullptr; } // object doesn't match slot generation

	// Return Object Pointer
	return objectPtr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ObjectContext::ObjectBucketIterator::find_object( u32 startIndex )
{
	// Begin iteration from our bucket id
	if( UNLIKELY( this->type == NULL_TYPE || this->bucketID == NULL_BUCKET ) ) { this->ptr = nullptr; return; }
	ObjectBucket *bucket = &context.buckets[this->bucketID];

	// Loop over ObjectBuckets until we find a live object of our type
	for( ;; )
	{
		// Ensure the current ObjectBucket is initialized
		if( bucket->data != nullptr )
		{
			// Loop over the bucket's instances & check if they're alive
			for( u32 i = startIndex; i < bucket->top; i++ )
			{
				byte *const objectPtr = bucket->data + i * iObjects::OBJECT_TYPE_SIZE[bucket->type];
				const iObjects::OBJECT_BASE_t *const object = reinterpret_cast<const iObjects::OBJECT_BASE_t *>( objectPtr );

				if( object->id.alive )
				{
					this->ptr = objectPtr;
					this->bucketID = bucket->bucketID;
					this->index = static_cast<u16>( i );
					return; // Success!
				}
			}
		}

		// Move to the next bucket
		if( bucket->bucketIDNext == NULL_BUCKET ) { break; }
		bucket = &context.buckets[bucket->bucketIDNext];
		startIndex = bucket->bottom;

		// Check if the bucket is still our type
		if( bucket->type == this->type ) { continue; }

		// Are we iterating polymorphically (allow children types too)?
		if( !polymorphic ) { break; }

		// If so, check if the bucket is one of our child types
		const u16 targetTypeDepth = iObjects::OBJECT_TYPE_INHERITANCE_DEPTH[this->type];
		const u16 bucketTypeDepth = iObjects::OBJECT_TYPE_INHERITANCE_DEPTH[bucket->type];
		if( bucketTypeDepth > targetTypeDepth ) { continue; }

		// We must be at the end of valid buckets
		break;
	}

	// If we've made it this far, there are no live objects of our type
	ptr = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////