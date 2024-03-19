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
    juce::Label m_inputGainLabel;
    juce::Label m_outputGainLabel;
    juce::Label m_dryWetLabel;
    juce::Label m_inpulseLabel;
    // Those are only used to make the type names shorters
    using APTVS = juce::AudioProcessorValueTreeState;
    using SliderAttachment = APTVS::SliderAttachment;
    using ComboboxAttachment = APTVS::ComboBoxAttachment;
    // GUI components' attachments
    SliderAttachment m_inputGainAttachment;
    SliderAttachment m_outputGainAttachment;
    SliderAttachment m_dryWetAttachment;
    std::unique_ptr<ComboboxAttachment> m_inpulseComboBoxAttachment;
// Methods
public:
    explicit MHVAudioProcessorEditor (MHVAudioProcessor&);
    ~MHVAudioProcessorEditor() override;
    void paint (juce::Graphics&) override;
    void resized() override;
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MHVAudioProcessorEditor)
};
