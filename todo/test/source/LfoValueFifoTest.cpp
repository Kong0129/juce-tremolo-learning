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
}  // namespace tremolo