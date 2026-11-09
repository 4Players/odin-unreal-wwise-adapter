# Unreal Plugin: Odin Wwise Adapter

![Wwise and ODIN](https://docs.4players.io/assets/images/wwise_with_odin_header-bb97f77fa2adf0aa23dcc9129f61c216.jpg)

This is an optional Unreal Engine plugin that provides an Audio Input Component for integrating the new Version 2 of our [ODIN Voice Chat Plugin](https://odin.4players.io/voice-chat/) with the [Audiokinetic Wwise Sound Engine](https://www.audiokinetic.com/products/wwise/) in Unreal.

__IMPORTANT:__ This Plugin requires ODIN Voice Chat Plugin version 20208.1.0 or newer (ODIN v2) and is not compatible with Version 1.x! A version compatible with Odin 1.x can be found in this [repository](https://github.com/4Players/odin-unreal-wwise-adapter-1.x).

## Guide

Following is a short guide outlining the integration of the Plugin into your Unreal Engine project. Alternatively you can look it up in our [Developer Documentation](https://docs.4players.io/voice/unreal/next/guides/odin-wwise/).

## Getting Started

To use the Component we recommend the following:

- Basic knowledge of Unreal as well as Wwise
- The Wwise Plugin for Unreal, which you can get from the [Audiokinetic Launcher](https://www.audiokinetic.com/download/)
- The ODIN Voice Chat Plugin, available [here](https://github.com/4Players/odin-sdk-unreal/releases)

To set up Wwise in your project, please follow Wwise's integration documentation. You can find the documentation [here](https://www.audiokinetic.com/library/edge/?source=UE4&id=index.html).

To set up the ODIN Voice Chat Plugin, please take a look at our [Getting-Started guide](https://docs.4players.io/voice/unreal/).

## Plugin Structure

The plugin consists primarily of the `UAkOdinInputComponent`, which serves as a substitute for the [UOdinSynthComponent](https://docs.4players.io/voice/unreal/blueprint-reference/odin-synth-component/). The component uses the Odin Decoder to generate Audio and forward it to the Wwise Audio Engine instead of to the Unreal Audio Engine.

## Usage

### Installation

This repository contains a source plugin. To install it:

1. Download this repository
2. Place its contents in `YourProject/Plugins/OdinWwiseAdapter`, with `OdinWwiseAdapter.uplugin` directly inside that folder
3. Enable the plugin and rebuild your project
4. Follow the steps below to use the Component in your project

### Wwise Project Setup

The `UAkOdinInputComponent` requires a Wwise Event to playback the Odin Audio Data in your Unreal Project. This enables you to define additional properties and effects in Wwise that should be applied to the voice chat audio. For instance, this allows audio designers to apply spatial audio processing to the voice chat.

To provide Unreal with the correct Wwise Event you need to add an Audio Input Plugin Source to your Soundbank. An example of this can be found in the [sample project](https://github.com/4Players/odin-unreal-wwise). To achieve this, you can follow these steps:

- In the `Audio` tab of the Wwise Project Explorer right-click on the wanted work unit of the `Containers` and add a `New Child->Audio Input`.
- Make adjustments to it like needed in your project.
- Make sure to go to the `Conversion` tab in the Contents Editor and set the Conversion to `Factory Conversion Settings->PCM->PCM as Input`.
- Right-click the newly created source and add a `New Event->Play` to it.
- If you have no Soundbank yet, create one.
- Lastly you need to add that event to the Soundbank by dragging it from the `Events` tab in the Project Explorer to the Soundbank's Content Editor.

Export using the Wwise Browser as described in the [Audiokinetic Guide for Unreal](https://www.audiokinetic.com/en/library/edge/?source=UE4&id=gettingstarted.html) and you are good to go! A quick way to import to your Unreal Project is to open it in the Unreal Editor and go to `Window->Wwise Browser`. Here you can click on `Generate Soundbanks` in the top right corner. Now you should be able to see your defined `Play_voice` event in the `Events` folder of the tree. Drag and drop it into a folder in your content browser to use it in the Unreal project.

### Integrating the Adapter in your Unreal Project

To use the Wwise adapter, replace the `UOdinSynthComponent` from your original Odin implementation with the `UAkOdinInputComponent`.

Before assigning a decoder, configure the component's `SampleRate` and `bIsStereo` to match it (defaults: 48000 Hz, mono). Invalid decoders or mismatched settings are rejected and logged as errors.

1. Set the `Ak Audio Event` on the component to the `Play_voice` event created in Wwise.
2. Assign the Odin Decoder using the `AssignOdinDecoder` function
3. Set the associated Wwise audio input event by calling `PostAssociatedAudioInputEvent` on the `AkOdinInputComponent`

Tip: You can add the `AkOdinInputComponent` directly on your Player Character as a component. This approach makes it easier to manage its properties and Wwise-specific settings.

Call `UnassignOdinDecoder` to disconnect the decoder. The next Wwise audio callback stops the event. To reuse the component, repeat the integration steps above with a matching decoder.

### Muting and Virtual Voices

To use `SetIsMuted`, assign a Wwise RTPC asset to `VoiceActivityRtpc` in the component's Blueprint defaults. Configure its Game Parameter with a range of 0 to 1 and a default of 1. Map it to the Audio Input's Voice Volume: 0 = -200 dB (muted), 1 = 0 dB (active). Set Virtual Voice Behavior to `Send to virtual voice`. Without an assigned RTPC, `SetIsMuted` only changes the component's mute flag.

Keep component ticking enabled while muted or virtualized, including by distance. The adapter drains skipped audio to maintain decoder events and prevent audio latency from accumulating.

## Conclusion

This Odin to Wwise adapter for Unreal provides powerful audio control for your voice chat integration. It enables your audio designers to leverage Wwise's extensive audio processing capabilities for voice chat, including:

- [Wwise Spatial Audio](https://www.audiokinetic.com/en/wwise/wwise-spatial-audio/) for proximity chat and position-based audio
- Advanced audio effects and processing
- Dynamic mixing and audio states

For more information, check out these resources:

- [Video Tutorial Series on Odin and Unreal Engine](https://www.youtube.com/playlist?list=PLAe4Im8mFTAuFFrFKnnl_MMJi8de7dYHs)
- [Odin General Introduction](https://docs.4players.io/voice/introduction/)
- [Odin Unreal SDK Blueprint Reference](https://docs.4players.io/voice/unreal/blueprint-reference/)
