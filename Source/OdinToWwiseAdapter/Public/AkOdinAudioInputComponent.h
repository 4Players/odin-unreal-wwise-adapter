// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Odin.h"
#include "OdinMediaSoundGenerator.h"
#include "OdinPlaybackMedia.h"
#include "OdinFunctionLibrary.h"
#include "AkAudioInputComponent.h"
#include "AkOdinAudioInputComponent.generated.h"

/**
 * 
 */
class OdinMediaSoundGenerator;
class UOdinPlaybackMedia;

UCLASS(BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class ODINTOWWISEADAPTER_API UAkOdinAudioInputComponent : public UAkAudioInputComponent
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Odin|Sound")
	void AssignOdinMedia(UPARAM(ref) UOdinPlaybackMedia*& Media);

	virtual void GetChannelConfig(AkAudioFormat& AudioFormat) override;
	virtual bool FillSamplesBuffer(uint32 NumChannels, uint32 NumSamples, float** BufferToFill) override;

protected:
	UPROPERTY(Category = "Odin|Sound", BlueprintReadOnly)
	UOdinPlaybackMedia* PlaybackMedia = nullptr;
	TSharedPtr<OdinMediaSoundGenerator, ESPMode::ThreadSafe> SoundGenerator;
	
	TArray<float> Buffer;
};
