/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SubsynthAudioProcessor::SubsynthAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor (BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
                          .withInput ("Input", juce::AudioChannelSet::stereo(), true)
#endif
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
#endif
    )
#endif
{
    synth.addSound (new CustomSound());

    for (int i = 0; i < numVoices; i++)
    {
        synth.addVoice (new CustomVoice());
    }
}

SubsynthAudioProcessor::~SubsynthAudioProcessor()
{
}

// Returns the name of this processor.
//
// @return The name of the processor as a String obj.
const juce::String SubsynthAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

// Returns true if the processor wants MIDI messages.
bool SubsynthAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

// Returns true if the processor produces MIDI messages.
bool SubsynthAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

// Returns true if this is a MIDI effect plug-in and does no audio processing.
bool SubsynthAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

// Returns the length of the processor's tail, in seconds. No tail used (0 seconds).
double SubsynthAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

// Returns the number of preset programs the processor supports. Always returns at least 1.
int SubsynthAudioProcessor::getNumPrograms()
{
    return 1;
}

// Returns the number of the currently active program. Programs NYI, returns 0.
int SubsynthAudioProcessor::getCurrentProgram()
{
    return 0;
}

// Called by the host to change the current program. NYI, required template code.
void SubsynthAudioProcessor::setCurrentProgram (int index)
{
}

// Returns the name of a given program. NYI, required template code.
const juce::String SubsynthAudioProcessor::getProgramName (int index)
{
    return {};
}

// Called by the host to rename a program. NYI, required template code.
void SubsynthAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================

// Called before playback starts, to let the processor prepare itself.
//
// @param sampleRate: Target sample rate
// @param samplesPerBlock: A strong hint about the maximum number of samples 
// that will be provided in each block.
void SubsynthAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    synth.setCurrentPlaybackSampleRate (sampleRate);
    for (int i = 0; i < synth.getNumVoices(); i++)
        (dynamic_cast<CustomVoice*> (synth.getVoice (i)))->prepareToPlay (sampleRate, samplesPerBlock, getTotalNumOutputChannels());

    wfVisualiser.clear();
    runTests();
}

// Called after playback has stopped, to let the object free up any 
// resources it no longer needs.
void SubsynthAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    keyState.reset();
}


#ifndef JucePlugin_PreferredChannelConfigurations
// Callback to query if the AudioProcessor supports a specific layout.
//
// @param layouts: The bus layout to check
bool SubsynthAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

        // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

// Renders the next audio block
//
// @param buffer: The buffer obj to use for rendering
// @param midiMessages: The collected MIDI messages associated with this buffer.
void SubsynthAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    if (buffer.getNumSamples() == 0)
    {
        return;
    }

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    {
        buffer.clear (i, 0, buffer.getNumSamples());
    }
    // Scan midi buffer and add any messages generated by onscreen keyboard to buffer
    // injectIndirectEvents bool (last argument) must be true
    keyState.processNextMidiBuffer (midiMessages, 0, buffer.getNumSamples(), true);
    synth.renderNextBlock (buffer, midiMessages, 0, buffer.getNumSamples());

    wfVisualiser.pushBuffer (buffer);
}

//==============================================================================

// Processor subclass must override this and return true if it can create an editor component.
bool SubsynthAudioProcessor::hasEditor() const
{
    return true;
}

// Creates the processor's GUI via creating an AudioProcessorEditor
juce::AudioProcessorEditor* SubsynthAudioProcessor::createEditor()
{
    return new SubsynthAudioProcessorEditor (*this);
}

//==============================================================================

// The host will call this method when it wants to save the processor's internal state.
// NYI, template code.
void SubsynthAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

// This must restore the processor's state from a block of data previously created 
// using getStateInformation(). NYI, template code.
void SubsynthAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}


//============================= UI Callbacks ========================================


 // Calls the setADSR CustomVoice method to change the attack, decay, sustain, release 
 // values of the volume envelope on each voice instantiated within the synth data member.
 //
 // @param params: A set of attack, decay sustain, release values in an ADSR::Parameters
 // object for the volume envelope to be set to.
void SubsynthAudioProcessor::changeADSREnv (juce::ADSR::Parameters params)
{
    for (int i = 0; i < synth.getNumVoices(); i++)
    {
        dynamic_cast<CustomVoice*> (synth.getVoice (i))->setADSR (params);
    }
}

// Calls the setWave CustomVoice method to change the waveform being produced by 
// the oscillator in each voice of the synth data member.
//
// @param waveformNum: An integer representation for sine, square, saw, and triangle
// waveforms.
void SubsynthAudioProcessor::changeWaveform (int waveformNum)
{
    for (int i = 0; i < synth.getNumVoices(); i++)
    {
        dynamic_cast<CustomVoice*> (synth.getVoice (i))->setWave (waveformNum);
    }
}

// Calls the setGain CustomVoice method to change the gain being applied to
// the audio buffer of each voice of the synth data member
//
// @param gain: A number, in decibels, the gain voice data member should be set to.
void SubsynthAudioProcessor::changeVolume (double gain)
{
    for (int i = 0; i < synth.getNumVoices(); i++)
    {
        dynamic_cast<CustomVoice*> (synth.getVoice (i))->setGain (gain);
    }
}

// Calls the setFilter CustomVoice method to change the filter type, cutoff frequency,
// and resonance of the state variable filter of each voice in the synth data member
//
// @param filterNum: An integer representation of the filter type to be set low pass,
// band pass, high pass.
// @param cutoff: The cutoff frequency for the state variable filter in Hz.
// @param resonance: The amount of resonance to be applied by the state variable filter
void SubsynthAudioProcessor::changeFilter (int filterNum, float cutoff, float resonance)
{
    for (int i = 0; i < synth.getNumVoices(); i++)
    {
        dynamic_cast<CustomVoice*> (synth.getVoice (i))->setFilter (filterNum, cutoff, resonance);
    }
}

void SubsynthAudioProcessor::runTests()
{
    CustomVoice testVoice;
    testVoice.voiceTests();
}
//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SubsynthAudioProcessor();
}
