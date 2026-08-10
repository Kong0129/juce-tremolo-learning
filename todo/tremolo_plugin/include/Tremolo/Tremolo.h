#pragma once
#include <array>

namespace tremolo {
class Tremolo {
public:
  enum class LfoWaveform : std::size_t {
    sine = 0,
    triangle = 1,
  };

  Tremolo() {
    for (auto& oscillator : lfos) {
      oscillator.setFrequency(2.0f, true);
    }
  }
  void prepare(double sampleRate, int expectedMaxFramesPerBlock) {
    const juce::dsp::ProcessSpec processSpec{
        .sampleRate = sampleRate,
        .maximumBlockSize =
            static_cast<juce::uint32>(expectedMaxFramesPerBlock),
        .numChannels = 1u,
    };

    for (auto& oscillator : lfos) {
      oscillator.prepare(processSpec);
    }
  }

  void setModulationDepth(float newDepth) noexcept {
    modulationDepth = juce::jlimit(0.0f, 1.0f, newDepth);
  }

  void process(juce::AudioBuffer<float>& buffer) noexcept {
    // for each frame
    for (const auto frameIndex : std::views::iota(0, buffer.getNumSamples())) {
      const auto lfoIndex = juce::toUnderlyingType(currentLfo);
      const auto rawLfoValue = lfos[lfoIndex].processSample(0.0f);
      const auto modulationValue = 1.0f + modulationDepth * rawLfoValue;

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

  void reset() noexcept {
    for (auto& oscillator : lfos) {
      oscillator.reset();
    }
  }

private:
  float modulationDepth = 0.4f;

  static float triangle(float phaseValue) noexcept {
    const auto ft = phaseValue / juce::MathConstants<float>::twoPi;

    return 4.0f * std::abs(ft - std::floor(ft + 0.5f)) - 1.0f;
  }
  std::array<juce::dsp::Oscillator<float>, 2> lfos{
      juce::dsp::Oscillator<float>{
          [](float phaseValue) { return std::sin(phaseValue); }},
      juce::dsp::Oscillator<float>{triangle},

  };

  LfoWaveform currentLfo = LfoWaveform::sine;
};

}  // namespace tremolo
