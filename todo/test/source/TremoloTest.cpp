#include "TestUtils.h"
#include <gtest/gtest.h>
#include <tremolo_plugin/tremolo_plugin.h>

#include <wolfsound/file/wolfsound_WavFileWriter.hpp>
#include <wolfsound/dsp/wolfsound_testSignals.hpp>

namespace tremolo {
namespace {
void extractLfo(Tremolo& tremolo, juce::AudioBuffer<float>& bufferToUse) {
  juce::dsp::AudioBlock<float> block{bufferToUse};
  block.fill(1.f);
  tremolo.process(bufferToUse);
  block.subtract(1.f);
}
}  // namespace

/** This test extracts the LFO used by the Tremolo effect and saves it to a WAV
 * file "lfo.wav".
 *
 * You can find the file in the same folder where the test executable resides,
 * most probably [CMake binary dir]/test/.
 *
 * The LFO rate is not changed; thus, the one set in Tremolo's constructor will
 * be used.
 */
TEST(Tremolo, ExtractLfo) {
  for (const auto lfoWaveform :
       {Tremolo::LfoWaveform::sine, Tremolo::LfoWaveform::triangle}) {
    Tremolo testee;
    constexpr auto sampleRate = 48000.0;
    testee.setLfoWaveform(lfoWaveform);
    testee.prepare(sampleRate, static_cast<int>(sampleRate));

    juce::AudioBuffer<float> buffer;
    buffer.setSize(1, static_cast<int>(sampleRate));

    extractLfo(testee, buffer);

    const auto* const fileName = lfoWaveform == Tremolo::LfoWaveform::sine
                                     ? "sineLfo.wav"
                                     : "triangleLfo.wav";

    wolfsound::WavFileWriter::writeToFile(
        getFileOutputPath(fileName),
        juce::Span{buffer.getReadPointer(0),
                   static_cast<size_t>(buffer.getNumSamples())},
        wolfsound::Frequency{sampleRate});
  }
}

/** This test extracts the LFO used by the Tremolo effect switching the LFO
 * shape mid-processing, and saves it to a WAV file "smoothedLfo.wav".
 *
 * You can find the file in the same folder where the test executable resides,
 * most probably [CMake binary dir]/test/.
 *
 * The LFO rate is not changed; thus, the one set in Tremolo's constructor will
 * be used.
 */
TEST(Tremolo, LfoWaveformTransitionIsSmooth) {
  Tremolo testee;
  constexpr auto sampleRate = 48000.0;
  constexpr auto channelCount = 1;
  constexpr auto blockSizeSeconds = 1;
  constexpr auto blockSizeSamples =
      static_cast<int>(blockSizeSeconds * sampleRate);
  testee.prepare(sampleRate, blockSizeSamples);

  juce::AudioBuffer<float> outputBuffer;
  constexpr auto recordingLengthSamples = 2 * blockSizeSamples;
  outputBuffer.setSize(channelCount, recordingLengthSamples);

  juce::AudioBuffer<float> processBuffer;
  processBuffer.setSize(channelCount, blockSizeSamples);

  testee.setLfoWaveform(Tremolo::LfoWaveform::sine);
  extractLfo(testee, processBuffer);
  outputBuffer.copyFrom(0, 0, processBuffer, 0, 0, blockSizeSamples);
  testee.setLfoWaveform(Tremolo::LfoWaveform::triangle);
  extractLfo(testee, processBuffer);
  outputBuffer.copyFrom(0, blockSizeSamples, processBuffer, 0, 0,
                        blockSizeSamples);

  wolfsound::WavFileWriter::writeToFile(
      getFileOutputPath("smoothedLfo.wav"),
      juce::Span{outputBuffer.getReadPointer(0),
                 static_cast<size_t>(outputBuffer.getNumSamples())},
      wolfsound::Frequency{sampleRate});
}

TEST(Tremolo, ZeroDepthLeavesStereoFrameUnchanged) {
  Tremolo tremolo;
  tremolo.prepare(48000.0, 1);
  tremolo.setModulationDepth(0.0f);
  tremolo.reset();

  juce::AudioBuffer<float> buffer;
  buffer.setSize(2, 1);
  buffer.setSample(0, 0, 0.25f);
  buffer.setSample(1, 0, -0.5f);

  tremolo.process(buffer);

  const auto actualOutput0 = buffer.getSample(0, 0);
  const auto actualOutput1 = buffer.getSample(1, 0);

  EXPECT_NEAR(actualOutput0, 0.25f, 1.0e-6f);
  EXPECT_NEAR(actualOutput1, -0.5f, 1.0e-6f);
}

TEST(Tremolo, DepthReachesZeroAfterSmoothingTime) {
  Tremolo tremolo;
  constexpr auto sampleRate = 1000.0;
  constexpr auto smoothingSampleCount = 20;

  tremolo.prepare(sampleRate, smoothingSampleCount);
  tremolo.setLfoWaveform(Tremolo::LfoWaveform::triangle);
  tremolo.setModulationDepth(0.0f);

  juce::AudioBuffer<float> warmupBuffer;
  warmupBuffer.setSize(1, smoothingSampleCount);
  warmupBuffer.clear();

  tremolo.process(warmupBuffer);

  juce::AudioBuffer<float> verificationBuffer;
  verificationBuffer.setSize(1, 1);
  verificationBuffer.setSample(0, 0, 0.25f);

  tremolo.process(verificationBuffer);

  const auto actualOutput = verificationBuffer.getSample(0, 0);
  EXPECT_NEAR(actualOutput, 0.25f, 1.0e-6f);
}
}  // namespace tremolo
