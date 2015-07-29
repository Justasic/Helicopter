#pragma once
#include <pulse/pulseaudio.h>
#include <QtCore>
#include <deque>
#include "xmp_interface.h"

struct DataBlock;

class Pulseaudio : public QObject
{

public:
	Pulseaudio();
	~Pulseaudio();

	void WriteAudioData(void *data, size_t len);
	bool Initialize();


	// Variables. All public because I don't feel like dealing with shit.
	pa_channel_map channel_map;
	int channel_map_set;
	pa_volume_t volume;
	char *stream_name, *client_name, *device, *server;

	pa_context *context;
	pa_stream *stream;
	pa_mainloop_api *mainloop_api;
	pa_mainloop* m;

	// Length of buffer Pulseauido first requested.
	int bufferlength;

	// XMP player
	libxmp *player;

	// Unused?
	std::deque<DataBlock*> AudioData;

	// testable variables
	bool StreamReady;

	void DoIteration();
	void DisconnectContext();
	bool ConnectContext();
};
