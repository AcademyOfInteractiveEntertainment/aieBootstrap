#include "Audio.h"
#include "SoundManager.h"
#include <memory>

namespace aie {

Audio::Audio(const char* filename)
	: m_soundManager(SoundManager::getInstance()),
	m_bufferID(0),
	m_sourceID(-1),
	m_next(nullptr),
    m_gain(1.0f),
    m_pitch(1.0f),
	m_looping(false) {
    m_soundManager->initialiseSound(this, filename);
}

Audio::~Audio() {
	m_soundManager->releaseSound(this);
}

void Audio::play() {
	m_soundManager->playSoundInternal(this);
}

void Audio::stop() {
	m_soundManager->stopSoundInternal(this);
}

void Audio::pause() {
	m_soundManager->pauseSoundInternal(this);
}

bool Audio::getIsPlaying() {
	return m_soundManager->getIsPlayingInternal(m_sourceID);
}

bool Audio::getIsPaused() {
	return m_soundManager->getIsPausedInternal(m_sourceID);
}

void Audio::setLooping(bool looping) {
	m_looping = looping;
}

void Audio::setPitch(float pitch) {
	m_pitch = pitch;

	if (m_sourceID >= 0)
		m_soundManager->setPitchInternal(this);
}

void Audio::setGain(float gain) {
	m_gain = gain;

	if (m_sourceID >= 0)
		m_soundManager->setGainInternal(this);
}

} // namespace aie
