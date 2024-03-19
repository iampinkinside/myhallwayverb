#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ParamDefinitions.h"

// The dimensions of the background image
#define JPG_WIDTH 263
#define JPG_HEIGHT 400


//==============================================================================
MHVAudioProcessorEditor::MHVAudioProcessorEditor (MHVAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p),
    m_inputGainAttachment(p.apvts, MHV_PID_INPUT_GAIN, m_inputGainDial),
    m_outputGainAttachment(p.apvts, MHV_PID_OUTPUT_GAIN, m_outputGainDial),
    m_dryWetAttachment(p.apvts, MHV_PID_DRY_WET, m_dryWetSlider)
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

    m_inpulseComboBox.addItem(MHV_NEAR_STR, 1);
    m_inpulseComboBox.addItem(MHV_FAR_STR, 2);
    m_inpulseComboBox.addItem(MHV_WHEREVER_STR, 3);

    // Because the ComboBoxAttachment updates the combobox for the first time when it's created
    // we need to create the attachment after the combobox was filled with items
    m_inpulseComboBoxAttachment = std::make_unique<ComboboxAttachment>(p.apvts, MHV_PID_IR_INDEX, m_inpulseComboBox);
    addAndMakeVisible(m_inpulseComboBox);

    // Take care of the labels
    m_inputGainLabel.setText("Input Gain", juce::dontSendNotification);
    m_inputGainLabel.attachToComponent(&m_inputGainDial, false);
    addAndMakeVisible(m_inputGainLabel);

    m_outputGainLabel.setText("Output Gain", juce::dontSendNotification);
    m_outputGainLabel.attachToComponent(&m_outputGainDial, false);
    addAndMakeVisible(m_outputGainLabel);

    m_dryWetLabel.setText("Dry/Wet Mix", juce::dontSendNotification);
    m_dryWetLabel.attachToComponent(&m_dryWetSlider, false);
    addAndMakeVisible(m_dryWetLabel);

    m_inpulseLabel.setText("Impulse Response", juce::dontSendNotification);
    m_inpulseLabel.attachToComponent(&m_inpulseComboBox, false);
    addAndMakeVisible(m_inpulseLabel);

    // Set the size of the editor
    setSize(JPG_WIDTH, JPG_HEIGHT);
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
    area.removeFromTop(area.getHeight() / 20);
    auto comboBoxArea = area.removeFromTop(area.getHeight() * 0.8).reduced(border);
    // Then remove 25% of the width from the left and right sides
    auto inputGainArea = area.removeFromLeft(area.getWidth() / 4).reduced(border);
    auto outputGainArea  = area.removeFromRight(area.getWidth() * 0.33);
    // Finally, we remove the top 33% of the middle area
    area.removeFromTop(area.getHeight() * 0.66);
    // After the area was removed, we can set the middle area
    auto sliderArea = area.reduced(border);
    // Now we set the bounds of the components
    m_inputGainDial.setBounds(inputGainArea);
    m_inpulseComboBox.setBounds(comboBoxArea);
    m_outputGainDial.setBounds(outputGainArea);
    m_dryWetSlider.setBounds(sliderArea);
    // And we set the size of the combobox
    m_inpulseComboBox.setSize(comboBoxArea.getWidth(), comboBoxArea.getHeight() / 12);
}
