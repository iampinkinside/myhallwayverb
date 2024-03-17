#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "BinaryData.h"

// This struct represents the plugin's settings
struct ChainSettings
{
    float inputGain;
    float outputGain;
    float dryWet;
    float irIndex;

    static ChainSettings getSettings(juce::AudioProcessorValueTreeState& apvts);
};

// This struct represents the impulse response data
struct IRData
{
    const void* const data;
    const size_t size;
    const unsigned int index;

    IRData(const void* const iRdata, const size_t iRsize, const unsigned int iRindex);
};

// This is the plugin's main class
class MHVAudioProcessor final : public juce::AudioProcessor
{
// Variables
public:
    // Manages the plugin's parameters and synchronizing them with the GUI
    juce::AudioProcessorValueTreeState apvts{*this, nullptr, "Parameters", CreateParameterLayout()};
private:
     // Chain element's position defined as an enum for easier access
    enum ChainPositions { PosInputGain = 0, PosConvolution, PosOutputGain, };
    // Those are only used to make the type names shorters
    using Convolution = juce::dsp::Convolution;
    using Gain = juce::dsp::Gain<float>;
    using DryWetMixer = juce::dsp::DryWetMixer<float>;
    using MonoChain = juce::dsp::ProcessorChain<Gain, Convolution, Gain>;
    // Left and right channel's signal processing chains
    MonoChain m_LeftChain, m_RightChain;
    // Dry/Wet mixers
    DryWetMixer m_DryWetLeft, m_DryWetRight;
    // The pointer to the current impulse response data
    IRData* m_CurrentIRData;
    // The array with the impulse response data
    IRData* m_IRDataArray[3];
// Methods
public:
  // AudioProcessor methods overrides
    // Constructor
    MHVAudioProcessor();
    // Destructor marked with override as the parent class has a virtual destructor,
    // by doing this we ensure that the destructor is called when the object is deleted
    ~MHVAudioProcessor() override;
    // This method gets called before the plugin starts processing audio
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    using AudioProcessor::processBlock;
    // This method gets called when the plugin is processing audio
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    // This method gets called when the playback stop, it can be used to release resources
    void releaseResources() override;
    // This returns true if the plugin supports the given bus layout
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    // This method creates the plugin's editor
    juce::AudioProcessorEditor* createEditor() override;
    // This method returns true as the plugin has an editor
    bool hasEditor() const override;
    // This method returns the plugin's name
    const juce::String getName() const override;
    // This method returns if the plugin accepts MIDI (it doesn't)
    bool acceptsMidi() const override;
    // This method returns if the plugin produces MIDI (it doesn't)
    bool producesMidi() const override;
    // This method returns if the plugin is a MIDI effect (it isn't)
    bool isMidiEffect() const override;
    // Miscelaneous other methods
    double getTailLengthSeconds() const override;
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
  // Custom methods    
    // Creates the plugin's parameters layout
    static juce::AudioProcessorValueTreeState::ParameterLayout CreateParameterLayout();
private:
    // Loads the binary impulse response data - call this method in the constructor
    void LoadIRBinaryData();
    // Delete binary impulse response data - call this method in the destructor
    void DeleteIRBinaryData();
    // Updates the current impulse response
    void UpdateCurrentIR(IRData* newIRData);
    // Updates the plugin's parameters (update the DSP chain with the new parameters values)
    void UpdateParameters();
    // Internal method used to process the buffer using the plugin's DSP chain
    void ProcessBufferUsingDSP(juce::AudioBuffer<float>& buffer);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MHVAudioProcessor)
};
