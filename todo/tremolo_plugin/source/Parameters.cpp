
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
juce::AudioParameterFloat& createRateParameter(
    juce::AudioProcessor& processor) {
  auto parameter = std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"modulation.rate", 1}, "Rate",
      juce::NormalisableRange<float>{0.1f, 20.0f, 0.01f, 0.4f}, 2.0f, "Hz");

  auto& parameterReference = *parameter;
  processor.addParameter(parameter.release());

  return parameterReference;
}
juce::AudioParameterFloat& createDepthParameter(
    juce::AudioProcessor& processor) {
  auto parameter = std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"modulation.depth", 1}, "Depth",
      juce::NormalisableRange<float>{0.0f, 100.0f, 0.1f}, 40.0f, "%");

  auto& parameterReference = *parameter;
  processor.addParameter(parameter.release());

  return parameterReference;
}
juce::AudioParameterBool& createBypassParameter(
    juce::AudioProcessor& processor) {
  auto parameter = std::make_unique<juce::AudioParameterBool>(
      juce::ParameterID{"bypass", 1}, "Bypass", false);

  auto& parameterReference = *parameter;
  processor.addParameter(parameter.release());

  return parameterReference;
}
}  // namespace
Parameters::Parameters(juce::AudioProcessor& processor)
    : waveform(createWaveformParameter(processor)),
      rate(createRateParameter(processor)),
      depth(createDepthParameter(processor)),
      bypassed(createBypassParameter(processor)) {}
}  // namespace tremolo
