#include "SoundManager.h"
#include <sndfile.hh>
#include "Audio.h"
#include <al.h>
#include <alc.h>

namespace aie {

SoundManager* SoundManager::m_instance = nullptr;

static int getSampleSize(int format) {
	int subFormat = format & SF_FORMAT_SUBMASK;
	switch (subFormat) {
	case SF_FORMAT_PCM_16:		return sizeof(short);
	case SF_FORMAT_GSM610:		return sizeof(short);
	case SF_FORMAT_DWVW_16:		return sizeof(short);
	case SF_FORMAT_DPCM_16:		return sizeof(short);
	case SF_FORMAT_VORBIS:		return sizeof(short);
	case SF_FORMAT_ALAC_16:		return sizeof(short);
	case SF_FORMAT_PCM_U8:		return sizeof(unsigned char);

	//Untested formats
	case SF_FORMAT_PCM_24:		return sizeof(char[3]);
	case SF_FORMAT_DWVW_24:		return sizeof(char[3]);
	case SF_FORMAT_ALAC_24:		return sizeof(char[3]);
	case SF_FORMAT_PCM_32:		return sizeof(int);
	case SF_FORMAT_ALAC_32:		return sizeof(int);
	case SF_FORMAT_FLOAT:		return sizeof(float);
	case SF_FORMAT_DOUBLE:		return sizeof(double);
	
	default:					return sizeof(char);
	}
}

static int getOpenALFormat(int fileFormat, int channels) {
	int subFormat = fileFormat & SF_FORMAT_SUBMASK;
	switch (subFormat) {
		case SF_FORMAT_PCM_16:
		case SF_FORMAT_DPCM_16:
		case SF_FORMAT_ALAC_16:
		case SF_FORMAT_VORBIS:
		case SF_FORMAT_GSM610: {
			if(channels > 1)
				return AL_FORMAT_STEREO16;
			
			return AL_FORMAT_MONO16;
		}
		default: {
			if(channels > 1)
				return AL_FORMAT_STEREO8;
			
			return AL_FORMAT_MONO8;
		}
	}
}

SoundManager::SoundManager() {
	//Create OpenAL device and setup context.
	m_openALDevice = alcOpenDevice(nullptr);
	if (alGetError() != AL_NO_ERROR) {
		//error
		printf("Could not initialise OpenAL\n");
	}
	m_openALContext = alcCreateContext(m_openALDevice, nullptr);
	if (alGetError() != AL_NO_ERROR) {
		//error
		printf("Could not initialise OpenAL\n");
	}
	alcMakeContextCurrent(m_openALContext);
	if (alGetError() != AL_NO_ERROR) {
		//error
		printf("Could not initialise OpenAL\n");
	}

	//Create sources and buffers
	alGenSources(MAX_SOUNDS, m_audioSources);

	if (alGetError() != AL_NO_ERROR) {
		//error
		printf("Could not initialise OpenAL\n");
	}

	m_audioList = nullptr;
}

SoundManager::~SoundManager() {
	//Close OpenAL device
	alcMakeContextCurrent(nullptr);
	alcDestroyContext(m_openALContext);
	alcCloseDevice(m_openALDevice);
}

void SoundManager::update() {
	Audio* pCurrent = m_audioList;
	while (pCurrent != nullptr) {
		int nSourceID = pCurrent->m_sourceID;
		if (nSourceID >= 0) {
			if (!getIsPlayingInternal(nSourceID) &&
				!getIsPausedInternal(nSourceID)) {
				if (pCurrent->m_looping) {
					playSoundInternal(pCurrent);
				}
				else {
					//Unbind sound from source so other sounds can use it.
					alSourcei(m_audioSources[nSourceID], AL_BUFFER, 0);
					pCurrent->m_sourceID = -1;
				}
			}
		}

		pCurrent = pCurrent->m_next;
	}
}

Audio* SoundManager::createSound(const char* filename) {
	// create Audio instance and add to linked list.
	return new Audio(filename);
}

void SoundManager::destroySound(Audio* audio) {
	// delete audio instance.
	delete audio;
}

void SoundManager::initialiseSound(Audio* audio, const char* filename) {
	SNDFILE *infile;
	SF_INFO audioInfo;
	ALuint bufferID;

	//Open sound file using libsndfile.
	memset(&audioInfo, 0, sizeof(SF_INFO));
	if (!(infile = sf_open(filename, SFM_READ, &audioInfo))) {
		printf("Not able to open input file %s.\n", filename);
		puts(sf_strerror(nullptr));
		return;
	}

	//Allocate enough memory to store the sound data.
	int sampleSize = getSampleSize(audioInfo.format);
	int sampleCount = (int)audioInfo.frames * audioInfo.channels;
	void* pAudioData = malloc(sampleSize * sampleCount);
	if (pAudioData == nullptr) {
		printf("Not able to allocate memory for input file %s.\n", filename);
		sf_close(infile);
		return;
	}

	//Read in the sound data from the file using libsndfile.
	//sf_read_short() is the recommended function to read data from any file format.
	//Using sf_read_raw() if the file is PCM 8 because sf_read_short() seems to crash on these formats.
	//This might be a bug in libsndfile or at least undocumented behavior.
	if (sampleSize == 1)
		sf_read_raw(infile, pAudioData, sampleCount);
	else
		sf_read_short(infile, (short*)pAudioData, sampleCount);

	//Create OpenAL buffer and load the sound data into it.
	int format = getOpenALFormat(audioInfo.format, audioInfo.channels);
	alGenBuffers(1, &bufferID);
	alBufferData(bufferID, format, pAudioData, sampleSize * sampleCount, audioInfo.samplerate);

	//Free memory and close file.
	free(pAudioData);
	sf_close(infile);

	// Create Audio instance and add to linked list.
	audio->m_bufferID = bufferID;
	if (m_audioList != nullptr) {
		audio->m_next = m_audioList->m_next;
		m_audioList->m_next = audio;
	}
	else
		m_audioList = audio;
}

void SoundManager::releaseSound(Audio* audio) {
	if (audio == nullptr)
		return;

	//Stop sound if it is playing.
	stopSoundInternal(audio);

	//Delete the sound data.
	alDeleteBuffers(1, &audio->m_bufferID);

	//Remove from linked list.
	Audio* pCurrent = m_audioList;
	Audio* pPrev = nullptr;
	while (pCurrent != nullptr) {
		if (pCurrent == audio)
			break;

		pPrev = pCurrent;
		pCurrent = pCurrent->m_next;
	}

	if (pPrev != nullptr)
		pPrev->m_next = pCurrent->m_next;
	else
		m_audioList = pCurrent->m_next;
}

void SoundManager::playSoundInternal(Audio* audio) {
	if (audio == nullptr)
		return;

	//Check if sound has already been bound to a source.
	if (audio->m_sourceID >= 0) {
		//If it's not already playing, play it.
		if (!getIsPlayingInternal(audio->m_sourceID)) {
			setPitchInternal(audio);
			setGainInternal(audio);
			alSourcePlay(m_audioSources[audio->m_sourceID]);
		}
	}
	else {
		//Sound not already bout to a source so find the next available one.
		int nSourceID = getAvailableSource();

		//Bind sound to source.
		alSourcei(m_audioSources[nSourceID], AL_BUFFER, audio->m_bufferID);
		audio->m_sourceID = nSourceID;

		setPitchInternal(audio);
		setGainInternal(audio);

		//Play sound.
		alSourcePlay(m_audioSources[nSourceID]);
	}
}

void SoundManager::stopSoundInternal(Audio* pAudio) {
	if (pAudio != nullptr &&
		pAudio->m_sourceID >= 0) {
		//Stop the sound.
		alSourceStop(m_audioSources[pAudio->m_sourceID]);

		//Unbind sound from source so other sounds can use it.
		alSourcei(m_audioSources[pAudio->m_sourceID], AL_BUFFER, 0);
		pAudio->m_sourceID = -1;
	}
}

void SoundManager::pauseSoundInternal(Audio* pAudio) {
	if (pAudio != nullptr &&
		pAudio->m_sourceID >= 0 &&
		getIsPlayingInternal(pAudio->m_sourceID)) {
		alSourcePause(m_audioSources[pAudio->m_sourceID]);
	}
}

bool SoundManager::getIsPlayingInternal(int nSourceID) {
	if (nSourceID >= 0) {
		ALint state;
		alGetSourcei(m_audioSources[nSourceID], AL_SOURCE_STATE, &state);
		return (state == AL_PLAYING);
	}
	return false;
}

bool SoundManager::getIsPausedInternal(int nSourceID) {
	if (nSourceID >= 0) {
		ALint state;
		alGetSourcei(m_audioSources[nSourceID], AL_SOURCE_STATE, &state);
		return (state == AL_PAUSED);
	}
	return false;
}

int SoundManager::getAvailableSource() {
	for (int i = 0; i < MAX_SOUNDS; ++i) {
		if (!getIsPlayingInternal(i) &&
			!getIsPausedInternal(i))
			return i;
	}

	return 0;
}

void SoundManager::setPitchInternal(Audio* pAudio) {
	if (pAudio != nullptr && 
		pAudio->m_sourceID >= 0) {
		alSourcef(m_audioSources[pAudio->m_sourceID], AL_PITCH, pAudio->m_pitch);
	}
}

void SoundManager::setGainInternal(Audio* pAudio) {
	if (pAudio != nullptr &&
		pAudio->m_sourceID >= 0) {
		alSourcef(m_audioSources[pAudio->m_sourceID], AL_GAIN, pAudio->m_gain);
	}
}

} // namespace aie