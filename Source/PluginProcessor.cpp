/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "spleeter_common/spleeter_common.h"

//==============================================================================
SpleetervstAudioProcessor::SpleetervstAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                         .withInput("Input", AudioChannelSet::stereo(), true)
#endif
                         .withOutput("Output", AudioChannelSet::stereo(), true)
#endif
                         )
#endif
      ,
      m_filter(nullptr), m_buffer(nullptr), m_vocals_volume(1.0),
      m_bass_volume(1.0), m_drums_volume(1.0), m_piano_volume(1.0),
      m_other_volume(1.0) {

  std::error_code err;
  auto models_path =
      File::getSpecialLocation(File::SpecialLocationType::currentExecutableFile)
          .getParentDirectory()
#ifdef OSX
          .getParentDirectory()
          .getChildFile("Resources")
#endif  // OSX
          .getChildFile("models")
          .getFullPathName();

  spleeter::Initialize(models_path.toStdString(),
                       {spleeter::SeparationType::FiveStems}, err);
  m_filter =
      std::make_shared<spleeter::Filter>(spleeter::SeparationType::FiveStems);
  m_filter->set_extra_frame_latency(10);  // TODO: might be a lot...
  m_filter->Init(err);
}

SpleetervstAudioProcessor::~SpleetervstAudioProcessor() {}

//==============================================================================
const String SpleetervstAudioProcessor::getName() const {
  return JucePlugin_Name;
}

bool SpleetervstAudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool SpleetervstAudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool SpleetervstAudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
  return true;
#else
  return false;
#endif
}

double SpleetervstAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int SpleetervstAudioProcessor::getNumPrograms() {
  return 1; // NB: some hosts don't cope very well if you tell them there are 0
            // programs,
  // so this should be at least 1, even if you're not really implementing
  // programs.
}

int SpleetervstAudioProcessor::getCurrentProgram() { return 0; }

void SpleetervstAudioProcessor::setCurrentProgram(int index) {}

const String SpleetervstAudioProcessor::getProgramName(int index) { return {}; }

void SpleetervstAudioProcessor::changeProgramName(int index,
                                                  const String &newName) {}

//==============================================================================
void SpleetervstAudioProcessor::prepareToPlay(double sampleRate,
                                              int samplesPerBlock) {
  // resample to 44100Hz
  m_in_interpolator.clear();
  m_out_interpolator.clear();
  for (auto channel_idx = 0; channel_idx < getTotalNumInputChannels(); channel_idx++) {
    m_in_interpolator.push_back(LagrangeInterpolator());
    m_out_interpolator.push_back(LagrangeInterpolator());
  }
  auto desired_interpolation_ratio = 44100 / sampleRate;
  int block_size = samplesPerBlock * desired_interpolation_ratio;
  m_interpolation_ratio = static_cast<float>(block_size) / samplesPerBlock;
  
  // Initialize the buffer
  m_filter->set_volume(0, m_vocals_volume);
  m_filter->set_volume(1, m_drums_volume);
  m_filter->set_volume(2, m_bass_volume);
  m_filter->set_volume(3, m_piano_volume);
  m_filter->set_volume(4, m_other_volume);
  m_filter->set_block_size(block_size);
  m_buffer = std::make_shared<rtff::AudioBuffer>(block_size, 2);
  
  // Latency
  setLatencySamples(m_filter->FrameLatency() * (1.0 / m_interpolation_ratio));
}

void SpleetervstAudioProcessor::setVocalsVolume(double value) {
  m_vocals_volume = value;
  if (m_filter) {
    m_filter->set_volume(0, m_vocals_volume);
  }
}
double SpleetervstAudioProcessor::getVocalsVolume() const {
  return m_vocals_volume;
}
void SpleetervstAudioProcessor::setBassVolume(double value) {
  m_bass_volume = value;
  if (m_filter) {
    m_filter->set_volume(2, m_bass_volume);
  }
}
double SpleetervstAudioProcessor::getBassVolume() const {
  return m_bass_volume;
}
void SpleetervstAudioProcessor::setDrumsVolume(double value) {
  m_drums_volume = value;
  if (m_filter) {
    m_filter->set_volume(1, m_drums_volume);
  }
}
double SpleetervstAudioProcessor::getDrumsVolume() const {
  return m_drums_volume;
}
void SpleetervstAudioProcessor::setPianoVolume(double value) {
  m_piano_volume = value;
  if (m_filter) {
    m_filter->set_volume(3, m_piano_volume);
  }
}
double SpleetervstAudioProcessor::getPianoVolume() const {
  return m_piano_volume;
}
void SpleetervstAudioProcessor::setOtherVolume(double value) {
  m_other_volume = value;
  if (m_filter) {
    m_filter->set_volume(4, m_other_volume);
  }
}
double SpleetervstAudioProcessor::getOtherVolume() const {
  return m_other_volume;
}

void SpleetervstAudioProcessor::releaseResources() {
  // When playback stops, you can use this as an opportunity to free up any
  // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SpleetervstAudioProcessor::isBusesLayoutSupported(
    const BusesLayout &layouts) const {
#if JucePlugin_IsMidiEffect
  ignoreUnused(layouts);
  return true;
#else
  // This is the place where you check if the layout is supported.
  // In this template code we only support mono or stereo.
  if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono() &&
      layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
    return false;

// This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
  if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
    return false;
#endif

  return true;
#endif
}
#endif

void SpleetervstAudioProcessor::processBlock(AudioBuffer<float> &buffer,
                                             MidiBuffer &midiMessages) {
  ScopedNoDenormals noDenormals;
  auto totalNumInputChannels = getTotalNumInputChannels();
  auto totalNumOutputChannels = getTotalNumOutputChannels();

  // In case we have more outputs than inputs, this code clears any output
  // channels that didn't contain input data, (because these aren't
  // guaranteed to be empty - they may contain garbage).
  // This is here to avoid people getting screaming feedback
  // when they first compile a plugin, but obviously you don't need to keep
  // this code if your algorithm always overwrites all the output channels.
  for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    buffer.clear(i, 0, buffer.getNumSamples());

  // This is the place where you'd normally do the guts of your plugin's
  // audio processing...
  // Make sure to reset the state if your inner loop is processing
  // the samples and the outer loop is handling the channels.
  // Alternatively, you can process the samples with the channels
  // interleaved by keeping the same state.
  
  
  // TODO: factorize !
  if (totalNumInputChannels == 2) {
    for (int channel = 0; channel < totalNumInputChannels; ++channel) {
      float *channelData = buffer.getWritePointer(channel);
      m_in_interpolator[channel].process(1.0 / m_interpolation_ratio, channelData, m_buffer->data(channel), m_buffer->frame_count(), buffer.getNumSamples(), 0);
//      memcpy(m_buffer->data(channel), channelData, m_buffer->frame_count() * sizeof(float));
    }
  } else {
    // For any other case than stereo, sum in the first channel
    // -- cleanup
    memset(m_buffer->data(0), 0, m_buffer->frame_count() * sizeof(float));
    memset(m_buffer->data(1), 0, m_buffer->frame_count() * sizeof(float));

    for (int channel = 0; channel < totalNumInputChannels; ++channel) {
      // -- interpolate input data in channel 0
      float *channelData = buffer.getWritePointer(channel);
      m_in_interpolator[channel].process(1.0 / m_interpolation_ratio, channelData, m_buffer->data(0), m_buffer->frame_count(), buffer.getNumSamples(), 0);
      // -- store the sum in channel 1
      Eigen::Map<Eigen::VectorXf>(m_buffer->data(1), m_buffer->frame_count()).array() += Eigen::Map<Eigen::VectorXf>(m_buffer->data(0), m_buffer->frame_count()).array();
    }
    // divide by the number of channels
    Eigen::Map<Eigen::VectorXf>(m_buffer->data(1), m_buffer->frame_count()).array() /= totalNumInputChannels;
    // copy channel 1 in others
    Eigen::Map<Eigen::VectorXf>(m_buffer->data(0), m_buffer->frame_count()) - Eigen::Map<Eigen::VectorXf>(m_buffer->data(1), m_buffer->frame_count());
  }
  
  // convert to stereo
  m_filter->ProcessBlock(m_buffer.get());
  
  if (totalNumInputChannels == 2) {
    for (int channel = 0; channel < totalNumInputChannels; ++channel) {
      float *channelData = buffer.getWritePointer(channel);
      m_out_interpolator[channel].process(m_interpolation_ratio, m_buffer->data(channel), channelData, buffer.getNumSamples(), m_buffer->frame_count(), 0);
      
//      memcpy(channelData, m_buffer->data(channel), m_buffer->frame_count() * sizeof(float));
    }
  } else {
    // TODO: move to another location
    // For any other case than stereo, sum the result in channel 0 and divide by two
    Eigen::Map<Eigen::VectorXf>(m_buffer->data(0), m_buffer->frame_count()).array() += Eigen::Map<Eigen::VectorXf>(m_buffer->data(1), m_buffer->frame_count()).array();
    Eigen::Map<Eigen::VectorXf>(m_buffer->data(0), m_buffer->frame_count()).array() /= 2;
    // Copy in each channel
    for (int channel = 0; channel < totalNumInputChannels; ++channel) {
      float *channelData = buffer.getWritePointer(channel);
      m_out_interpolator[channel].process(m_interpolation_ratio, m_buffer->data(0), channelData, buffer.getNumSamples(), m_buffer->frame_count(), 0);
    }
  }
}

//==============================================================================
bool SpleetervstAudioProcessor::hasEditor() const {
  return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor *SpleetervstAudioProcessor::createEditor() {
  return new SpleetervstAudioProcessorEditor(*this);
}

//==============================================================================
void SpleetervstAudioProcessor::getStateInformation(MemoryBlock &destData) {
  // You should use this method to store your parameters in the memory block.
  // You could do that either as raw data, or use the XML or ValueTree classes
  // as intermediaries to make it easy to save and load complex data.
}

void SpleetervstAudioProcessor::setStateInformation(const void *data,
                                                    int sizeInBytes) {
  // You should use this method to restore your parameters from this memory
  // block,
  // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new SpleetervstAudioProcessor();
}
