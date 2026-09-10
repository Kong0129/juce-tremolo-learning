#include <tremolo_plugin/tremolo_plugin.h>
#include <gtest/gtest.h>

namespace tremolo {
TEST(LfoValueFifo, PreservesInsertionOrder) {
  LfoValueFifo fifo;

  EXPECT_TRUE(fifo.push(0.1f));
  EXPECT_TRUE(fifo.push(0.2f));
  EXPECT_TRUE(fifo.push(0.3f));

  std::array<float, 3> output{};
  const auto valuesRead =
      fifo.pop(output.data(), static_cast<int>(output.size()));

  ASSERT_EQ(valuesRead, 3);
  EXPECT_FLOAT_EQ(output[0], 0.1f);
  EXPECT_FLOAT_EQ(output[1], 0.2f);
  EXPECT_FLOAT_EQ(output[2], 0.3f);
}

TEST(LfoValueFifo, EmptyFifoReturnsZero) {
  LfoValueFifo fifo;
  std::array<float, 3> output{};

  const auto valuesRead =
      fifo.pop(output.data(), static_cast<int>(output.size()));

  EXPECT_EQ(valuesRead, 0);
}

TEST(LfoValueFifo, LimitedReadLeavesRemainingValue) {
  LfoValueFifo fifo;

  ASSERT_TRUE(fifo.push(0.1f));
  ASSERT_TRUE(fifo.push(0.2f));
  ASSERT_TRUE(fifo.push(0.3f));

  std::array<float, 2> firstOutput{};
  const auto firstValuesRead =
      fifo.pop(firstOutput.data(), static_cast<int>(firstOutput.size()));

  ASSERT_EQ(firstValuesRead, 2);
  EXPECT_FLOAT_EQ(firstOutput[0], 0.1f);
  EXPECT_FLOAT_EQ(firstOutput[1], 0.2f);

  std::array<float, 1> secondOutput{};
  const auto secondValuesRead =
      fifo.pop(secondOutput.data(), static_cast<int>(secondOutput.size()));

  ASSERT_EQ(secondValuesRead, 1);
  EXPECT_FLOAT_EQ(secondOutput[0], 0.3f);
}

TEST(LfoValueFifo, FullFifoRejectsNewValue) {
  LfoValueFifo fifo;
  constexpr auto usableCapacity = 2047;

  for (int index = 0; index < usableCapacity; ++index) {
    ASSERT_TRUE(fifo.push(static_cast<float>(index)));
  }

  EXPECT_FALSE(fifo.push(2047.0f));
}

TEST(LfoValueFifo, WrapAroundPreservesOrder) {
  LfoValueFifo fifo;
  constexpr auto usableCapacity = 2047;

  for (int index = 0; index < usableCapacity; ++index) {
    ASSERT_TRUE(fifo.push(static_cast<float>(index)));
  }

  std::array<float, 2040> discardedValues{};
  const auto discardedCount = fifo.pop(
      discardedValues.data(), static_cast<int>(discardedValues.size()));
  ASSERT_EQ(discardedCount, 2040);

  for (int value = 2047; value < 2055; ++value) {
    ASSERT_TRUE(fifo.push(static_cast<float>(value)));
  }

  std::array<float, 15> output{};
  const auto valuesRead =
      fifo.pop(output.data(), static_cast<int>(output.size()));

  ASSERT_EQ(valuesRead, 15);

  for (int index = 0; index < valuesRead; ++index) {
    EXPECT_FLOAT_EQ(output[static_cast<std::size_t>(index)],
                    static_cast<float>(2040 + index));
  }
}
}  // namespace tremolo
