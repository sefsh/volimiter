//
//  volimiter
//
//  Written by Konstantin Anoshkin on 29.07.06.
//  This arsware is released under the terms of GNU GPL.
//

#import <Carbon/Carbon.h>
#include <CoreAudio/CoreAudio.h>

UInt32 gChannels[2];
Float32 gVolume;

void KAPrintError(const char * str, OSStatus err)
{
	printf("%s returned %c%c%c%c\n", str, (err & 0xff000000)>>24, (err & 0x00ff0000)>>16, (err & 0x0000ff00)>>8, err & 0x000000ff);
}

OSStatus VolumeHasChangedCallback ( AudioDeviceID inDevice, UInt32 inChannel, Boolean isInput, AudioDevicePropertyID inPropertyID, void* inClientData)
{
	Float32 volume;
	UInt32 size = sizeof(Float32);
	OSStatus err = AudioDeviceGetProperty(inDevice, inChannel, isInput, inPropertyID, &size, &volume);
	if ( err ) {
		KAPrintError("AudioDeviceGetProperty()", err);
		return err;
	}
	
	if ( volume > gVolume ) {
		err = AudioDeviceSetProperty(inDevice, NULL, inChannel, isInput, inPropertyID, size, &gVolume);
		if ( err ) {
			KAPrintError("AudioDeviceSetProperty()", err);
			return err;
		}
	}
	
	return err;
}

int main (int argc, const char * argv[])
{
	// Get the default output audio device
	AudioDeviceID device;
	UInt32 size = sizeof(AudioDeviceID);
	OSStatus err = AudioHardwareGetProperty(kAudioHardwarePropertyDefaultOutputDevice, &size, &device);
	if ( err ) {
		KAPrintError("AudioHardwareGetProperty()", err);
		return err;
	}
	
	// Get the default output audio device's channels
	size = sizeof(gChannels);
	err = AudioDeviceGetProperty(device, 0, false, kAudioDevicePropertyPreferredChannelsForStereo, &size, gChannels);
	if ( err ) {
		KAPrintError("AudioDeviceGetProperty()", err);
		return err;
	}
	
	// Check current volume
	size = sizeof(Float32);
	Float32 volL, volR;
	err = AudioDeviceGetProperty(device, gChannels[0], false, kAudioDevicePropertyVolumeScalar, &size, &volL);
	if ( err ) {
		KAPrintError("AudioDeviceGetProperty()", err);
		return err;
	}
	err = AudioDeviceGetProperty(device, gChannels[1], false, kAudioDevicePropertyVolumeScalar, &size, &volR);
	if ( err ) {
		KAPrintError("AudioDeviceGetProperty()", err);
		return err;
	}
	
	// We either use the volume supplied as an argument or stick to current volume
	if ( argc > 1 ) {
		sscanf(argv[1], "%f", &gVolume);
		// Sanity checks
		if ( gVolume < 0.0f )
			gVolume = 0.0f;
		else
			if ( gVolume > 1.0f )
				gVolume = 1.0f;
		// If current volume is already too high, set it now
		if ( volL > gVolume || volR > gVolume ) {
			err = AudioDeviceSetProperty(device, NULL, gChannels[0], false, kAudioDevicePropertyVolumeScalar, size, &gVolume);
			if ( err ) {
				KAPrintError("AudioDeviceSetProperty()", err);
				return err;
			}
			err = AudioDeviceSetProperty(device, NULL, gChannels[1], false, kAudioDevicePropertyVolumeScalar, size, &gVolume);
			if ( err ) {
				KAPrintError("AudioDeviceSetProperty()", err);
				return err;
			}
		}
	} else
		gVolume = (volL + volR)*0.5f;
	printf("volimiter [%d]: Restricting sound volume to %f\n", getpid(), gVolume);
	
	// Install our listener for changes in scalar volume for both channels
	err = AudioDeviceAddPropertyListener(device, gChannels[0], false, kAudioDevicePropertyVolumeScalar, (AudioDevicePropertyListenerProc) VolumeHasChangedCallback, NULL);
	if ( err ) {
		KAPrintError("AudioDeviceAddPropertyListener()", err);
		return err;
	}
	err = AudioDeviceAddPropertyListener(device, gChannels[1], false, kAudioDevicePropertyVolumeScalar, (AudioDevicePropertyListenerProc) VolumeHasChangedCallback, NULL);
	if ( err ) {
		KAPrintError("AudioDeviceAddPropertyListener()", err);
		return err;
	}
	
	// Run the event loop
	RunApplicationEventLoop();
	
    return 0;
}
