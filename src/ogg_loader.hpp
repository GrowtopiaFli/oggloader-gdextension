#pragma once

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/core/class_db.hpp>

/*
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/packed_int64_array.hpp>
#include <godot_cpp/variant/string.hpp>
*/

#include <godot_cpp/godot.hpp>

using namespace godot;

struct OggLoaderData {
	PackedInt64Array granule_positions;
	Array packet_data;
};

struct ogg_file
{
    char* curPtr;
    char* filePtr;
    size_t fileSize;
};


class OggLoader : public Object
{
	GDCLASS(OggLoader, Object);

	static OggLoader *singleton;

	enum {
		OGG_SYNC_BUFFER_SIZE = 8192,
	};

protected:
	static void _bind_methods();

private:
	Dictionary tree = Dictionary();

public:
	static OggLoader *get_singleton();

	OggLoader();
	~OggLoader();

	bool cached(String id);
	bool remove(String id);
	void clear();
	bool proc(String id, PackedByteArray file_data);
	PackedInt64Array get_granule_positions(String id);
	Array get_packet_data(String id);
	Dictionary get_ogg_metadata(PackedByteArray data);
};
