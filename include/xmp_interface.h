#include <xmp.h>
#include <cstdio>
#include <string>

#if 0
EXPORT xmp_context xmp_create_context  (void);
EXPORT void        xmp_free_context    (xmp_context);
EXPORT int         xmp_test_module     (char *, struct xmp_test_info *);
EXPORT int         xmp_load_module     (xmp_context, char *);
EXPORT void        xmp_scan_module     (xmp_context);
EXPORT void        xmp_release_module  (xmp_context);
EXPORT int         xmp_start_player    (xmp_context, int, int);
EXPORT int         xmp_play_frame      (xmp_context);
EXPORT int         xmp_play_buffer     (xmp_context, void *, int, int);
EXPORT void        xmp_get_frame_info  (xmp_context, struct xmp_frame_info *);
EXPORT void        xmp_end_player      (xmp_context);
EXPORT void        xmp_inject_event    (xmp_context, int, struct xmp_event *);
EXPORT void        xmp_get_module_info (xmp_context, struct xmp_module_info *);
EXPORT char      **xmp_get_format_list (void);
EXPORT int         xmp_next_position   (xmp_context);
EXPORT int         xmp_prev_position   (xmp_context);
EXPORT int         xmp_set_position    (xmp_context, int);
EXPORT void        xmp_stop_module     (xmp_context);
EXPORT void        xmp_restart_module  (xmp_context);
EXPORT int         xmp_seek_time       (xmp_context, int);
EXPORT int         xmp_channel_mute    (xmp_context, int, int);
EXPORT int         xmp_channel_vol     (xmp_context, int, int);
EXPORT int         xmp_set_player      (xmp_context, int, int);
#endif

class libxmp
{
	xmp_context ctx;
	struct xmp_module_info mi;
	struct xmp_frame_info fi;

	int freq;
	bool playing;
	std::string filename;

public:
	libxmp(const std::string &filename = "", int = 48000);
	~libxmp();

	// Control functions
	bool Seek(int);
	bool NextPosition();
	bool PreviousPosition();
	void Stop();
	void Restart();
	bool Start();
	bool MuteChannel(int);
	bool SetChannelVolume(int, int);
	bool SetVolume(int);
	void SetPlaying(bool);

	// Run functions
	int PlayBuffer(void*, int, int);

	// Loading functions
	bool LoadFromFilePointer(FILE*, int = 0);
	bool LoadFromFile(const std::string &file);
	bool LoadFromBuffer(void*, size_t);

	// Unloading functions
	void ReleaseFile();


	// Getters
	struct xmp_frame_info *GetFrameInfo();
	struct xmp_module_info *GetModuleInfo();
	bool GetPlaying();
};

