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
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    synth.addSound(new CustomSound());
    
    for (int i = 0; i < numVoices; i++) {
        synth.addVoice(new CustomVoice());
    }
}

SubsynthAudioProcessor::~SubsynthAudioProcessor()
{
}

//==============================================================================
const juce::String SubsynthAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SubsynthAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SubsynthAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SubsynthAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SubsynthAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SubsynthAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SubsynthAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SubsynthAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SubsynthAudioProcessor::getProgramName (int index)
{
    return {};
}

void SubsynthAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SubsynthAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    synth.setCurrentPlaybackSampleRate(sampleRate)
    for (int i = 0; i < synth.getNumVoices(); i++)
        (dynamic_cast<CustomVoice*>(synth.getVoice(i)))->prepareToPlay(sampleRate, samplesPerBlock, getTotalNumOutputChannels()); 
}

void SubsynthAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    keyState.reset();
}

#ifndef JucePlugin_PreferredChannelConfigurations
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

void SubsynthAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    
    if (buffer.getNumSamples() == 0) {
        return;
    }
    
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
        buffer.clear (i, 0, buffer.getNumSamples());
        midiMessages.clear();
    }
    // Scan midi buffer and add any messages generated by onscreen keyboard to buffer
    // injectIndirectEvents bool (last argument) must be true
    keyState.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), true);
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

//==============================================================================
bool SubsynthAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SubsynthAudioProcessor::createEditor()
{
    return new SubsynthAudioProcessorEditor (*this);
}

//==============================================================================
void SubsynthAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SubsynthAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//====== UI Component Callbacks ================================================


void SubsynthAudioProcessor::changeADSREnv(juce::ADSR::Parameters params) {
    for (int i = 0; i < synth.getNumVoices(); i++) {
        dynamic_cast<CustomVoice*>(synth.getVoice(i))->setADSR(params);
    }
}

void SubsynthAudioProcessor::changeWaveform(int waveformNum) {
    for (int i = 0; i < synth.getNumVoices(); i++) {
        dynamic_cast<CustomVoice*>(synth.getVoice(i))->setWave(waveformNum);
    }
}
//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SubsynthAudioProcessor();
}
