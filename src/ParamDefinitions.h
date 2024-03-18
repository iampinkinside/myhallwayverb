#pragma once

// This namespace contains the names for the plugin's parameters.
// Those need to be unique and are used to identify the parameters in the code.
// They are also used to synchronize the parameters with the GUI.
namespace paramID
{
    const char* const inputGain = "inputGain";  
    const char* const outputGain = "outputGain";
    const char* const dryWet = "dryWet";
    const char* const irIndex = "irIndex";
}

// This namespace contains the values for the plugin's parameters.
// It is used to avoid hardcoding the values in other places of the code.
namespace paramValue
{
    const char * const near = "Near...";
    const char * const far = "Far...";
    const char * const wherever = "Wherever you are?";
    const float minGain = -24.0f;
    const float maxGain = 6.0f;
    const float defaultGain = 0.0f;
    const float defaultMix = 50.0f;
    const float stepValue = 0.1f;
    const float skewFactor = 1.0f;
    const float minMix = 0.0f;
    const float maxMix = 100.0f;     
}
