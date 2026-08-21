
namespace tremolo {
PluginProcessor::PluginProcessor()
    : AudioProcessor(
          BusesProperties()
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)) {}

const juce::String PluginProcessor::getName() const {
  return TREMOLO_PLUGIN_NAME;
}

bool PluginProcessor::acceptsMidi() const {
  return false;
}

bool PluginProcessor::producesMidi() const {
  return false;
}

bool PluginProcessor::isMidiEffect() const {
  return false;
}

double PluginProcessor::getTailLengthSeconds() const {
  return 0.0;
}

int PluginProcessor::getNumPrograms() {
  // Some hosts don't cope very well if you tell them there are 0 programs, so
  // this should be at least 1, even if you're not really implementing programs.
  return 1;
}

int PluginProcessor::getCurrentProgram() {
  return 0;
}

void PluginProcessor::setCurrentProgram(int index) {
  juce::ignoreUnused(index);
}

const juce::String PluginProcessor::getProgramName(int index) {
  juce::ignoreUnused(index);
  return "None";
}

void PluginProcessor::changeProgramName(int index,
                                        const juce::String& newName) {
  juce::ignoreUnused(index, newName);
}

void PluginProcessor::prepareToPlay(double sampleRate,
                                    int expectedMaxFramesPerBlock) {
  // Use this method as the place to do any pre-playback
  // initialization that you need, e.g., allocate memory.
  tremolo.prepare(sampleRate, expectedMaxFramesPerBlock);
  const juce::dsp::ProcessSpec processSpec{
      .sampleRate = sampleRate,
      .maximumBlockSize = static_cast<juce::uint32>(expectedMaxFramesPerBlock),
      .numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels()),
  };

  bypassTransitionSmoother.prepare(processSpec);
}

void PluginProcessor::releaseResources() {
  // When playback stops, you can use this as an opportunity to free up any
  // spare memory, etc.
  bypassTransitionSmoother.reset();
  tremolo.reset();
}

bool PluginProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
  // This is the place where you check if the layout is supported.
  // In this template code we only support mono or stereo.
  // Some plugin hosts, such as certain GarageBand versions, will only
  // load plugins that support stereo bus layouts.
  if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
      layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo()) {
    return false;
  }

  // This checks if the input layout matches the output layout
  if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet()) {
    return false;
  }

  return true;
}

void PluginProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                   juce::MidiBuffer& midiMessages) {
  juce::ignoreUnused(midiMessages);

  juce::ScopedNoDenormals noDenormals;
  const auto totalNumInputChannels = getTotalNumInputChannels();
  const auto totalNumOutputChannels = getTotalNumOutputChannels();

  // In case we have more outputs than inputs, this code clears any output
  // channels that didn't contain input data, (because these aren't
  // guaranteed to be empty - they may contain garbage).
  // This is here to avoid people getting screaming feedback
  // when they first compile a plugin, but obviously you don't need to keep
  // this code if your algorithm always overwrites all the output channels.
  for (const auto channelToClear :
       std::views::iota(totalNumInputChannels, totalNumOutputChannels)) {
    buffer.clear(channelToClear, 0, buffer.getNumSamples());
  }

  // TODO: update parameters
  const auto waveform =
      static_cast<Tremolo::LfoWaveform>(parameters.waveform.getIndex());
  const auto modulationRate = parameters.rate.get();
  const auto modulationDepth = parameters.depth.get() / 100.0f;
  const auto bypassed = parameters.bypassed.get();
  tremolo.setModulationDepth(modulationDepth);
  tremolo.setLfoWaveform(waveform);
  tremolo.setModulationRate(modulationRate);

  // TODO: check for bypass
  // apply tremolo
  bypassTransitionSmoother.setBypass(bypassed);

  if (bypassTransitionSmoother.isTransitioning()) {
    bypassTransitionSmoother.setDryBuffer(buffer);
    tremolo.process(buffer);
    bypassTransitionSmoother.mixToWetBuffer(buffer);
    return;
  }

  if (!bypassed) {
    tremolo.process(buffer);
  }
}

juce::AudioProcessorParameter* PluginProcessor::getBypassParameter() const {
  return &parameters.bypassed;
}

bool PluginProcessor::hasEditor() const {
  return true;
}

// This function will be called to create an instance of the editor
juce::AudioProcessorEditor* PluginProcessor::createEditor() {
  return new PluginEditor{*this};
}

void PluginProcessor::getStateInformation(juce::MemoryBlock& destData) {
  juce::var state{new juce::DynamicObject};

  auto* stateObject = state.getDynamicObject();
  stateObject->setProperty("waveform", parameters.waveform.getIndex());
  stateObject->setProperty("rate", parameters.rate.get());
  stateObject->setProperty("depth", parameters.depth.get());
  stateObject->setProperty("bypassed", parameters.bypassed.get());

  const auto json = juce::JSON::toString(state);
  destData.replaceAll(json.toRawUTF8(), json.getNumBytesAsUTF8());
}

void PluginProcessor::setStateInformation(const void* data, int sizeInBytes) {
  if (data == nullptr || sizeInBytes <= 0) {
    return;
  }

  const auto json =
      juce::String::fromUTF8(static_cast<const char*>(data), sizeInBytes);
  const auto state = juce::JSON::parse(json);
  auto* stateObject = state.getDynamicObject();

  if (stateObject == nullptr) {
    return;
  }

  if (stateObject->hasProperty("waveform")) {
    parameters.waveform =
        static_cast<int>(stateObject->getProperty("waveform"));
  }

  if (stateObject->hasProperty("rate")) {
    parameters.rate = static_cast<float>(stateObject->getProperty("rate"));
  }

  if (stateObject->hasProperty("depth")) {
    parameters.depth = static_cast<float>(stateObject->getProperty("depth"));
  }

  if (stateObject->hasProperty("bypassed")) {
    parameters.bypassed =
        static_cast<bool>(stateObject->getProperty("bypassed"));
  }

  bypassTransitionSmoother.setBypassForced(parameters.bypassed.get());
}
}  // namespace tremolo

// This creates new instances of the plugin.
// This function definition must be in the global namespace.
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
  return new tremolo::PluginProcessor();
}
