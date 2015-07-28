#pragma once
#include <pulse/pulseaudio.h>
#include <QtCore>

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

	// testable variables
	bool StreamReady;

	void DoIteration();
};
