#pragma once

namespace tremolo {
class LfoValueFifo {
public:
  bool push(float value) noexcept;
  int pop(float* destination, int maximumValues) noexcept;

private:
  static constexpr int capacity = 2048;

  std::array<float, capacity> buffer{};
  juce::AbstractFifo fifo{capacity};
};
}  // namespace tremolo