#include "AkOdinInputComponent.h"
#include "OdinAudio/OdinDecoder.h"
#include "OdinAudio/OdinSoundGenerator.h"

UAkOdinInputComponent::UAkOdinInputComponent(const FObjectInitializer& ObjectInitializer) : UAkAudioInputComponent(
	ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UAkOdinInputComponent::SetDecoder(UOdinDecoder*& InDecoder)
{
	if (!InDecoder)
	{
		UE_LOG(LogTemp, Error, TEXT("%s: Tried to assign null Decoder."), TEXT(__FUNCTION__));
		return;
	}

	{
		FScopeLock InitValuesLock(&DataAccessCS);
		Decoder = InDecoder;
		SoundGenerator = MakeShared<OdinSoundGenerator, ESPMode::ThreadSafe>();
		SoundGenerator->SetOdinDecoder(InDecoder);
		SampleRate = InDecoder->Samplerate;
		NumChannels = InDecoder->Stereo ? 2 : 1;
		bHasValidDecoder = true;
	}
}

void UAkOdinInputComponent::GetChannelConfig(AkAudioFormat& AudioFormat)
{
	int32 AkNumChannels, AkSampleRate;
	{
		FScopeLock InitValuesLock(&DataAccessCS);
		if (!bHasValidDecoder)
		{
			UE_LOG(LogTemp, Error,
			       TEXT(
				       "No Decoder assigned, please call UAkOdinInputComponent::AssignDecoder before using UAkAudioInputComponent::PostAssociatedAudioInputEvent."
			       ));
			return;
		}
		AkNumChannels = NumChannels;
		AkSampleRate = SampleRate;
	}

	AkChannelConfig ChannelConfig;
	ChannelConfig.SetStandard(AK::ChannelMaskFromNumChannels(AkNumChannels));

	UE_LOG(LogTemp, Log, TEXT("Initializing Ak Odin Input Component with %i channels and Sample Rate of %i"),
	       AkNumChannels, AkSampleRate);

	// set audio format
	AudioFormat.SetAll(
		AkSampleRate, // Sample rate
		ChannelConfig, // \ref AkChannelConfig
		8 * sizeof(float), // Bits per samples
		sizeof(float), // Block Align = 4 Bytes? Shouldn't it be 2*4=8 Bytes, because of two channels?
		AK_FLOAT, // feeding floats
		AK_NONINTERLEAVED
	);
}

bool UAkOdinInputComponent::FillSamplesBuffer(uint32 AkRequestedChannels, uint32 AkRequestedSampleRate,
                                              float** BufferToFill)
{
	const int32 RequestedTotalSamples = AkRequestedChannels * AkRequestedSampleRate;
	if (Buffer.Num() != RequestedTotalSamples)
	{
		Buffer.SetNum(RequestedTotalSamples);
	}

	uint32 NumGeneratedSamples;
	{
		FScopeLock InitValuesLock(&DataAccessCS);
		if (!bHasValidDecoder)
		{
			return false;
		}

		NumGeneratedSamples = SoundGenerator->OnGenerateAudio(Buffer.GetData(), RequestedTotalSamples);
	}

	for (uint32 s = 0; s < NumGeneratedSamples; ++s)
	{
		for (uint32 c = 0; c < AkRequestedChannels; ++c)
		{
			BufferToFill[c][s] = Buffer[s * AkRequestedChannels + c];
		}
	}

	return NumGeneratedSamples > 0 && static_cast<int32>(NumGeneratedSamples) <= RequestedTotalSamples;
}
