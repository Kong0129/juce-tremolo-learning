namespace tremolo {

void LfoIndicator::setValue(float newValue) {
  value = juce::jlimit(-1.0f, 1.0f, newValue);
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

PluginEditor::PluginEditor(PluginProcessor& p)
    : AudioProcessorEditor(&p),
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
  startTimerHz(30);
}

void PluginEditor::timerCallback() {
  testLfoValue += 0.1f * testLfoDirection;

  if (testLfoValue >= 1.0f) {
    testLfoValue = 1.0f;
    testLfoDirection = -1.0f;
  } else if (testLfoValue <= -1.0f) {
    testLfoValue = -1.0f;
    testLfoDirection = 1.0f;
  }

  lfoIndicator.setValue(testLfoValue);
}

void PluginEditor::resized() {
  const auto bounds = getLocalBounds();

  background.setBounds(bounds);

  logo.setBounds({16, 16, 105, 24});

  lfoIndicator.setBounds(440, 120, 80, 80);

  waveformSelector.setBounds(16, 64, 160, 32);

  rateSlider.setBounds(200, 48, 100, 100);

  depthSlider.setBounds(320, 48, 100, 100);

  bypassButton.setBounds(440, 64, 80, 32);

  waveformLabel.setBounds(16, 40, 160, 24);
  rateLabel.setBounds(200, 24, 100, 24);
  depthLabel.setBounds(320, 24, 100, 24);
}
}  // namespace tremolo
