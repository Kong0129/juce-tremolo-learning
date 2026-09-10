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

TEST(PluginProcessor, InvalidStateLeavesParametersUnchanged) {
  // Arrange
  PluginProcessor processor{};

  processor.getWaveformParameter() = 1;
  processor.getRateParameter() = 7.25f;
  processor.getDepthParameter() = 63.3f;
  processor.getBypassedParameter() = true;

  constexpr char invalidState[] = "this is not valid JSON";

  // Act
  processor.setStateInformation(invalidState,
                                static_cast<int>(sizeof(invalidState) - 1));

  // Assert
  EXPECT_EQ(processor.getWaveformParameter().getIndex(), 1);
  EXPECT_NEAR(processor.getRateParameter().get(), 7.25f, 1.0e-6f);
  EXPECT_NEAR(processor.getDepthParameter().get(), 63.3f, 1.0e-6f);
  EXPECT_TRUE(processor.getBypassedParameter().get());
}
}  // namespace tremolo
