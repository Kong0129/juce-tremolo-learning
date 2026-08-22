#pragma once

namespace tremolo {
class LfoIndicator : public juce::Component {
public:
  void setValue(float newValue);
  void paint(juce::Graphics& graphics) override;

private:
  float value = 0.0f;
};

class PluginEditor : public juce::AudioProcessorEditor, private juce::Timer {
public:
  explicit PluginEditor(PluginProcessor&);

  void resized() override;

private:
  juce::ImageComponent background;
  juce::ImageComponent logo;
  LfoIndicator lfoIndicator;
  float testLfoValue = -1.0f;
  float testLfoDirection = 1.0f;

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
