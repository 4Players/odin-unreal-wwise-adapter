// Fill out your copyright notice in the Description page of Project Settings.


#include "AkOdinAudioInputComponent.h"
#include "OdinSubsystem.h"

void UAkOdinAudioInputComponent::AssignOdinMedia(UPARAM(ref) UOdinPlaybackMedia*& Media)
{
    if (nullptr == Media)
        return;

    this->SoundGenerator = MakeShared<OdinMediaSoundGenerator, ESPMode::ThreadSafe>();
    this->PlaybackMedia = Media;

    SoundGenerator->SetOdinStream(Media->GetMediaHandle());
}

void UAkOdinAudioInputComponent::GetChannelConfig(AkAudioFormat& AudioFormat)
{
	int NumChannels = ODIN_DEFAULT_CHANNEL_COUNT;
	int SampleRate = ODIN_DEFAULT_SAMPLE_RATE;

	if (GetWorld() && GetWorld()->GetGameInstance())
	{
		if (const UOdinSubsystem* OdinInitSubsystem =
			GetWorld()->GetGameInstance()->GetSubsystem<UOdinSubsystem>())
		{
			NumChannels = OdinInitSubsystem->GetChannelCount();
			SampleRate = OdinInitSubsystem->GetSampleRate();
		}
	}

	AkChannelConfig ChannelConfig;
	ChannelConfig.SetStandard(AK::ChannelMaskFromNumChannels(NumChannels));

	UE_LOG(LogTemp, Warning, TEXT("Initializing Ak Odin Input Component with %i channels and Sample Rate of %i"),
		NumChannels, SampleRate);

	// set audio format
	AudioFormat.SetAll(
		SampleRate, // Sample rate
		ChannelConfig, // \ref AkChannelConfig
		8 * sizeof(float), // Bits per samples
		sizeof(float), // Block Align = 4 Bytes? Shouldn't it be 2*4=8 Bytes, because of two channels?
		AK_FLOAT, // feeding floats
		AK_NONINTERLEAVED
	);
}

bool UAkOdinAudioInputComponent::FillSamplesBuffer(uint32 AkRequestedChannels, uint32 AkRequestedSampleRate, float** BufferToFill)
{
    if (!SoundGenerator || !PlaybackMedia)
        return false;

	const int32 RequestedTotalSamples = AkRequestedChannels * AkRequestedSampleRate;
	if (Buffer.Num() != RequestedTotalSamples)
	{
		Buffer.SetNum(RequestedTotalSamples);
	}

	uint32 NumGeneratedSamples;

	NumGeneratedSamples = SoundGenerator->OnGenerateAudio(Buffer.GetData(), RequestedTotalSamples);

	uint32 SamplesCount = 0;

	for (uint32 s = 0; s < NumGeneratedSamples; ++s)
	{
		if (SamplesCount >= NumGeneratedSamples)
			break;

		for (uint32 c = 0; c < AkRequestedChannels; ++c)
		{
			float intermediate = Buffer[s * AkRequestedChannels + c];
			BufferToFill[c][s] = intermediate;
			++SamplesCount;
		}
	}

	return NumGeneratedSamples > 0 && static_cast<int32>(NumGeneratedSamples) <= RequestedTotalSamples;
}