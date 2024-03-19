#include "HelperStructs.h"

IRData::IRData(const void* const iRdata, const size_t iRsize, const unsigned int iRindex)
    : data(iRdata), size(iRsize), index(iRindex)
{
}

void ChainSettings::updateSettings(const ParamPointers& params)
{
    // Get the input and output gain as raw values as we'll be setting them using decibels
    inputGain = params.inputGain->load();
    outputGain = params.outputGain->load();
    // Get the dry/wet mix as a normalised value as this is what the mixer expects
    dryWet = params.dryWet->getValue();
    // Get the impulse response raw value and cast it to an unsigned 
    irIndex = (int)params.irIndex->load();
}

bool ChainSettings::operator==(const ChainSettings& other)
{
    return (juce::approximatelyEqual(inputGain, other.inputGain) &&
            juce::approximatelyEqual(outputGain, other.outputGain) &&
            juce::approximatelyEqual(dryWet, other.dryWet) &&
            irIndex == other.irIndex);
}

ParamPointers::ParamPointers(juce::AudioProcessorValueTreeState& apvts)
    : inputGain(apvts.getRawParameterValue(paramID::inputGain)),
      outputGain(apvts.getRawParameterValue(paramID::outputGain)),
      dryWet(apvts.getParameter(paramID::dryWet)),
      irIndex(apvts.getRawParameterValue(paramID::irIndex))
{
}