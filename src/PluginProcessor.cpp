#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "BinaryData.h"

MHVAudioProcessor::MHVAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
     apvts(*this, nullptr, "Parameters", createParameterLayout()),
     m_paramPointers(apvts)
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
    spec.maximumBlockSize = (unsigned int)samplesPerBlock;
    spec.numChannels = 1;
    spec.sampleRate = sampleRate;
    // Prepare the chains   
    prepareChains(spec);
}

void MHVAudioProcessor::prepareChains(const juce::dsp::ProcessSpec& spec)
{
    // Prepare the mono reverb chains
    for (auto& chain : chainArray)
    {
        chain.prepare(spec);
    }

    // Prepare the mono mixer chains and et the mixing rule
    for (auto& mixer : mixerArray)
    {
        mixer.prepare(spec);
        mixer.setMixingRule(juce::dsp::DryWetMixer<float>::MixingRule::balanced);
    }

    // Update the parameters
    updateParameters(true);
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

    // Process the buffer using the DSP chains, because we support only symmetric channels
    // we can safaly assume that the number of input channels is equal to the number of output channels
    processBufferUsingDSP(buffer, (unsigned int)totalNumInputChannels);
}

bool MHVAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MHVAudioProcessor::createEditor()
{
    return new MHVAudioProcessorEditor (*this);
    // Uncomment the line below and comment the line above to use the generic editor
    // return new juce::GenericAudioProcessorEditor(*this);
}

void MHVAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);
}

void MHVAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    auto tree = juce::ValueTree::readFromData(data, (size_t)sizeInBytes);
    if(tree.isValid())
    {
        apvts.replaceState(tree);
        updateParameters(true);
    }
}

// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MHVAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout MHVAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>(MHV_PID_INPUT_GAIN,
                                                           "Input Gain",
                                                           juce::NormalisableRange<float>(MHV_PV_MIN_GAIN, MHV_PV_MAX_GAIN, MHV_PV_STEP_VALUE, MHV_PV_SKEW_FACTOR),
                                                           MHV_PV_DEFAULT_GAIN));

    layout.add(std::make_unique<juce::AudioParameterFloat>(MHV_PID_OUTPUT_GAIN,
                                                           "Output Gain",
                                                           juce::NormalisableRange<float>(MHV_PV_MIN_GAIN, MHV_PV_MAX_GAIN, MHV_PV_STEP_VALUE, MHV_PV_SKEW_FACTOR),
                                                           MHV_PV_DEFAULT_GAIN));

    layout.add(std::make_unique<juce::AudioParameterFloat>(MHV_PID_DRY_WET,
                                                           "Dry/Wet",
                                                           juce::NormalisableRange<float>(0.f, 100.f, MHV_PV_STEP_VALUE, MHV_PV_SKEW_FACTOR),
                                                           MHV_PV_DEFAULT_MIX));
    layout.add(std::make_unique<juce::AudioParameterChoice>(MHV_PID_IR_INDEX,
                                                            "Impulse Response",
                                                            juce::StringArray({MHV_NEAR_STR, MHV_FAR_STR, MHV_WHEREVER_STR}),
                                                            0));                               
    return layout;
}

void MHVAudioProcessor::updateParameters(bool forceUpdate)
{
    // Get the chain settings
    m_newChainSettings.updateSettings(m_paramPointers);
    // Apply the parameters to the chains
    if(forceUpdate || !(m_newChainSettings == m_currentChainSettings))
    {
        // Get the chain settings
        m_currentChainSettings = m_newChainSettings;
        // Apply the parameters to the chains
        applyChainSettings();
        // Store the current settings as the old settings
        m_oldChainSettings = m_currentChainSettings;
    }
}

void MHVAudioProcessor::applyChainSettings()
{
    for(unsigned int i = 0; i < chainArray.size(); i++)
    {
        // Apply the input gain parameter
        chainArray[i].get<ChainPositions::PosInputGain>().setGainDecibels(m_currentChainSettings.inputGain);
        // Apply the output gain parameter
        chainArray[i].get<ChainPositions::PosOutputGain>().setGainDecibels(m_currentChainSettings.outputGain);
        // Apply the dry/wet mix parameter
        mixerArray[i].setWetMixProportion(m_currentChainSettings.dryWet);
        // Update the current impulse response if needed
        if (m_oldChainSettings.irIndex == MHV_INVALID_IR_INDEX || m_currentChainSettings.irIndex != m_oldChainSettings.irIndex)
        {
            updateCurrentIR(&m_IRDataArray[(unsigned int)m_currentChainSettings.irIndex]);
        }
    }
}

void MHVAudioProcessor::processBufferUsingDSP(juce::AudioBuffer<float>& buffer, unsigned int numChannels)
{
    // Create an AudioBlock to wrap the buffer
    juce::dsp::AudioBlock<float> block(buffer);
    // Create left and right blocks
    for(unsigned int i = 0; i < numChannels; i++)
    {
        auto channelBlock = block.getSingleChannelBlock(i);
        // Push the dry samples into the chains
        mixerArray[i].pushDrySamples(channelBlock);
        // Create left and right context
        juce::dsp::ProcessContextReplacing<float> context(channelBlock);
        // Process the left and right chains
        chainArray[i].process(context);
        // Mix the wet samples
        mixerArray[i].mixWetSamples(channelBlock);
    }
}

void MHVAudioProcessor::updateCurrentIR(const IRData* const newIRData)
{
    for(auto& chain : chainArray)
    {
        chain.get<ChainPositions::PosConvolution>().loadImpulseResponse(newIRData->data,
                                                                        newIRData->size,
                                                                        juce::dsp::Convolution::Stereo::no,
                                                                        juce::dsp::Convolution::Trim::no,
                                                                        0,
                                                                        juce::dsp::Convolution::Normalise::yes);
    }
}