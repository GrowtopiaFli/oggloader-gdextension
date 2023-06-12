#include "ogg_loader.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#define OGG_IMPL
#define VORBIS_IMPL
#include "minivorbis.h"

using namespace godot;

OggLoader *OggLoader::singleton = nullptr;

void OggLoader::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("cached", "ID"), &OggLoader::cached);
	ClassDB::bind_method(D_METHOD("remove", "ID"), &OggLoader::remove);
	ClassDB::bind_method(D_METHOD("clear"), &OggLoader::clear);
	ClassDB::bind_method(D_METHOD("proc", "ID", "PackedByteArray"), &OggLoader::proc);
	ClassDB::bind_method(D_METHOD("get_granule_positions", "ID"), &OggLoader::get_granule_positions);
	ClassDB::bind_method(D_METHOD("get_packet_data", "ID"), &OggLoader::get_packet_data);
	ClassDB::bind_method(D_METHOD("get_ogg_metadata", "PackedByteArray"), &OggLoader::get_ogg_metadata);
}

OggLoader *OggLoader::get_singleton()
{
	return singleton;
}

OggLoader::OggLoader()
{
	ERR_FAIL_COND(singleton != nullptr);
	singleton = this;
}

OggLoader::~OggLoader()
{
	ERR_FAIL_COND(singleton != this);
	singleton = nullptr;
}

bool OggLoader::cached(String id)
{
	return tree.has(id);
}

bool OggLoader::remove(String id)
{
	return tree.erase(id);
}

void OggLoader::clear()
{
	tree.clear();
}

bool OggLoader::proc(String id, PackedByteArray file_data)
{
	PackedInt64Array arrA = PackedInt64Array();
	Array arrB = Array();
	int64_t len = file_data.size();
	ogg_stream_state stream_state;
	ogg_sync_state sync_state;
	ogg_page page;
	ogg_packet packet;
	bool initialized_stream = false;
	ogg_sync_init(&sync_state);
	int err;
	size_t cursor = 0;
	size_t packet_count = 0;
	bool done = false;
	while (!done) {
		err = ogg_sync_check(&sync_state);
		if (err != 0)
		{
			UtilityFunctions::print_verbose("Ogg sync error " + itos(err));
			return false;
		}
		while (ogg_sync_pageout(&sync_state, &page) != 1) {
			if (cursor >= len) {
				done = true;
				break;
			}
			err = ogg_sync_check(&sync_state);
			if (err != 0)
			{
				UtilityFunctions::print_verbose("Ogg sync error " + itos(err));
				return false;
			}
			char *sync_buf = ogg_sync_buffer(&sync_state, OGG_SYNC_BUFFER_SIZE);
			err = ogg_sync_check(&sync_state);
			if (err != 0)
			{
				UtilityFunctions::print_verbose("Ogg sync error " + itos(err));
				return false;
			}
			if (cursor > len) return false;
			size_t copy_size = len - cursor;
			if (copy_size > OGG_SYNC_BUFFER_SIZE) copy_size = OGG_SYNC_BUFFER_SIZE;
			memcpy(sync_buf, &file_data[cursor], copy_size);
			ogg_sync_wrote(&sync_state, copy_size);
			cursor += copy_size;
			err = ogg_sync_check(&sync_state);
			if (err != 0)
			{
				UtilityFunctions::print_verbose("Ogg sync error " + itos(err));
				return false;
			}
		}
		if (done) {
			break;
		}
		err = ogg_sync_check(&sync_state);
		if (err != 0)
		{
			UtilityFunctions::print_verbose("Ogg sync error " + itos(err));
			return false;
		}
		if (!initialized_stream) {
			if (ogg_stream_init(&stream_state, ogg_page_serialno(&page))) {
				UtilityFunctions::print_verbose("Failed allocating memory for Ogg Vorbis stream.");
				return false;
			}
			initialized_stream = true;
		}
		ogg_stream_pagein(&stream_state, &page);
		err = ogg_stream_check(&stream_state);
		if (err != 0)
		{
			UtilityFunctions::print_verbose("Ogg stream error " + itos(err));
			return false;
		}
		int desync_iters = 0;
		Array packet_data;
		int64_t granule_pos = 0;
		while (true) {
			err = ogg_stream_packetout(&stream_state, &packet);
			if (err == -1) {
				desync_iters++;
				if (desync_iters > 100)
				{
					UtilityFunctions::print_verbose("Packet sync issue during Ogg import");
					return false;
				}
				continue;
			} else if (err == 0) break;
			if (packet_count == 0 && vorbis_synthesis_idheader(&packet) == 0) {
				UtilityFunctions::print_verbose("Found a non-vorbis-header packet in a header position");
				if (initialized_stream) {
					ogg_stream_clear(&stream_state);
					initialized_stream = false;
				}
				break;
			}
			granule_pos = packet.granulepos;
			PackedByteArray data;
			data.resize(packet.bytes);
			memcpy(data.ptrw(), packet.packet, packet.bytes);
			packet_data.push_back(data);
			packet_count++;
		}
		if (initialized_stream) {
			arrA.push_back(granule_pos);
			arrB.push_back(packet_data);
		}
	}
	if (initialized_stream) ogg_stream_clear(&stream_state);
	ogg_sync_clear(&sync_state);
	if (arrA.is_empty())
	{
		UtilityFunctions::print_verbose("Ogg Vorbis decoding failed. Check that your data is a valid Ogg Vorbis audio stream.");
		return false;
	}
	if (tree.has(id)) tree.erase(id);
	Array arrC = Array();
	arrC.push_back(arrA);
	arrC.push_back(arrB);
	tree[id] = arrC;
	return true;
}

PackedInt64Array OggLoader::get_granule_positions(String id)
{
	if (!cached(id)) return PackedInt64Array();
	return (PackedInt64Array)((Array)tree[id])[0];
}

Array OggLoader::get_packet_data(String id)
{
	if (!cached(id)) return Array();
	return (Array)((Array)tree[id])[1];
}

Dictionary OggLoader::get_ogg_metadata(PackedByteArray data)
{
	ov_callbacks callbacks;
	ogg_file t;
	int64_t len = data.size();
	char* file_data = (char*)memalloc(sizeof(char) * len);
	for (int i = 0; i < len; i++) {
		file_data[i] = data[i];
	}
	t.curPtr = t.filePtr = file_data;
	t.fileSize = len;
	OggVorbis_File* ov = new OggVorbis_File;
	memset(ov, 0, sizeof(OggVorbis_File));
	callbacks.read_func = AR_readOgg;
	callbacks.seek_func = AR_seekOgg;
	callbacks.close_func = AR_closeOgg;
	callbacks.tell_func = AR_tellOgg;
	int ret = ov_open_callbacks((void*)&t, ov, NULL, -1, callbacks);
	if (ret != 0)
	{
		UtilityFunctions::print_verbose("Failed to load OGG.");
		return Dictionary();
	}
	vorbis_info* vi = ov_info(ov, -1);
	Dictionary info_data = Dictionary();
	info_data["rate"] = vi->rate;
	info_data["channels"] = vi->channels;
	info_data["bitrate"] = vi->bitrate_nominal;
	vorbis_comment* vc = ov_comment(ov, -1);
	Dictionary comment_data = Dictionary();
	comment_data["TITLE"] = "";
	comment_data["VERSION"] = "";
	comment_data["ALBUM"] = "";
	comment_data["TRACKNUMBER"] = "";
	comment_data["ARTIST"] = "";
	comment_data["PERFORMER"] = "";
	comment_data["COPYRIGHT"] = "";
	comment_data["LICENSE"] = "";
	comment_data["ORGANIZATION"] = "";
	comment_data["DESCRIPTION"] = "";
	comment_data["GENRE"] = "";
	comment_data["DATE"] = "";
	comment_data["LOCATION"] = "";
	comment_data["CONTACT"] = "";
	comment_data["ISRC"] = "";
	for (int i = 0; i < vc->comments; i++) {
		PackedStringArray comment = String(vc->user_comments[i]).substr(0, vc->comment_lengths[i]).split("=");
		int64_t com_size = comment.size();
		if (com_size >= 2) {
			String key = comment[0].to_upper();
			if (comment_data.has(key)) {
				comment.remove_at(0);
				comment.resize(com_size - 1);
				comment_data[key] = String("").join(comment);
			}
		}
	}
	ov_clear(ov);
	memfree(file_data);
	Dictionary return_data = Dictionary();
	return_data["info_data"] = info_data;
	return_data["comment_data"] = comment_data;
	return return_data;
}

