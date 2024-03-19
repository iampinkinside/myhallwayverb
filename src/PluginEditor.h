#pragma once

#include "PluginProcessor.h"
#include "memory"

// This is the plugin's GUI class
class MHVAudioProcessorEditor final : public juce::AudioProcessorEditor
{
// Variables
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MHVAudioProcessor& processorRef;
    // GUI components
    juce::Slider m_inputGainDial;
    juce::Slider m_outputGainDial;
    juce::Slider m_dryWetSlider;
    juce::ComboBox m_inpulseComboBox;
// Methods
public:
    explicit MHVAudioProcessorEditor (MHVAudioProcessor&);
    ~MHVAudioProcessorEditor() override;
    void paint (juce::Graphics&) override;
    void resized() override;
private:
    using APTVS = juce::AudioProcessorValueTreeState;
    using SliderAttachment = APTVS::SliderAttachment;
    using ComboboxAttachment = APTVS::ComboBoxAttachment;

    SliderAttachment m_inputGainAttachment;
    SliderAttachment m_outputGainAttachment;
    SliderAttachment m_dryWetAttachment;
    std::unique_ptr<ComboboxAttachment> m_inpulseComboBoxAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MHVAudioProcessorEditor)
};
