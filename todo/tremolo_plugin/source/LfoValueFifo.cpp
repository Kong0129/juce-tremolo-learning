namespace tremolo {
bool LfoValueFifo::push(float value) noexcept {
  int startIndex1 = 0;
  int blockSize1 = 0;
  int startIndex2 = 0;
  int blockSize2 = 0;

  fifo.prepareToWrite(1, startIndex1, blockSize1, startIndex2, blockSize2);

  if (blockSize1 == 0) {
    return false;
  }

  buffer[static_cast<std::size_t>(startIndex1)] = value;
  fifo.finishedWrite(1);
  return true;
}
int LfoValueFifo::pop(float* destination, int maximumValues) noexcept {
  if (destination == nullptr || maximumValues <= 0) {
    return 0;
  }

  int startIndex1 = 0;
  int blockSize1 = 0;
  int startIndex2 = 0;
  int blockSize2 = 0;

  fifo.prepareToRead(maximumValues, startIndex1, blockSize1, startIndex2,
                     blockSize2);

  for (int index = 0; index < blockSize1; ++index) {
    destination[index] =
        buffer[static_cast<std::size_t>(startIndex1 + index)];
  }

  for (int index = 0; index < blockSize2; ++index) {
    destination[blockSize1 + index] =
        buffer[static_cast<std::size_t>(startIndex2 + index)];
  }

  const auto valuesRead = blockSize1 + blockSize2;
  fifo.finishedRead(valuesRead);
  return valuesRead;
}
}  // namespace tremolo