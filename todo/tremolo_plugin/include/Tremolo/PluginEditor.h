#pragma once
#include <array>

namespace tremolo {
class LfoIndicator : public juce::Component {
public:
  void setValue(float newValue);
  void paint(juce::Graphics& graphics) override;

private:
  float value = 0.0f;
};

class LfoWaveformDisplay : public juce::Component {
public:
  void pushValue(float newValue);
  void paint(juce::Graphics& graphics) override;

private:
  static constexpr std::size_t historySize = 120;
  std::array<float, historySize> history{};
};

class PluginEditor : public juce::AudioProcessorEditor, private juce::Timer {
public:
  explicit PluginEditor(PluginProcessor&);

  void resized() override;

private:
  juce::ImageComponent background;
  juce::ImageComponent logo;
  LfoIndicator lfoIndicator;
  LfoWaveformDisplay lfoWaveformDisplay;

  PluginProcessor& processor;

  juce::Label waveformLabel;
  juce::ComboBox waveformSelector;
  juce::ComboBoxParameterAttachment waveformAttachment;

  juce::Label rateLabel;
  juce::Slider rateSlider;
  juce::SliderParameterAttachment rateAttachment;

  juce::Label depthLabel;
  juce::Slider depthSlider;
  juce::SliderParameterAttachment depthAttachment;

  juce::ToggleButton bypassButton;
  juce::ButtonParameterAttachment bypassButtonAttachment;

  void timerCallback() override;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)
};
}  // namespace tremolo
