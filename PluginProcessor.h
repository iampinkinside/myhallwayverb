#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "BinaryData.h"

struct ChainSettings
{
    float inputGain;
    float outputGain;
    float dryWet;

    static ChainSettings getSettings(juce::AudioProcessorValueTreeState& apvts);
};

//==============================================================================
class MHVAudioProcessor final : public juce::AudioProcessor
{
// --- BOILERPLATE JUCE CODE ---
public:
    //==============================================================================
    MHVAudioProcessor();
    ~MHVAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MHVAudioProcessor)

// --- PLUGIN-SPECIFIC CODE ---
public:
// - VARIABLES -
    // Manages the plugin's parameters and synchronizing them with the GUI
    juce::AudioProcessorValueTreeState apvts{*this, nullptr, "Parameters", CreateParameterLayout()};
private:
    // Those are only used to make the code more readable
    using Convolution = juce::dsp::Convolution;
    using Gain = juce::dsp::Gain<float>;
    using DryWetMixer = juce::dsp::DryWetMixer<float>;
    using MonoChain = juce::dsp::ProcessorChain<Gain, Convolution, Gain>;
    // Chain element's position
    enum ChainPositions
    {
        PosInputGain = 0,
        PosConvolution,
        PosOutputGain,
        PosDryWet
    };
    // Left and right channel's signal processing chains
    MonoChain m_LeftChain, m_RightChain;
    // Dry/Wet mixers
    DryWetMixer m_DryWetLeft, m_DryWetRight;
    // The impulse response binary data
    const void* m_ImpulseResponseData = BinaryData::SmallHall_aif;
    // The impulse response data size
    const size_t m_ImpulseResponseDataSize = BinaryData::SmallHall_aifSize;
public:
// - METHODS -
    // Creates the plugin's parameters layout
    static juce::AudioProcessorValueTreeState::ParameterLayout CreateParameterLayout();
};
