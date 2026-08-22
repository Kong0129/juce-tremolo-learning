#pragma once

namespace tremolo {
class PluginEditor : public juce::AudioProcessorEditor {
public:
  explicit PluginEditor(PluginProcessor&);

  void resized() override;

private:
  juce::ImageComponent background;
  juce::ImageComponent logo;
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

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)
};
}  // namespace tremolo
