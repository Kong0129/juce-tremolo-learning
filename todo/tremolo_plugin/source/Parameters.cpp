
namespace tremolo {
namespace {
juce::AudioParameterChoice& createWaveformParameter(
    juce::AudioProcessor& processor) {
  auto parameter = std::make_unique<juce::AudioParameterChoice>(
      juce::ParameterID{"modulation.waveform", 1}, "Waveform",
      juce::StringArray{"Sine", "Triangle"}, 0);

  auto& parameterReference = *parameter;
  processor.addParameter(parameter.release());

  return parameterReference;
}
}  // namespace
Parameters::Parameters(juce::AudioProcessor& processor)
    : waveform(createWaveformParameter(processor)) {}
}  // namespace tremolo
