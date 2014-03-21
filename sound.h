#include "includes.h"

ALuint Buffers[NUM_BUFFERS];

ALuint Sources[NUM_SOURCES];

ALfloat SourcesPos[NUM_SOURCES][3];

ALfloat SourcesVel[NUM_SOURCES][3];

ALfloat ListenerPos[]={0.0,0.0,0.0};

ALfloat ListenerVel[]={0.0,0.0,0.0};

ALfloat ListenerOri[]={0.0,0.0,-1.0,0.0,1.0,0.0};                                                                                                                                            


ALboolean LoadALData()
{
	// Variables to load into.

	ALenum format;
	ALsizei size;
	ALvoid* data;
	ALsizei freq;
	ALboolean loop;

	// Load wav data into buffers.

	alGenBuffers(NUM_BUFFERS, Buffers);

	if (alGetError() != AL_NO_ERROR)
	{
		return AL_FALSE;
	}

	alutLoadWAVFile((ALbyte*)"motorcycle-start.wav", &format, &data, &size, &freq, &loop);
	
	alBufferData(Buffers[START], format, data, size, freq);
	alutUnloadWAV(format, data, size, freq);

	alutLoadWAVFile((ALbyte*)"back_bourne.wav", &format, &data, &size, &freq, &loop);
	alBufferData(Buffers[IDLE], format, data, size, freq);
	alutUnloadWAV(format, data, size, freq);


	alutLoadWAVFile((ALbyte*)"coin2.wav", &format, &data, &size, &freq, &loop);
	alBufferData(Buffers[COIN], format, data, size, freq);
	alutUnloadWAV(format, data, size, freq);
	
    alutLoadWAVFile((ALbyte*)"rooster.wav", &format, &data, &size, &freq, &loop);
	alBufferData(Buffers[ROOSTER], format, data, size, freq);
	alutUnloadWAV(format, data, size, freq);
	
    alutLoadWAVFile((ALbyte*)"wolf.wav", &format, &data, &size, &freq, &loop);
	alBufferData(Buffers[WOLF], format, data, size, freq);
	alutUnloadWAV(format, data, size, freq);
	// Bind buffers into audio sources.

	alGenSources(NUM_SOURCES, Sources);

	if (alGetError() != AL_NO_ERROR)
		return AL_FALSE;

	alSourcei (Sources[START], AL_BUFFER,   Buffers[START]  );
	alSourcef (Sources[START], AL_PITCH,    1.0              );
	alSourcef (Sources[START], AL_GAIN,     1.0              );
	alSourcefv(Sources[START], AL_POSITION, SourcesPos[START]);
	alSourcefv(Sources[START], AL_VELOCITY, SourcesVel[START]);
	alSourcei (Sources[START], AL_LOOPING,  AL_FALSE          );

	alSourcei (Sources[IDLE], AL_BUFFER,   Buffers[IDLE]  );
	alSourcef (Sources[IDLE], AL_PITCH,    1.0         );
	alSourcef (Sources[IDLE], AL_GAIN,     1.0            );
	alSourcefv(Sources[IDLE], AL_POSITION, SourcesPos[IDLE]);
	alSourcefv(Sources[IDLE], AL_VELOCITY, SourcesVel[IDLE]);
	alSourcei (Sources[IDLE], AL_LOOPING,  AL_TRUE       );


	alSourcei (Sources[COIN], AL_BUFFER,   Buffers[COIN]  );
	alSourcef (Sources[COIN], AL_PITCH,    1.0            );
	alSourcef (Sources[COIN], AL_GAIN,     1.0            );
	alSourcefv(Sources[COIN], AL_POSITION, SourcesPos[COIN]);
	alSourcefv(Sources[COIN], AL_VELOCITY, SourcesVel[COIN]);
	alSourcei (Sources[COIN], AL_LOOPING,  AL_FALSE       );

	alSourcei (Sources[ROOSTER], AL_BUFFER,   Buffers[ROOSTER]  );
	alSourcef (Sources[ROOSTER], AL_PITCH,    1.0            );
	alSourcef (Sources[ROOSTER], AL_GAIN,     1.0            );
	alSourcefv(Sources[ROOSTER], AL_POSITION, SourcesPos[ROOSTER]);
	alSourcefv(Sources[ROOSTER], AL_VELOCITY, SourcesVel[ROOSTER]);
	alSourcei (Sources[ROOSTER], AL_LOOPING,  AL_FALSE       );
	
    alSourcei (Sources[WOLF], AL_BUFFER,   Buffers[WOLF]  );
	alSourcef (Sources[WOLF], AL_PITCH,    1.0            );
	alSourcef (Sources[WOLF], AL_GAIN,     1.0            );
	alSourcefv(Sources[WOLF], AL_POSITION, SourcesPos[WOLF]);
	alSourcefv(Sources[WOLF], AL_VELOCITY, SourcesVel[WOLF]);
	alSourcei (Sources[WOLF], AL_LOOPING,  AL_FALSE       );
	// Do another error check and return.

	if( alGetError() != AL_NO_ERROR)
		return AL_FALSE;

	return AL_TRUE;
}

void SetListenerValues()
{
	alListenerfv(AL_POSITION,ListenerPos);
	alListenerfv(AL_VELOCITY,ListenerVel);
	alListenerfv(AL_ORIENTATION,ListenerOri);
}



void KillALData()
{
	alDeleteBuffers(NUM_BUFFERS, &Buffers[0]);
	alDeleteSources(NUM_SOURCES, &Sources[0]);
	alutExit();
}


void init_sound()
{
	alutInit(NULL,0);
	alGetError();
	if(LoadALData()==AL_FALSE)
		return;
	SetListenerValues();
	atexit(KillALData);
	alSourcefv(Sources[START], AL_POSITION, SourcesPos[START]);
	alSourcePlay(Sources[START]);
	alSourcefv(Sources[IDLE], AL_POSITION, SourcesPos[IDLE]);
	alSourcePlay(Sources[IDLE]);
	alSourcefv(Sources[COIN], AL_POSITION, SourcesPos[COIN]);
	alSourcefv(Sources[ROOSTER], AL_POSITION, SourcesPos[ROOSTER]);
	alSourcefv(Sources[WOLF], AL_POSITION, SourcesPos[WOLF]);
}
