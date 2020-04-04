/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include <list>
#include <utility>

#include "PluginProcessor.h"

//==============================================================================
/**
*/
class SpleetervstAudioProcessorEditor : public AudioProcessorEditor {
public:
  SpleetervstAudioProcessorEditor(SpleetervstAudioProcessor &);
  ~SpleetervstAudioProcessorEditor();

  //==============================================================================
  void paint(Graphics &) override;
  void resized() override;

private:
  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  void UpdateVolumes();
  
  SpleetervstAudioProcessor &processor;
  Slider vocals_volume_slider;
  Slider drums_volume_slider;
  Slider bass_volume_slider;
  Slider piano_volume_slider;
  Slider other_volume_slider;
  
  std::list<std::pair<std::string, Slider*>> volume_sliders;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpleetervstAudioProcessorEditor)
};
