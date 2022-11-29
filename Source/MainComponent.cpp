#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    //Cleaner layout, code more understandable
    addAndMakeVisible         (loadButton);
    addAndMakeVisible         (loadDefaultSoundButton);
    addAndMakeVisible         (loopButton);
    addAndMakeVisible         (durationLabel);
    addAndMakeVisible         (durationMinTE);
    addAndMakeVisible         (durationMaxTE);
    addAndMakeVisible         (densityLabel);
    addAndMakeVisible         (densityTE);
    addAndMakeVisible         (grainLabel);
    addAndMakeVisible         (grainInPointFeedbackLabel);
    addAndMakeVisible         (grainOutPointFeedbackLabel);
    addAndMakeVisible         (grainDurationFeedbackLabel);
    addAndMakeVisible         (grainInPointFeedbackTE);
    addAndMakeVisible         (grainOutPointFeedbackTE);
    addAndMakeVisible         (grainDurationFeedbackTE);
    addAndMakeVisible         (granularButton);
    addAndMakeVisible         (makeGrainButton);
    addAndMakeVisible         (densityMetroButton);

    durationMinTE                  .setEditable(true);
    durationMaxTE                  .setEditable(true);
    densityTE                      .setEditable(true);

    loadButton                     .setButtonText ("Open...");
    loadDefaultSoundButton         .setButtonText ("Default");
    durationLabel                  .setText ("Duration (ms)", juce::dontSendNotification);
    durationMinTE                  .setText ("10", juce::dontSendNotification);
    durationMaxTE                  .setText ("200", juce::dontSendNotification);
    densityLabel                   .setText("Density", juce::dontSendNotification);
    densityTE                      .setText ("500", juce::dontSendNotification);
    grainLabel                     .setText("GRAIN", juce::dontSendNotification);
    grainInPointFeedbackLabel      .setText("In", juce::dontSendNotification);
    grainOutPointFeedbackLabel     .setText("Out", juce::dontSendNotification);
    grainDurationFeedbackLabel     .setText("Duration", juce::dontSendNotification);

    durationLabel                  .setJustificationType(juce::Justification::right);
    durationMinTE                  .setJustificationType(juce::Justification::right);
    durationMaxTE                  .setJustificationType(juce::Justification::right);
    densityLabel                   .setJustificationType(juce::Justification::right);
    densityTE                      .setJustificationType(juce::Justification::right);

    durationMinTE                  .setColour(juce::Label::outlineColourId, juce::Colours::darkorange);
    durationMaxTE                  .setColour(juce::Label::outlineColourId, juce::Colours::darkorange);
    durationMaxTE                  .setColour(juce::Label::outlineColourId, juce::Colours::darkorange);
    densityTE                      .setColour(juce::Label::outlineColourId, juce::Colours::darkorange);
    grainInPointFeedbackTE         .setColour(juce::Label::outlineColourId, juce::Colours::darkorange);
    grainOutPointFeedbackTE        .setColour(juce::Label::outlineColourId, juce::Colours::darkorange);
    grainDurationFeedbackTE        .setColour(juce::Label::outlineColourId, juce::Colours::darkorange);
    
    ranges.minDuration             = durationMinTE.getText().getIntValue();
    ranges.maxDuration             = durationMaxTE.getText().getIntValue();
    ranges.density                 = densityTE.getText().getIntValue();

    loadButton.onClick             = [this] { openButtonClicked(); initStartStop(); };
    loadDefaultSoundButton.onClick = [this] { loadDefaultSound(); initStartStop();  };
    loopButton.onClick             = [this] { loopButton.getToggleState() ? isLooping = true : isLooping = false; };

    durationMinTE.onTextChange     = [this] { ranges.minDuration = durationMinTE.getText().getIntValue(); };
    durationMaxTE.onTextChange     = [this] { ranges.maxDuration = durationMaxTE.getText().getIntValue(); };
    densityTE.onTextChange         = [this] { applyDensityOrMetroDensity(densityTE.getText().getIntValue()); };
    granularButton.onClick         = [this] { granularButton.getToggleState() ? isGranularizing = true : isGranularizing = false; };
    // The line below works, because ranges.minduration and ranges.maxDuration have been initialized above with related GUI elements values
    makeGrainButton.onClick        = [this] { makeGrain( ranges.minDuration, ranges.maxDuration ); };

    densityMetroButton.onClick = [this] { 
        densityMetroButton.getToggleState() ? useMetro = true : useMetro = false; 
        applyDensityOrMetroDensity(densityTE.getText().getIntValue()); 
    };

    
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (450, 100);

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (0, 2);
    }

    formatManager.registerBasicFormats();
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()
    samplingRate = sampleRate;
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!

    // For more details, see the help for AudioProcessor::getNextAudioBlock()

    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)
    bufferToFill.clearActiveBufferRegion();

    const int numSamplesInBlock = bufferToFill.buffer->getNumSamples();
    const int numSamplesInFile  = fileBuffer.getNumSamples();

    bufferToFill.buffer->clear(); // To be sure nothing is remaining in the buffer

    // return from the function if there's nothing in our buffer
    if (fileBuffer.getNumSamples() == 0) return;


    for (int sample = 0; sample < numSamplesInBlock; ++sample) 
    {
        for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel) 
        {
            const float* fileData = fileBuffer.getReadPointer (channel % fileBuffer.getNumChannels());
            float* channelData    = bufferToFill.buffer->getWritePointer (channel);
            if (filePosition > numSamplesInFile)
            {
                filePosition = numSamplesInFile;
                DBG ("filePosition trimed back to numSamplesInFile");
            }
            channelData[sample]   = fileData [filePosition];
        }
       
        if (filePosition < currentGrain.stop_sample)
        {
            ++filePosition;
        }
        else if (isLooping)
        {
            filePosition = currentGrain.stop_sample;
        }
    }
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    // You can add your drawing code here!
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    loadButton            .setBounds (10, 10, 50, 20);
    loadDefaultSoundButton.setBounds (10, 40, 50, 20);
    loopButton            .setBounds (7 , 70, 50, 20);
    durationLabel.setBounds ( 
        loadButton.getBounds().getRight() + 10, 
        loadButton.getBounds().getTopRight().getY(), 
        100, 
        20  
    );

    durationMinTE.setBounds (
        durationLabel.getBounds().getRight() + 10,
        durationLabel.getBounds().getTopLeft().getY(),
        50,
        20
    );

    durationMaxTE.setBounds (
        durationMinTE.getBounds().getRight() + 10,
        durationLabel.getBounds().getTopLeft().getY(),
        50,
        20
    );

    densityLabel.setBounds ( 
        durationLabel.getBounds().getBottomLeft().getX(), 
        durationLabel.getBounds().getBottomLeft().getY() + 5, 
        100, 
        20
    );
    densityTE.setBounds (durationMinTE.getBounds().getBottomLeft().getX(), durationMinTE.getBottom() + 5, 50, 20);

    makeGrainButton.setBounds (
        durationMaxTE.getBounds().getRight() + 20,
        durationMaxTE.getBounds().getTopLeft().getY(),
        50,
        20
    );

    densityMetroButton.setBounds(
        densityTE.getRight() + 15,
        densityTE.getBounds().getTopLeft().getY(),
        150,
        20
    );
        

    grainLabel.setBounds ((getWidth()-20)/2, densityMetroButton.getBounds().getBottom() + 20, 50, 20);
    grainInPointFeedbackTE.setBounds( grainLabel.getRight() + 5, grainLabel.getBounds().getTopLeft().getY(), 50, 20);
    //grainOutPointFeedbackLabel.setBounds();
    grainOutPointFeedbackTE.setBounds(grainInPointFeedbackTE.getRight() + 5, grainLabel.getBounds().getTopLeft().getY(), 50, 20);
    //grainDurationFeedbackLabel.setBounds();
    grainDurationFeedbackTE.setBounds(getWidth()-60, grainLabel.getBounds().getTopLeft().getY(), 50, 20);
    grainInPointFeedbackLabel.setBounds(
        grainInPointFeedbackTE.getBounds().getTopLeft().getX(), 
        grainInPointFeedbackTE.getBounds().getTopLeft().getY() - 20,
        50,
        20);
    grainOutPointFeedbackLabel.setBounds(
        grainOutPointFeedbackTE.getBounds().getTopLeft().getX(),
        grainOutPointFeedbackTE.getBounds().getTopLeft().getY() - 20,
        50,
        20);
    grainDurationFeedbackLabel.setBounds(
        grainDurationFeedbackTE.getBounds().getTopLeft().getX(),
        grainDurationFeedbackTE.getBounds().getTopLeft().getY() - 20,
        50,
        20);


}

void MainComponent::loadDefaultSound ()
{
    shutdownAudio();

    const juce::File defaultSoundFile (defaultSoundFilePath);
    if ( defaultSoundFile.existsAsFile() ) 
    {
        std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(defaultSoundFile));
        fileBuffer.setSize((int)reader->numChannels, (int)reader->lengthInSamples);  // [4]
        reader->read(&fileBuffer,                                                      // [5]
            0,                                                                //  [5.1]
            (int)reader->lengthInSamples,                                    //  [5.2]
            0,                                                                //  [5.3]
            true,                                                             //  [5.4]
            true);                                                            //  [5.5]
        filePosition = 0;                                                                   // [6]
        setAudioChannels(0, (int)reader->numChannels);
    }

}

void MainComponent::openButtonClicked()
{
    shutdownAudio();                                                                            // [1]

    chooser = std::make_unique<juce::FileChooser>("Select a Wave file shorter than 2 seconds to play...",
        juce::File{},
        "*.wav");
    auto chooserFlags = juce::FileBrowserComponent::openMode
        | juce::FileBrowserComponent::canSelectFiles;

    chooser->launchAsync(chooserFlags, [this](const juce::FileChooser& fc)
        {
            auto file = fc.getResult();

            if (file == juce::File{})
                return;

            std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file)); // [2]

            if (reader.get() != nullptr)
            {
                auto duration = (float)reader->lengthInSamples / reader->sampleRate;               // [3]

                //    if (duration < 2)
                //    {
                fileBuffer.setSize((int)reader->numChannels, (int)reader->lengthInSamples);  // [4]
                reader->read(&fileBuffer,                                                      // [5]
                    0,                                                                //  [5.1]
                    (int)reader->lengthInSamples,                                    //  [5.2]
                    0,                                                                //  [5.3]
                    true,                                                             //  [5.4]
                    true);                                                            //  [5.5]
                filePosition = 0;                                                                   // [6]
                setAudioChannels (0, (int)reader->numChannels);                                // [7]
            }
        });
}


/** Compute grain in and out point based on GUI elements.
   
    */
void MainComponent::makeGrain( int minDuration, int maxDuration )
{
    if (minDuration != maxDuration)
    {
        currentGrain.duration_ms = juce::Random::getSystemRandom().nextInt (juce::Range<int> (minDuration, maxDuration));
    }
    else
    {
        currentGrain.duration_ms = minDuration;
    }

    currentGrain.duration_sample = int( currentGrain.duration_ms * samplingRate / 1000 );

    // Prevent from having grainStartSample + grainDuration > fileBuffer.getNumSamples()
    currentGrain.start_sample = juce::Random::getSystemRandom().nextInt( fileBuffer.getNumSamples()- currentGrain.duration_sample );
    currentGrain.stop_sample  = currentGrain.start_sample + currentGrain.duration_sample;
    
    auto grainStartSampleString    = juce::String (int (currentGrain.start_sample / samplingRate * 1000));
    auto grainStopSampleString     = juce::String (int (currentGrain.stop_sample / samplingRate * 1000));
    auto grainDurationSampleString = juce::String (int (currentGrain.duration_sample / samplingRate * 1000));

    grainInPointFeedbackTE .setText (grainStartSampleString, juce::dontSendNotification);
    grainOutPointFeedbackTE.setText (grainStopSampleString, juce::dontSendNotification);
    grainDurationFeedbackTE.setText (grainDurationSampleString, juce::dontSendNotification);

    filePosition = currentGrain.start_sample;
}

void MainComponent::initStartStop()
{
    currentGrain.start_sample = 0;
    currentGrain.stop_sample = fileBuffer.getNumSamples();
}

void MainComponent::timerCallback()
{
    //DBG ("Making a new grain every " << this->density << " ms");
    makeGrain ( ranges.minDuration, ranges .maxDuration );
}

void MainComponent::applyDensityOrMetroDensity( int densityFromGUI)
{
    ranges.density = densityFromGUI;
    if (useMetro)
    {
        stopTimer();
        startTimer(ranges.density);
    }
    else
    {
        stopTimer();
    }
}