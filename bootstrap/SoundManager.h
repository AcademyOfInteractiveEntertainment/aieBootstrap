#pragma once

// forward decalre some OpenAL structures
typedef struct ALCdevice_struct ALCdevice;
typedef struct ALCcontext_struct ALCcontext;

namespace aie {

class Audio;

// maximum simultanious sounds, many sound cards only support 16
const unsigned int MAX_SOUNDS = 16;

// a singleton class that manages the sound drivers and API
class SoundManager {
public:

	// get the singleton instance
	static SoundManager* getInstance() { return m_instance; }

	// load a new sound file and return a pointer which can be used to control playback
	Audio* createSound(const char* filename);

	// destroy sound instance and free memory
	void destroySound(Audio* pAudio);

protected:

	// Audio objects have access to initialise and release their data
	friend class Audio;

	void initialiseSound(Audio* audio, const char* filename);
	void releaseSound(Audio* audio);

	// only the Application class can create, destroy, and update this class
	friend class Application;

	static void create() { m_instance = new SoundManager(); }
	static void destroy() { delete m_instance; }

	void update();

	// the singleton instance
	static SoundManager* m_instance;
	
	// internal functions, for use by sound system only
	void playSoundInternal(Audio* audio);
	void stopSoundInternal(Audio* audio);
	void pauseSoundInternal(Audio* audio);
	bool getIsPlayingInternal(int sourceID);
	bool getIsPausedInternal(int sourceID);
	void setPitchInternal(Audio* audio);
	void setGainInternal(Audio* audio);

private:

	SoundManager();
	~SoundManager();

	int getAvailableSource();

	ALCdevice*		m_openALDevice;
	ALCcontext*		m_openALContext;
	Audio*			m_audioList;
	unsigned int	m_audioSources[MAX_SOUNDS];
};

} // namespace aie