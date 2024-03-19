#include <build/buffer.hpp>

#include <build/fileio.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Buffer::init( usize reserve, const bool grow )
{
	if( data != nullptr ) { free(); }
	capacity = reserve == 0 ? 1 : reserve;
	current = 0;
	fixed = !grow;
	tell = 0;
	data = reinterpret_cast<byte *>( memory_alloc( capacity ) );
	ErrorIf( data == nullptr, "Failed to allocate memory for init Buffer (%p: alloc %d bytes)", data, capacity );
}


void Buffer::free()
{
	if( data == nullptr ) { return; }
	memory_free( data );
	capacity = 0;
	current = 0;
	tell = 0;
	data = nullptr;
}


void Buffer::grow()
{
	Assert( data != nullptr );
	ErrorIf( capacity == USIZE_MAX, "Failed to grow Buffer: at capacity (%p: capacity: %d bytes)", data, USIZE_MAX );
	capacity = capacity == 0 ? 1 : ( capacity >= USIZE_MAX / 2 ? USIZE_MAX : capacity * 2 );
	data = reinterpret_cast<byte *>( memory_realloc( data, capacity ) );
	ErrorIf( data == nullptr, "Failed to reallocate memory for (grow) Buffer (%p: memory_realloc %d bytes)", data, capacity );
}


void Buffer::copy( const Buffer &other )
{
	Assert( other.data != nullptr );
	if( data != nullptr ) { free(); }
	init( other.capacity, !other.fixed );
	ErrorIf( data == nullptr, "Failed to copy Buffer (src %p, dst %p)", &other, this );
	memory_copy( data, other.data, other.capacity );
}


void Buffer::move( Buffer &&other )
{
	if( data != nullptr ) { free(); }

	data = other.data;
	tell = other.tell;
	capacity = other.capacity;
	current = other.current;
	fixed = other.fixed;

	other.data = nullptr;
	other.tell = 0;
	other.capacity = 0;
	other.current = 0;
	other.fixed = 0;
}


bool Buffer::save( const char *path )
{
	Assert( data != nullptr );
	FILE *file = fopen( path, "wb" );
	if( file == nullptr ) { return false; }

	bool returnCode = true;
	if( fwrite( data, current, 1, file ) < 1 ) { returnCode = false; goto cleanup; }

cleanup:
	if( fclose( file ) != 0 ) { return false; }
	return returnCode;
}


bool Buffer::load( const char *path, const bool grow )
{
	if( data != nullptr ) { free(); }

	FILE *file = fopen( path, "rb" );
	if( file == nullptr ) { return false; }

	bool returnCode = true;
	const usize size = fsize( file );
	if( size == 0 ) { returnCode = false; goto cleanup; }
	init( size, grow );
	if( fread( data, size, 1, file ) < 1 ) { returnCode = false; goto cleanup; }
	current = size;
	tell = 0;

cleanup:
	if( fclose( file ) != 0 ) { return false; }
	return returnCode;
}


void Buffer::write( void *data, const usize size )
{
	while( !fixed )
	{
		if( current + size < capacity ) { break; }
		grow();
	}
	ErrorIf( current + size >= capacity, "Failed writing to fixed buffer that is at capacity" );
	memory_copy( this->data + current, data, size );
	current += size;
	seek_end();
}