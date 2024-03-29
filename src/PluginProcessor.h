#pragma once

#include <memory>
#include <array>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "BinaryData.h"
#include "ParamDefinitions.h"
#include "HelperStructs.h"

#define PLUGIN_CHANNEL_COUNT 2

// This is the plugin's main class
class MHVAudioProcessor final : public juce::AudioProcessor
{
// Variables
public:
    // Manages the plugin's parameters and synchronizing them with the GUI
    juce::AudioProcessorValueTreeState apvts;
private:
     // Chain element's position defined as an enum for easier access
    enum ChainPositions { PosInputGain = 0, PosConvolution, PosOutputGain, };
    // Those are only used to make the type names shorters
    using Convolution = juce::dsp::Convolution;
    using Gain = juce::dsp::Gain<float>;
    using DryWetMixer = juce::dsp::DryWetMixer<float>;
    using MonoChain = juce::dsp::ProcessorChain<Gain, Convolution, Gain>;
    // Left and right channel's signal processing chains
    std::array<MonoChain, PLUGIN_CHANNEL_COUNT> chainArray; 
    // Dry/Wet mixers
    std::array<DryWetMixer, PLUGIN_CHANNEL_COUNT> mixerArray;
    // Chain settings, used to store the current old and new settings
    // When they are intialized, they are all the same and hold the default values
    ChainSettings m_oldChainSettings;
    ChainSettings m_currentChainSettings;
    ChainSettings m_newChainSettings;
    // The plugin's parameters pointers, its's important to have it declared below the AudioProcessorValueTreeState
    const ParamPointers m_paramPointers;
    // The array with the impulse response data
    const std::array<const IRData, 3> m_IRDataArray = { IRData(BinaryData::NearIR_wav, BinaryData::NearIR_wavSize, 0 ),
                                                        IRData(BinaryData::FarIR_wav, BinaryData::FarIR_wavSize, 1),
                                                        IRData(BinaryData::WhereverIR_wav, BinaryData::WhereverIR_wavSize, 2) };
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
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
private:
    // Updates the plugin's parameters (update the DSP chain with the new parameters values)
    // The update will only happen if the new parameters values are different from the current ones
    void updateParameters(const bool forceUpdate = false);
    // Updates the current impulse response
    void updateCurrentIR(const IRData* newIRData);
    // Internal method used to process the buffer using the plugin's DSP chain
    void processBufferUsingDSP(juce::AudioBuffer<float>& buffer, const unsigned int numChannels);
    // Internal method used to apply the plugin's settings to the DSP chain
    void applyChainSettings();
    // Internal method used to prepare the DSP chains
    void prepareChains(const juce::dsp::ProcessSpec& spec);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MHVAudioProcessor)
};
