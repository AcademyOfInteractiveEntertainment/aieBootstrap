#pragma once

namespace aie {

class SoundManager;

class Audio {
public:

	Audio(const char* filename);
	~Audio();

	// control playback.
	void play();
	void stop();
	void pause();

	bool getIsPlaying();
	bool getIsPaused();

	// set the sound to loop until it's told to stop.
	void setLooping(bool looping);

	// change the pitch and tempo of the sound. Positive values only.
	void setPitch(float pitch);

	// change the volumn of the sound. Valid range is 0.0f - 1.0f.
	void setGain(float gain);

protected:

	// only available to the SoundManager.
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