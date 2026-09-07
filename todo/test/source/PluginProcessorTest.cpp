#include <tremolo_plugin/tremolo_plugin.h>
#include <gtest/gtest.h>

namespace tremolo {

TEST(PluginProcessor, StateRoundTripRestoresParameters) {
  PluginProcessor sourceProcessor{};

  sourceProcessor.getWaveformParameter() = 1;
  sourceProcessor.getRateParameter() = 7.25f;
  sourceProcessor.getDepthParameter() = 63.3f;
  sourceProcessor.getBypassedParameter() = true;  // Arrange

  juce::MemoryBlock stateData;
  sourceProcessor.getStateInformation(stateData);

  ASSERT_GT(stateData.getSize(), 0u);

  PluginProcessor restoredProcessor{};
  restoredProcessor.setStateInformation(stateData.getData(),
                                        static_cast<int>(stateData.getSize()));  //Act

  EXPECT_EQ(restoredProcessor.getWaveformParameter().getIndex(), 1);

  EXPECT_NEAR(restoredProcessor.getRateParameter().get(), 7.25f, 1.0e-6f);
  EXPECT_NEAR(restoredProcessor.getDepthParameter().get(), 63.3f, 1.0e-6f);

  EXPECT_TRUE(restoredProcessor.getBypassedParameter().get());
}
}  // namespace tremolo
