#pragma once

#include <memory>
#include <juce_audio_processors/juce_audio_processors.h>
#include "ParamDefinitions.h"

// This struct represents the plugin's data tree parameter pointers
struct ParamPointers
{
    std::atomic<float>* inputGain;
    std::atomic<float>* outputGain;
    juce::RangedAudioParameter* dryWet;
    std::atomic<float>* irIndex;

    ParamPointers(juce::AudioProcessorValueTreeState& apvts);
};

// This struct represents the plugin's settings
struct ChainSettings
{
    float inputGain = paramValue::defaultGain;
    float outputGain = paramValue::defaultGain;
    float dryWet = paramValue::defaultMix;
    int irIndex = paramValue::invalidIRIndex;

    // Comparison operator overload to measure if two ChainSettings are equal
    bool operator==(const ChainSettings& other);
    
    // Updates the settings with the new parameters values
    void updateSettings(const ParamPointers& params);
};

// This struct represents the impulse response data
struct IRData
{
    const void* const data;
    const size_t size;
    const unsigned int index;

    IRData(const void* const iRdata, const size_t iRsize, const unsigned int iRindex);
};