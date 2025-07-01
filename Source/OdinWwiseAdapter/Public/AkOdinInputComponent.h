#pragma once

#include "CoreMinimal.h"
#include "AkAudioInputComponent.h"
#include "AkOdinInputComponent.generated.h"

class UOdinDecoder;
class OdinSoundGenerator;
class UOdinPlaybackMedia;

/**
 * The UOdinAkInputComponent is designed for handling audio playback using the Wwise Audio Engine. Before calling
 * `UAkAudioInputComponent::PostAssociatedAudioInputEvent` you'll need to have set the Odin Decoder using
 * `UOdinAkInputComponent::AssignDecoder`. Otherwise it behaves as any other AkAudioInputComponent would.
 */
UCLASS(BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class ODINWWISEADAPTER_API UAkOdinInputComponent : public UAkAudioInputComponent
{
	GENERATED_BODY()

public:
	UAkOdinInputComponent(const class FObjectInitializer& ObjectInitializer);

	/**
	 * Assigns a decoder to the input component, initializes the necessary sound generator, and configures internal properties.
	 *
	 * This method sets the provided UOdinDecoder as the decoder for this component. It initializes an OdinSoundGenerator,
	 * ties it to the decoder for audio sample generation, and applies the decoder's sample rate and channel configuration
	 * to the component.
	 *
	 * @param InDecoder A reference to the decoder to be assigned to the component. If the provided decoder is null, the method logs an error and exits without performing any operations.
	 */
	UFUNCTION(BlueprintCallable, Category = "Odin|Sound")
	void SetDecoder(UPARAM(ref)
		UOdinDecoder*& InDecoder);

	/**
	 * Sets up the Wwise Audio configuration for this component. Requires a connected Odin Decoder to be able to
	 * set up correctly, otherwise information on the used sample rate and channel count is missing.
	 *
	 * @param AudioFormat The output parameter that will hold the channel configuration details after retrieval.
	 */
	virtual void GetChannelConfig(AkAudioFormat& AudioFormat) override;
	/**
	 * Fills the audio buffer with generated ODIN audio samples based on the specified number of channels and sample rate.
	 *
	 * @param NumChannels The number of audio channels requested.
	 * @param NumSamples The sample rate requested for audio generation.
	 * @param BufferToFill A pointer to the buffer that will be filled with generated audio samples.
	 * @return Returns true if the buffer was successfully filled with audio samples.
	 */
	virtual bool FillSamplesBuffer(uint32 NumChannels, uint32 NumSamples, float** BufferToFill) override;

protected:
	/**
	 * Used for processing audio input data  within the Odin audio system. Stores the decoder instance
	 * assigned to the component, which is responsible for retrieving sound data from ODIN. The decoder’s
	 * specific configuration settings (such as sample rate and channel count) determine how the audio data is
	 * processed and managed.
	 * 
	 * Access to this variable may be locked to ensure synchronization during decoder assignment or modification operations.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Odin|Sound")
	UOdinDecoder* Decoder = nullptr;


	/**
	 * OdinSoundGenerator instance, used to generate Odin-based audio samples.
	 */
	TSharedPtr<OdinSoundGenerator, ESPMode::ThreadSafe> SoundGenerator;
	/**
	 * Used to synchronize access from multiple threads to data within the
	 * UOdinAkInputComponent class. 
	 */
	FCriticalSection DataAccessCS;

	/**
	 * Audio Sample Rate. Is set to the connected Decoders sample rate on assignment.
	 */
	int32 SampleRate = 48000;
	/**
	 * Number of Audio Channels. Is set to the connected Decoders number of audio channels on assignment.
	 */
	int32 NumChannels = 1;
	/**
	 * Boolean flag indicating whether a valid decoder is available.
	 * Used to ensure that operations relying on a decoder are safe to be used.
	 */
	bool bHasValidDecoder = false;
	/**
	 * Buffer used to store audio samples temporarily during audio processing.
	 */
	TArray<float> Buffer;
};
