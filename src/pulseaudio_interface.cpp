#include <cstdio>
#include "pulseaudio_interface.h"
//#include <pulse/i18n.h>

// Type of audio we're feeding.
static pa_sample_spec sample_spec = {
		.format = PA_SAMPLE_S16LE,
		.rate = 44100,
		.channels = 2
};

struct DataBlock
{
	void *orig;
	void *data;
	size_t len;

	DataBlock(void *memblk, size_t len) : len(len)
	{
		this->orig = this->data = malloc(len);
		memcpy(this->data, memblk, len);
	}

	~DataBlock()
	{
		free(this->orig);
	}
};

/* Connection draining complete */
static void context_drain_complete(pa_context*c, void *userdata)
{
	pa_context_disconnect(c);
}

/* Stream draining complete */
static void stream_drain_complete(pa_stream*s, int success, void *userdata)
{
	assert(userdata);
	Pulseaudio *pa = reinterpret_cast<Pulseaudio*>(userdata);
	pa_operation *o;

	if (!success)
	{
		fprintf(stderr, "Failed to drain stream: %s\n", pa_strerror(pa_context_errno(pa->context)));
		exit(1);
	}

	fprintf(stderr, "Playback stream drained.\n");

	pa_stream_disconnect(pa->stream);
	pa_stream_unref(pa->stream);
	pa->stream = nullptr;

	if (!(o = pa_context_drain(pa->context, context_drain_complete, nullptr)))
		pa_context_disconnect(pa->context);
	else
	{
		pa_operation_unref(o);

		fprintf(stderr, "Draining connection to server.\n");
	}
}

/* This is called whenever new data may be written to the stream */
static void stream_write_callback(pa_stream *s, size_t length, void *userdata)
{
	assert(userdata);
	Pulseaudio *pa = reinterpret_cast<Pulseaudio*>(userdata);
#if 0
	sf_count_t bytes;
	void *data;
	assert(s && length);

	if (!sndfile)
		return;

	data = pa_xmalloc(length);

	if (readf_function)
	{
		size_t k = pa_frame_size(&sample_spec);

		if ((bytes = readf_function(sndfile, data, (sf_count_t) (length/k))) > 0)
			bytes *= (sf_count_t) k;

	}
	else
		bytes = sf_read_raw(sndfile, data, (sf_count_t) length);

	if (bytes > 0)
		pa_stream_write(s, data, (size_t) bytes, pa_xfree, 0, PA_SEEK_RELATIVE);
	else
		pa_xfree(data);

	if (bytes < (sf_count_t) length)
	{
		sf_close(sndfile);
		sndfile = NULL;
		pa_operation_unref(pa_stream_drain(s, stream_drain_complete, pa));
	}
	#endif

	printf("Write callback!\n");

	//DataBlock *d = pa->AudioData.front();
	if (pa->player->GetPlaying())
	{
		// Allocate a buffer
		void *buffer = malloc(length);
		memset(buffer, 0, length);

		// Copy data into the buffer
		int status = pa->player->PlayBuffer(buffer, length, 1);
		printf("Writing %zu byes of playback data to pulseaudio\n", length);

		// Write the buffer to pulseaudio
		pa_stream_write(s, buffer, (size_t) length, NULL, 0, PA_SEEK_RELATIVE);

		// If that was the end of the song then drain the pulseaudio buffer
		if (status == -XMP_END)
		{
			printf("End if playback for file\n");
			pa_operation_unref(pa_stream_drain(s, stream_drain_complete, pa));
			pa->player->SetPlaying(false);
			pa->player->ReleaseFile();
		}

		free(buffer);
	}
	else
		pa->bufferlength = length;
}

/* This routine is called whenever the stream state changes */
static void stream_state_callback(pa_stream *s, void *userdata)
{
	assert(s);
	assert(userdata);

	Pulseaudio *pa = reinterpret_cast<Pulseaudio*>(userdata);

	switch (pa_stream_get_state(s))
	{
		case PA_STREAM_CREATING:
		case PA_STREAM_TERMINATED:
			break;

		case PA_STREAM_READY:
			fprintf(stderr, "Stream successfully created\n");
	        break;

		case PA_STREAM_FAILED:
		default:
			fprintf(stderr, "Stream errror: %s\n", pa_strerror(pa_context_errno(pa_stream_get_context(s))));
	        exit(1);
	}
}


/* This is called whenever the context status changes */
static void context_state_callback(pa_context *c, void *userdata)
{
	assert(c);
	assert(userdata);

	Pulseaudio *pa = reinterpret_cast<Pulseaudio*>(userdata);

	switch (pa_context_get_state(c))
	{
		case PA_CONTEXT_CONNECTING:
		case PA_CONTEXT_AUTHORIZING:
		case PA_CONTEXT_SETTING_NAME:
			break;

		case PA_CONTEXT_READY:
		{
			pa_cvolume cv;

			assert(c && !pa->stream);

			fprintf(stderr, "Connection established.\n");

			pa->stream = pa_stream_new(c, pa->stream_name, &sample_spec, pa->channel_map_set ? &pa->channel_map : NULL);
			if (!pa->stream)
				fprintf(stderr, "pa_stream_new failure: %s\n", pa_strerror(pa_context_errno(c)));
			assert(pa->stream);

			pa_stream_set_state_callback(pa->stream, stream_state_callback, pa);
			pa_stream_set_write_callback(pa->stream, stream_write_callback, pa);
			pa_stream_connect_playback(pa->stream, pa->device, NULL, (pa_stream_flags_t)0, pa_cvolume_set(&cv, sample_spec.channels, pa->volume), NULL);

			break;
		}

		case PA_CONTEXT_TERMINATED:
			exit(0);
	        break;

		case PA_CONTEXT_FAILED:
		default:
			fprintf(stderr, "Connection failure: %s\n", pa_strerror(pa_context_errno(c)));
	        exit(1);
	}
}

Pulseaudio::Pulseaudio() : channel_map_set(0), volume(PA_VOLUME_NORM),
                           stream_name(nullptr), client_name(nullptr),
                           device(nullptr), server(nullptr), context(nullptr),
                           stream(nullptr), mainloop_api(nullptr), m(nullptr),
                           StreamReady(false)
{
	// Empty because you should call Initialize() instead.
}

Pulseaudio::~Pulseaudio()
{
	if (this->stream)
		pa_stream_unref(this->stream);

	if (this->context)
		pa_context_unref(this->context);

	if (this->m)
	{
		pa_signal_done();
		pa_mainloop_free(this->m);
	}

	delete this->player;
}

void Pulseaudio::WriteAudioData(void *data, size_t len)
{
	this->AudioData.push_back(new DataBlock(data, len));
}

void Pulseaudio::DoIteration()
{
	/* Run the main loop - use pa_mainloop_iterate instead of pa_mainloop_run to make it non-blocking */
	int ret = 1;
	if (pa_mainloop_iterate(this->m, 0, &ret) < 0)
		fprintf(stderr, "pa_mainloop_iterate failed.\n");
}

bool Pulseaudio::Initialize()
{
	printf("Compiled with libpulse %s\nLinked with libpulse %s\n", pa_get_headers_version(), pa_get_library_version());
	this->player = new libxmp("", 44100);

	assert(pa_sample_spec_valid(&sample_spec));
	assert(pa_channel_map_parse(&this->channel_map, "left,right"));

	this->stream_name = strdup("Helicopter");

	char t[PA_SAMPLE_SPEC_SNPRINT_MAX];
	pa_sample_spec_snprint(t, sizeof(t), &sample_spec);
	fprintf(stderr, "Using sample spec '%s'\n", t);

	/* Set up a new main loop */
	if (!(this->m = pa_mainloop_new()))
	{
		fprintf(stderr, "pa_mainloop_new() failed.\n");
		return false;
	}

	this->mainloop_api = pa_mainloop_get_api(this->m);

	int r = pa_signal_init(this->mainloop_api);
	assert(r == 0);

	// Ignore SIGPIPE
#ifdef SIGPIPE
	signal(SIGPIPE, SIG_IGN);
#endif

	/* Create a new connection context */
	if (!(context = pa_context_new(this->mainloop_api, this->client_name)))
	{
		fprintf(stderr, "pa_context_new() failed.\n");
		return false;
	}

	pa_context_set_state_callback(this->context, context_state_callback, this);

	return true;
}


bool Pulseaudio::ConnectContext()
{
	/* Connect the context */
	if (pa_context_connect(this->context, this->server, (pa_context_flags_t)0, NULL) < 0) {
		fprintf(stderr, "pa_context_connect() failed: %s", pa_strerror(pa_context_errno(this->context)));
		return false;
	}
	return true;
}

void Pulseaudio::DisconnectContext()
{
	pa_context_disconnect(this->context);
}
