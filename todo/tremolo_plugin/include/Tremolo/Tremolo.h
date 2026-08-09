#pragma once

namespace tremolo {
class Tremolo {
public:
  Tremolo() { lfo.setFrequency(2.0f, true); }
  void prepare(double sampleRate, int expectedMaxFramesPerBlock) {
    const juce::dsp::ProcessSpec processSpec{
        .sampleRate = sampleRate,
        .maximumBlockSize =
            static_cast<juce::uint32>(expectedMaxFramesPerBlock),
        .numChannels = 1u,
    };

    lfo.prepare(processSpec);
  }

  void setModulationDepth(float newDepth) noexcept {
    modulationDepth = juce::jlimit(0.0f, 1.0f, newDepth);
  }

  void process(juce::AudioBuffer<float>& buffer) noexcept {
    // for each frame
    for (const auto frameIndex : std::views::iota(0, buffer.getNumSamples())) {
      const auto rawLfoValue = lfo.processSample(0.0f);
      const auto lfoValue = 0.5f * (1.0f + rawLfoValue);

      const auto modulationValue =
          (1.0f - modulationDepth) + modulationDepth * lfoValue;
      // for each channel sample in the frame
      for (const auto channelIndex :
           std::views::iota(0, buffer.getNumChannels())) {
        // get the input sample
        const auto inputSample = buffer.getSample(channelIndex, frameIndex);

        // modulate the sample
        const auto outputSample = inputSample * modulationValue;

        // set the output sample
        buffer.setSample(channelIndex, frameIndex, outputSample);
      }
    }
  }

  void reset() noexcept { lfo.reset(); }

private:
  float modulationDepth = 0.5f;
  juce::dsp::Oscillator<float> lfo{
      [](float phaseValue) { return std::sin(phaseValue); }};
};
}  // namespace tremolo
