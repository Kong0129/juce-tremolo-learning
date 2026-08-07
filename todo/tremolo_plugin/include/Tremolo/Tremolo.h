#pragma once

namespace tremolo {
class Tremolo {
public:
  void prepare(double sampleRate, int expectedMaxFramesPerBlock) {
    juce::ignoreUnused(expectedMaxFramesPerBlock);

    constexpr double lfoRateHz = 2.0;

    phase = 0.0;
    phaseIncrement =
        juce::MathConstants<double>::twoPi * lfoRateHz / sampleRate;
  }

  void process(juce::AudioBuffer<float>& buffer) noexcept {
    // for each frame
    for (const auto frameIndex : std::views::iota(0, buffer.getNumSamples())) {
      const auto lfoValue = 0.5 * (1.0 + std::sin(phase));

      const auto modulationValue = static_cast<float>(lfoValue);
      // for each channel sample in the frame
      for (const auto channelIndex :
           std::views::iota(0, buffer.getNumChannels())) {
        // get the input sample
        const auto inputSample = buffer.getSample(channelIndex, frameIndex);

        // TODO: modulate the sample
        const auto outputSample = inputSample * modulationValue;

        // set the output sample
        buffer.setSample(channelIndex, frameIndex, outputSample);
      }
      phase += phaseIncrement;
      if (phase >= juce::MathConstants<double>::twoPi) {
        phase -= juce::MathConstants<double>::twoPi;
      }
    }
  }

  void reset() noexcept { phase = 0.0; }

private:
  double phase = 0.0;
  double phaseIncrement = 0.0;
};
}  // namespace tremolo
