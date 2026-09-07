#pragma once
#include <array>

namespace tremolo {
class Tremolo {
public:
  enum class LfoWaveform : std::size_t {
    sine = 0,
    triangle = 1,
  };

  float getCurrentLfoValue() const noexcept { return currentLfoValue; }

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
    modulationDepth.reset(sampleRate, 0.02);
    waveformMix.reset(sampleRate, 0.01);
  }

  void setLfoWaveform(LfoWaveform newWaveform) noexcept {
    const auto isValidWaveform = newWaveform == LfoWaveform::sine ||
                                 newWaveform == LfoWaveform::triangle;

    jassert(isValidWaveform);

    if (!isValidWaveform) {
      return;
    }

    lfoToSet = newWaveform;
  }

  void setModulationDepth(float newDepth) noexcept {
    modulationDepth.setTargetValue(juce::jlimit(0.0f, 1.0f, newDepth));
  }

  void setModulationRate(float newRate) noexcept {
    for (auto& oscillator : lfos) {
      oscillator.setFrequency(newRate);
    }
  }

  void process(juce::AudioBuffer<float>& buffer) noexcept {
    updateLfoWaveform();
    // for each frame
    for (const auto frameIndex : std::views::iota(0, buffer.getNumSamples())) {
      const auto sineLfoValue =
          lfos[juce::toUnderlyingType(LfoWaveform::sine)].processSample(0.0f);

      const auto triangleLfoValue =
          lfos[juce::toUnderlyingType(LfoWaveform::triangle)].processSample(
              0.0f);

      const auto currentWaveformMix = waveformMix.getNextValue();

      const auto rawLfoValue = (1.0f - currentWaveformMix) * sineLfoValue +
                               currentWaveformMix * triangleLfoValue;
      currentLfoValue = rawLfoValue;
      const auto smoothedModulationDepth = modulationDepth.getNextValue();

      const auto modulationValue = 1.0f + smoothedModulationDepth * rawLfoValue;

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
    modulationDepth.setCurrentAndTargetValue(modulationDepth.getTargetValue());
    const auto currentMix = currentLfo == LfoWaveform::sine ? 0.0f : 1.0f;

    waveformMix.setCurrentAndTargetValue(currentMix);
    currentLfoValue = 0.0f;
  }

private:
  float currentLfoValue = 0.0f;
  juce::LinearSmoothedValue<float> modulationDepth{0.4f};
  juce::LinearSmoothedValue<float> waveformMix{0.0f};

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
  LfoWaveform lfoToSet = LfoWaveform::sine;
  void updateLfoWaveform() noexcept {
    if (currentLfo != lfoToSet) {
      currentLfo = lfoToSet;

      const auto targetMix = currentLfo == LfoWaveform::sine ? 0.0f : 1.0f;

      waveformMix.setTargetValue(targetMix);
    }
  }
};

}  // namespace tremolo
