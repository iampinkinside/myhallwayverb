#include "PluginProcessor.h"
#include "PluginEditor.h"

// --- JUCE BOILERPLATE CODE ---

//==============================================================================
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

//==============================================================================
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

//==============================================================================
void MHVAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::ignoreUnused (sampleRate, samplesPerBlock);
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

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        juce::ignoreUnused (channelData);
        // ..do something to the data...
    }
}

//==============================================================================
bool MHVAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MHVAudioProcessor::createEditor()
{
    return new MHVAudioProcessorEditor (*this);
    // return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
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

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MHVAudioProcessor();
}

// --- PLUGIN-SPECIFIC CODE ---

// - CONSTANTS -
// Those are default values for the plugin's parameters.
// They are static because we want them to be availible only to this translation unit
static const float MIN_GAIN = -24.0f;
static const float MAX_GAIN = 6.0f;
static const float DEFAULT_GAIN = 0.0f;
static const float DEFAULT_MIX = 50.0f;
static const float DEFAULT_STEP_VALUE = 0.1f;
static const float DEFAULT_SKEW_FACTOR = 1.0f;

// - METHODS -
juce::AudioProcessorValueTreeState::ParameterLayout MHVAudioProcessor::CreateParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>("inputGain",
                                                           "Input Gain",
                                                           juce::NormalisableRange<float>(MIN_GAIN,
                                                                                          MAX_GAIN,
                                                                                          DEFAULT_STEP_VALUE,
                                                                                          DEFAULT_SKEW_FACTOR),
                                                           DEFAULT_GAIN));

    layout.add(std::make_unique<juce::AudioParameterFloat>("outputGain",
                                                           "Output Gain",
                                                           juce::NormalisableRange<float>(MIN_GAIN,
                                                                                          MAX_GAIN,
                                                                                          DEFAULT_STEP_VALUE,
                                                                                          DEFAULT_SKEW_FACTOR),
                                                           DEFAULT_GAIN));

    layout.add(std::make_unique<juce::AudioParameterFloat>("dryWetMix",
                                                           "Dry/Wet",
                                                           juce::NormalisableRange<float>(0.0f,
                                                                                          100.0f,
                                                                                          DEFAULT_STEP_VALUE,
                                                                                          DEFAULT_SKEW_FACTOR),
                                                           DEFAULT_MIX));                                    
    return layout;
}
