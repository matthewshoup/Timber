#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
class TimberAudioProcessorEditor:public juce::AudioProcessorEditor{public:explicit TimberAudioProcessorEditor(TimberAudioProcessor&);void paint(juce::Graphics&)override;void resized()override;private:TimberAudioProcessor&p;juce::Slider body,stringTone,pick,res,human;using SA=juce::AudioProcessorValueTreeState::SliderAttachment;std::vector<std::unique_ptr<SA>>at;std::vector<juce::Slider*>knobs;JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TimberAudioProcessorEditor)};
