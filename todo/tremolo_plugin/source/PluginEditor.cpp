namespace tremolo {
PluginEditor::PluginEditor(PluginProcessor& p)
    : AudioProcessorEditor(&p),
      waveformAttachment(p.getWaveformParameter(), waveformSelector, nullptr),
      rateAttachment(p.getRateParameter(), rateSlider, nullptr) {
  background.setImage(juce::ImageCache::getFromMemory(
      assets::Background_png, assets::Background_pngSize));

  logo.setImage(
      juce::ImageCache::getFromMemory(assets::Logo_png, assets::Logo_pngSize));

  addAndMakeVisible(background);
  addAndMakeVisible(logo);
  waveformSelector.addItem("Sine", 1);
  waveformSelector.addItem("Triangle", 2);
  waveformAttachment.sendInitialUpdate();
  addAndMakeVisible(waveformSelector);
  rateSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  rateSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
  rateSlider.setTextValueSuffix(" Hz");
  addAndMakeVisible(rateSlider);
  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.
  setSize(540, 270);
}

void PluginEditor::resized() {
  const auto bounds = getLocalBounds();

  background.setBounds(bounds);

  logo.setBounds({16, 16, 105, 24});

  waveformSelector.setBounds(16, 64, 160, 32);

  rateSlider.setBounds(200, 48, 100, 100);
}
}  // namespace tremolo
