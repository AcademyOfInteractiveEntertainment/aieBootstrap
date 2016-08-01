#pragma once

typedef struct ALCdevice_struct ALCdevice;
typedef struct ALCcontext_struct ALCcontext;

namespace aie {

class Audio;

// maximum simultanious sounds, many sound cards only support 16.
const unsigned int MAX_SOUNDS = 16;

class SoundManager {
public:

	// get the singleton instance.
	static SoundManager* getInstance() { return m_instance; }

	// load a new sound file and return a pointer which can be used to control playback.
	Audio* createSound(const char* filename);

	// destroy sound instance and free memory.
	void destroySound(Audio* pAudio);

protected:

	friend class Audio;

	void initialiseSound(Audio* audio, const char* filename);
	void releaseSound(Audio* audio);

	// only the "Application" class can create, destroy, and update this class.
	friend class Application;

	static SoundManager* m_instance;

	static void create() { m_instance = new SoundManager(); }
	static void destroy() { delete m_instance; }

	void update();
	
	// internal functions, for use by sound system only.
	friend class Audio;
	void playSoundInternal(Audio* pAudio);
	void stopSoundInternal(Audio* pAudio);
	void pauseSoundInternal(Audio* pAudio);
	bool getIsPlayingInternal(int nSourceID);
	bool getIsPausedInternal(int nSourceID);
	void setPitchInternal(Audio* pAudio);
	void setGainInternal(Audio* pAudio);

private:

	SoundManager();
	~SoundManager();

	int getAvailableSource();

	ALCdevice* m_openALDevice;
	ALCcontext* m_openALContext;
	Audio* m_audioList;
	unsigned int m_audioSources[MAX_SOUNDS];
};

} // namespace aie