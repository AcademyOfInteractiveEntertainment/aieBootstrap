#pragma once

namespace aie {

class SoundManager;

// a class that wraps up an audio file (mp3, ogg, wav)
class Audio {
public:

	Audio(const char* filename);
	~Audio();

	// control playback
	void play();
	void stop();
	void pause();

	// query status
	bool getIsPlaying();
	bool getIsPaused();

	// set the sound to loop until it's told to stop
	void setLooping(bool looping);

	// change the pitch and tempo of the sound (positive values only)
	void setPitch(float pitch);

	// change the volumn of the sound (valid range is [0.0,1.0])
	void setGain(float gain);

protected:

	// only available to the SoundManager
	friend class SoundManager;
		
	unsigned int	m_bufferID;
	int				m_sourceID;
	bool			m_looping;
	float			m_pitch;
	float			m_gain;
	Audio*			m_next;

private:

	SoundManager* m_soundManager;
};

} // namespace aie