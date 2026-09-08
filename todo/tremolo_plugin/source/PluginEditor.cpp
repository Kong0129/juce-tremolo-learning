namespace tremolo {

void LfoIndicator::setValue(float newValue) {
  const auto targetValue = juce::jlimit(-1.0f, 1.0f, newValue);
  constexpr auto smoothingFactor = 0.5f;

  value += smoothingFactor * (targetValue - value);
  repaint();
}
void LfoIndicator::paint(juce::Graphics& graphics) {
  const auto bounds = getLocalBounds().toFloat().reduced(4.0f);

  graphics.setColour(juce::Colours::darkgrey);
  graphics.fillEllipse(bounds);

  constexpr auto indicatorDiameter = 12.0f;
  const auto normalizedValue = 0.5f * (1.0f + value);

  const auto indicatorX =
      bounds.getX() + normalizedValue * (bounds.getWidth() - indicatorDiameter);

  const auto indicatorY = bounds.getCentreY() - indicatorDiameter / 2.0f;

  graphics.setColour(juce::Colours::white);
  graphics.fillEllipse(indicatorX, indicatorY, indicatorDiameter,
                       indicatorDiameter);
}
void LfoWaveformDisplay::pushValue(float newValue) {
  for (std::size_t index = 1; index < history.size(); ++index) {
    history[index - 1] = history[index];
  }

  history.back() = juce::jlimit(-1.0f, 1.0f, newValue);
  repaint();
}
void LfoWaveformDisplay::paint(juce::Graphics& graphics) {
  const auto bounds = getLocalBounds().toFloat().reduced(4.0f);

  graphics.setColour(juce::Colours::darkgrey);
  graphics.fillRoundedRectangle(bounds, 4.0f);

  graphics.setColour(juce::Colours::grey);
  graphics.drawLine(bounds.getX(), bounds.getCentreY(), bounds.getRight(),
                    bounds.getCentreY(), 1.0f);

  juce::Path waveformPath;

  for (std::size_t index = 0; index < history.size(); ++index) {
    const auto normalizedIndex =
        static_cast<float>(index) /
        static_cast<float>(history.size() - 1);

    const auto x = bounds.getX() + normalizedIndex * bounds.getWidth();
    const auto y =
        bounds.getY() + 0.5f * (1.0f - history[index]) * bounds.getHeight();

    if (index == 0) {
      waveformPath.startNewSubPath(x, y);
    } else {
      waveformPath.lineTo(x, y);
    }
  }

  graphics.setColour(juce::Colours::white);
  graphics.strokePath(waveformPath,
                      juce::PathStrokeType{2.0f});
}

PluginEditor::PluginEditor(PluginProcessor& p)
    : AudioProcessorEditor(&p),
      processor(p),
      waveformAttachment(p.getWaveformParameter(), waveformSelector, nullptr),
      rateAttachment(p.getRateParameter(), rateSlider, nullptr),
      depthAttachment(p.getDepthParameter(), depthSlider, nullptr),
      bypassButtonAttachment(p.getBypassedParameter(), bypassButton, nullptr) {
  waveformLabel.setText("Waveform", juce::dontSendNotification);
  rateLabel.setText("Rate", juce::dontSendNotification);
  depthLabel.setText("Depth", juce::dontSendNotification);
  background.setImage(juce::ImageCache::getFromMemory(
      assets::Background_png, assets::Background_pngSize));

  logo.setImage(
      juce::ImageCache::getFromMemory(assets::Logo_png, assets::Logo_pngSize));

  addAndMakeVisible(background);
  addAndMakeVisible(logo);
  addAndMakeVisible(lfoIndicator);
  addAndMakeVisible(lfoWaveformDisplay);
  waveformSelector.addItem("Sine", 1);
  waveformSelector.addItem("Triangle", 2);
  waveformAttachment.sendInitialUpdate();
  waveformLabel.setJustificationType(juce::Justification::centred);
  rateLabel.setJustificationType(juce::Justification::centred);
  depthLabel.setJustificationType(juce::Justification::centred);
  addAndMakeVisible(waveformLabel);
  addAndMakeVisible(waveformSelector);
  rateSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  rateSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
  rateSlider.setTextValueSuffix(" Hz");
  depthSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  depthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
  depthSlider.setTextValueSuffix(" %");
  addAndMakeVisible(rateSlider);
  addAndMakeVisible(rateLabel);
  addAndMakeVisible(depthLabel);
  addAndMakeVisible(depthSlider);
  bypassButton.setButtonText("Bypass");
  addAndMakeVisible(bypassButton);

  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.
  setSize(540, 270);
  startTimerHz(60);
}

void PluginEditor::timerCallback() {
  const auto currentLfoValue = processor.getCurrentLfoValue();

  lfoIndicator.setValue(currentLfoValue);
  lfoWaveformDisplay.pushValue(currentLfoValue);
}

void PluginEditor::resized() {
  const auto bounds = getLocalBounds();

  background.setBounds(bounds);

  logo.setBounds({16, 16, 105, 24});

  lfoIndicator.setBounds(440, 120, 80, 80);

  lfoWaveformDisplay.setBounds(16, 160, 400, 90);

  waveformSelector.setBounds(16, 64, 160, 32);

  rateSlider.setBounds(200, 48, 100, 100);

  depthSlider.setBounds(320, 48, 100, 100);

  bypassButton.setBounds(440, 64, 80, 32);

  waveformLabel.setBounds(16, 40, 160, 24);
  rateLabel.setBounds(200, 24, 100, 24);
  depthLabel.setBounds(320, 24, 100, 24);
}
}  // namespace tremolo
