#include "xmp_interface.h"
#include "file.h"

libxmp::libxmp(const std::string &filename, int freq) : freq(freq), playing(false), filename(filename)
{
	this->ctx = xmp_create_context();
	this->LoadFromFile(filename);
}

libxmp::~libxmp()
{
	this->ReleaseFile();
	xmp_free_context(this->ctx);
}

// Control functions
bool libxmp::Seek(int time)
{
	return xmp_seek_time(this->ctx, time);
}

bool libxmp::NextPosition()
{
	return xmp_next_position(this->ctx);
}

bool libxmp::PreviousPosition()
{
	return xmp_prev_position(this->ctx);
}

void libxmp::Stop()
{
	return xmp_stop_module(this->ctx);
}

void libxmp::Restart()
{
	return xmp_restart_module(this->ctx);
}

bool libxmp::Start()
{
	return xmp_start_player(this->ctx, this->freq, 0);
}

bool libxmp::MuteChannel(int channel)
{
	int prevstate = xmp_channel_mute(this->ctx, channel, -1);
	prevstate = !prevstate;
	return xmp_channel_mute(this->ctx, channel, prevstate);
}

bool libxmp::SetChannelVolume(int channel, int vol)
{
	vol %= 100;
	return xmp_channel_vol(this->ctx, channel, vol);
}

bool libxmp::SetVolume(int vol)
{
	vol %= 100;
	return xmp_set_player(this->ctx, XMP_PLAYER_VOLUME, vol);
}

// Loading functions
int libxmp::PlayBuffer(void *data, int length, int loop)
{
	return xmp_play_buffer(this->ctx, data, length, loop);
}

bool libxmp::LoadFromFile(const std::string &file)
{
	if (!is_file(file.c_str()))
		return false;

	return xmp_load_module(this->ctx, const_cast<char*>(file.c_str()));
}

bool libxmp::LoadFromFilePointer(FILE *f, int size)
{
	#ifndef xmp_load_module_from_file

	fpos_t pos;
	fgetpos(f, &pos);
	fseek(f, 0, SEEK_END);
	size_t len = ftell(0);
	fsetpos(f, &pos);

	void *buffer = malloc(len);
	fread(buffer, 1, len, f);
	if (size <= 0)
		size = len;
	int st = xmp_load_module_from_memory(this->ctx, buffer, size);
	free(buffer);
	return st;
	#else
	return xmp_load_module_from_file(this->ctx, f, size);
	#endif
}

bool libxmp::LoadFromBuffer(void *buf, size_t len)
{
	return xmp_load_module_from_memory(this->ctx, buf, len);
}

// Unloading functions
void libxmp::ReleaseFile()
{
	xmp_end_player(this->ctx);
	xmp_release_module(this->ctx);
}

// Getters
struct xmp_frame_info *libxmp::GetFrameInfo()
{
	struct xmp_frame_info *ret = new struct xmp_frame_info;
	xmp_get_frame_info(this->ctx, ret);
	return ret;
}

struct xmp_module_info *libxmp::GetModuleInfo()
{
	struct xmp_module_info *ret = new struct xmp_module_info;
	xmp_get_module_info(this->ctx, ret);
	return ret;
}

void libxmp::SetPlaying(bool st)
{
	this->playing = st;
}

bool libxmp::GetPlaying()
{
	return this->playing;
}
