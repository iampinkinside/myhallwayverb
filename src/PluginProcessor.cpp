#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "BinaryData.h"

// Those are default values for the plugin's parameters.
// They are static because we want them to be availible only to this translation unit

static const float MIN_GAIN = -24.0f;
static const float MAX_GAIN = 6.0f;
static const float DEFAULT_GAIN = 0.0f;
static const float DEFAULT_MIX = 50.0f;
static const float DEFAULT_STEP_VALUE = 0.1f;
static const float DEFAULT_SKEW_FACTOR = 1.0f;

// -- MHVAudioProcessor methods --

MHVAudioProcessor::MHVAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
{   
}

MHVAudioProcessor::~MHVAudioProcessor()
{
}

const juce::String MHVAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MHVAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MHVAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MHVAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MHVAudioProcessor::getTailLengthSeconds() const
{
    // TODO: Implement this method, return the length of the current IR maybe?
    return 0.0;
}

int MHVAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MHVAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MHVAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String MHVAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void MHVAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

// ? Why this takes an int not an unsigned int ?
void MHVAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock) 
{
    // Use this method as the place to do any pre-playback
    // Create the process specification
    juce::dsp::ProcessSpec spec;
    // Configure the process specification
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 1;
    spec.sampleRate = sampleRate;
    // Prepare the mono chains and mixer
    m_LeftChain.prepare(spec);
    m_RightChain.prepare(spec);
    m_DryWetLeft.prepare(spec);
    m_DryWetRight.prepare(spec);
    // Set the mixing rule for the dry/wet mixer
    m_DryWetLeft.setMixingRule(juce::dsp::DryWetMixer<float>::MixingRule::balanced);
    m_DryWetRight.setMixingRule(juce::dsp::DryWetMixer<float>::MixingRule::balanced);
    // Before we update the parameters, we need to set the current impulse response data
    updateCurrentIR(&m_IRDataArray[0]);
    // Update the parameters
    updateParameters();
}

void MHVAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool MHVAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void MHVAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                      juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Leaving this code here as a reminder of how to process the audio buffer manually...
    //
    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    // for (int channel = 0; channel < totalNumInputChannels; ++channel)
    // {
    //     auto* channelData = buffer.getWritePointer (channel);
    //     juce::ignoreUnused (channelData);
    //     // ..do something to the data...
    // }

    // Update the parameters
    updateParameters();
    // Process the buffer using the DSP chain
    processBufferUsingDSP(buffer);
}

bool MHVAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MHVAudioProcessor::createEditor()
{
    // TODO: Create the editor
    // return new MHVAudioProcessorEditor (*this);
    // TODO: Remove the generic editor when the custom editor is ready
    return new juce::GenericAudioProcessorEditor(*this);
}

void MHVAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void MHVAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}

// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MHVAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout MHVAudioProcessor::CreateParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>("inputGain",
                                                           "Input Gain",
                                                           juce::NormalisableRange<float>(MIN_GAIN, MAX_GAIN,DEFAULT_STEP_VALUE, DEFAULT_SKEW_FACTOR),
                                                           DEFAULT_GAIN));

    layout.add(std::make_unique<juce::AudioParameterFloat>("outputGain",
                                                           "Output Gain",
                                                           juce::NormalisableRange<float>(MIN_GAIN, MAX_GAIN, DEFAULT_STEP_VALUE, DEFAULT_SKEW_FACTOR),
                                                           DEFAULT_GAIN));

    layout.add(std::make_unique<juce::AudioParameterFloat>("dryWetMix",
                                                           "Dry/Wet",
                                                           juce::NormalisableRange<float>(0.0f, 100.0f, DEFAULT_STEP_VALUE, DEFAULT_SKEW_FACTOR),
                                                           DEFAULT_MIX));
    layout.add(std::make_unique<juce::AudioParameterChoice>("impulseResponse",
                                                            "Impulse Response",
                                                            juce::StringArray({ "Near", "Far", "Wherever you are" }),
                                                            0));                               
    return layout;
}

void MHVAudioProcessor::updateParameters()
{
    // Get the chain settings
    auto chainSettings = ChainSettings::getSettings(apvts);
    // Apply the input gain parameter
    m_LeftChain.get<ChainPositions::PosInputGain>().setGainDecibels(chainSettings.inputGain);
    m_RightChain.get<ChainPositions::PosInputGain>().setGainDecibels(chainSettings.inputGain);
    // Apply the output gain parameter
    m_LeftChain.get<ChainPositions::PosOutputGain>().setGainDecibels(chainSettings.outputGain);
    m_RightChain.get<ChainPositions::PosOutputGain>().setGainDecibels(chainSettings.outputGain);
    // Apply the dry/wet mix parameter
    m_DryWetLeft.setWetMixProportion(chainSettings.dryWet);
    m_DryWetRight.setWetMixProportion(chainSettings.dryWet);
    // Update the current impulse response if needed
    if (m_CurrentIRData->index != (unsigned int)chainSettings.irIndex)
    {
        updateCurrentIR(&m_IRDataArray[(unsigned int)chainSettings.irIndex]);
    }
}

void MHVAudioProcessor::processBufferUsingDSP(juce::AudioBuffer<float>& buffer)
{
    // Create an AudioBlock to wrap the buffer
    juce::dsp::AudioBlock<float> block(buffer);
    // Create left and right blocks
    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);
    // Push the dry samples into the chains
    m_DryWetLeft.pushDrySamples(leftBlock);
    m_DryWetRight.pushDrySamples(rightBlock);
    // Create left and right context
    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);
    // Process the left and right chains
    m_LeftChain.process(leftContext);
    m_RightChain.process(rightContext);
    // Mix the wet samples
    m_DryWetLeft.mixWetSamples(leftBlock);
    m_DryWetRight.mixWetSamples(rightBlock);
}

IRData::IRData(const void* const iRdata, const size_t iRsize, const unsigned int iRindex)
    : data(iRdata), size(iRsize), index(iRindex)
{
}

void MHVAudioProcessor::updateCurrentIR(const IRData* newIRData)
{
    // Update the current impulse response data pointer
    m_CurrentIRData = newIRData;
    // Update the impulse response data in the DSP chains
    m_LeftChain.get<ChainPositions::PosConvolution>().loadImpulseResponse(m_CurrentIRData->data,
                                                                          m_CurrentIRData->size,
                                                                          juce::dsp::Convolution::Stereo::no,
                                                                          juce::dsp::Convolution::Trim::no,
                                                                          0,
                                                                          juce::dsp::Convolution::Normalise::yes);
    m_RightChain.get<ChainPositions::PosConvolution>().loadImpulseResponse(m_CurrentIRData->data,
                                                                           m_CurrentIRData->size,
                                                                           juce::dsp::Convolution::Stereo::no,
                                                                           juce::dsp::Convolution::Trim::no,
                                                                           0,
                                                                           juce::dsp::Convolution::Normalise::yes);
}

// -- ChainSettings methods --

ChainSettings ChainSettings::getSettings(juce::AudioProcessorValueTreeState& apvts)
{
    ChainSettings settings;
    settings.inputGain = apvts.getRawParameterValue("inputGain")->load();
    settings.outputGain = apvts.getRawParameterValue("outputGain")->load();
    settings.dryWet = apvts.getParameter("dryWetMix")->getValue();
    settings.irIndex = apvts.getRawParameterValue("impulseResponse")->load();
    return settings;
}