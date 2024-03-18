#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ParamDefinitions.h"

// The dimensions of the background image
const int jpg_width = 263;
const int jpg_height = 400;


//==============================================================================
MHVAudioProcessorEditor::MHVAudioProcessorEditor (MHVAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p),
    m_inputGainAttachment(p.apvts, paramID::inputGain, m_inputGainDial),
    m_outputGainAttachment(p.apvts, paramID::outputGain, m_outputGainDial),
    m_dryWetAttachment(p.apvts, paramID::dryWet, m_dryWetSlider),
    m_inpulseComboBoxAttachment(p.apvts, paramID::irIndex, m_inpulseComboBox)
{
    m_inputGainDial.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    m_inputGainDial.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(m_inputGainDial);

    m_outputGainDial.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    m_outputGainDial.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(m_outputGainDial);

    m_dryWetSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    m_dryWetSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(m_dryWetSlider);

    m_inpulseComboBox.addItem(paramValue::near, 1);
    m_inpulseComboBox.addItem(paramValue::far, 2);
    m_inpulseComboBox.addItem(paramValue::wherever, 3);

    m_inpulseComboBox.setSelectedId (1);

    addAndMakeVisible(m_inpulseComboBox);

    m_inputGainDial.onValueChange = [this] { processorRef.updateParameters(); };
    m_outputGainDial.onValueChange = [this] { processorRef.updateParameters(); };
    m_inpulseComboBox.onChange = [this] { processorRef.updateParameters(); };
    m_dryWetSlider.onValueChange = [this] { processorRef.updateParameters(); };

    setSize(jpg_width, jpg_height);
}

MHVAudioProcessorEditor::~MHVAudioProcessorEditor()
{
}

//==============================================================================
void MHVAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    // g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    // g.setColour (juce::Colours::white);
    juce::Image background = juce::ImageCache::getFromMemory(BinaryData::hallway_jpg, BinaryData::hallway_jpgSize);
    g.drawImageAt (background, 0, 0);
    g.setFont (15.0f);
    // g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void MHVAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto border = 2;
    auto area = getLocalBounds();
    // Don't worry about the magic numbers, they are not reused anywhere else
    // Also, don't worry about the implicit type conversion, it's safe here
    // First, we remove the top 80% of the area
    area.removeFromTop(area.getHeight() * 0.8);
    // Then remove 25% of the width from the left and right sides
    auto inputGainArea = area.removeFromLeft(area.getWidth() / 4).reduced(border);
    auto outputGainArea  = area.removeFromRight(area.getWidth() * 0.33);
    // Finally, we remove the top 33% of the middle area
    area.removeFromTop(area.getHeight() * 0.33);
    auto sliderArea = area.removeFromTop(area.getHeight() * 0.5).reduced(border);
    // After the area was removed, we can set the middle area
    auto middleArea = area.reduced(border);
    // Now we set the bounds of the components
    m_inputGainDial.setBounds(inputGainArea);
    m_inpulseComboBox.setBounds(middleArea);
    m_outputGainDial.setBounds(outputGainArea);
    m_dryWetSlider.setBounds(sliderArea);
    // And we set the size of the combobox
    m_inpulseComboBox.setSize(middleArea.getWidth(), middleArea.getHeight() * 0.9);
}
