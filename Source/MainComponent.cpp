#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    /*
    // First basic layout
    addAndMakeVisible (loadButton);
    loadButton.setButtonText ("Open...");
    loadButton.onClick = [this] { openButtonClicked(); };

    addAndMakeVisible (loadDefaultSoundButton);
    loadDefaultSoundButton.setButtonText ("Load default");
    loadDefaultSoundButton.onClick = [this] { loadDefaultSound(); };
    
    addAndMakeVisible (loopButton);
    loopButton.onClick = [this] { loopButton.getToggleState() ? isLooping = true : isLooping = false; };
    */

    

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

    durationMinTE             .setEditable(true);
    durationMaxTE             .setEditable(true);
    densityTE                 .setEditable(true);

    loadButton                .setButtonText ("Open...");
    loadDefaultSoundButton    .setButtonText ("Default");
    durationLabel             .setText ("Duration (ms)", juce::dontSendNotification);
    durationMinTE             .setText ("10", juce::dontSendNotification);
    durationMaxTE             .setText ("200", juce::dontSendNotification);
    densityLabel              .setText("Density", juce::dontSendNotification);
    densityTE                 .setText ("500", juce::dontSendNotification);
    grainLabel                .setText("GRAIN", juce::dontSendNotification);
    grainInPointFeedbackLabel .setText("In", juce::dontSendNotification);
    grainOutPointFeedbackLabel.setText("Out", juce::dontSendNotification);
    grainDurationFeedbackLabel.setText("Duration", juce::dontSendNotification);

    durationLabel              .setJustificationType(juce::Justification::right);
    durationMinTE              .setJustificationType(juce::Justification::right);
    durationMaxTE              .setJustificationType(juce::Justification::right);
    densityLabel               .setJustificationType(juce::Justification::right);
    densityTE                  .setJustificationType(juce::Justification::right);

    durationMinTE              .setColour(juce::Label::outlineColourId, juce::Colours::darkorange);
    durationMaxTE              .setColour(juce::Label::outlineColourId, juce::Colours::darkorange);
    durationMaxTE              .setColour(juce::Label::outlineColourId, juce::Colours::darkorange);
    densityTE                  .setColour(juce::Label::outlineColourId, juce::Colours::darkorange);
    grainInPointFeedbackTE     .setColour(juce::Label::outlineColourId, juce::Colours::darkorange);
    grainOutPointFeedbackTE    .setColour(juce::Label::outlineColourId, juce::Colours::darkorange);
    grainDurationFeedbackTE    .setColour(juce::Label::outlineColourId, juce::Colours::darkorange);
    
    minDuration                    = durationMinTE.getText().getIntValue();
    maxDuration                    = durationMaxTE.getText().getIntValue();
    density                        = densityTE.getText().getIntValue();

    loadButton.onClick = [this] { openButtonClicked(); initStartStop(); };
    loadDefaultSoundButton.onClick = [this] { loadDefaultSound(); initStartStop();  };
    loopButton.onClick             = [this] { loopButton.getToggleState() ? isLooping = true : isLooping = false; };

    // loopButton.getToggleState() ? isLooping = true : isLooping = false;
    // equivalent to
    // if( loopButton.getToggleState() )
    //    isLooping = true;
    // else
    //    isLooping = false;


    durationMinTE.onTextChange     = [this] { minDuration = durationMinTE.getText().getIntValue(); };
    durationMaxTE.onTextChange     = [this] { maxDuration = durationMaxTE.getText().getIntValue(); };
    densityTE.onTextChange         = [this] { applyDensityOrMetroDensity(densityTE.getText().getIntValue()); };
    granularButton.onClick         = [this] { granularButton.getToggleState() ? isGranularizing = true : isGranularizing = false; };
    makeGrainButton.onClick        = [this] { makeGrain(); };

    densityMetroButton.onClick = [this] { 
        densityMetroButton.getToggleState() ? useMetro = true : useMetro = false; 
        applyDensityOrMetroDensity(densityTE.getText().getIntValue()); 
    };

    //thumbnailHorizontalPosition = 100;

    // Make sure you set the size of the component after
    // you add any child components.
    setSize (450, 300);

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
        setAudioChannels (2, 2);
    }

    formatManager.registerBasicFormats();
    //transportSource.addChangeListener(this);
    //thumbnail.addChangeListener(this);

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
/*
        if (filePosition < numSamplesInFile) 
        {
            ++filePosition;
        }
        else if (isLooping) 
        {
            filePosition = 0;
        }
*/        
        if (filePosition < grainStopSample)
        {
            ++filePosition;
        }
        else if (isLooping)
        {
            // was : 
            // filePosition = 0;
            filePosition = grainStartSample;
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
 /*   g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    
    juce::Rectangle<int> thumbnailBounds(10, thumbnailHorizontalPosition, getWidth() - 20, getHeight() - 120);

    if (thumbnail.getNumChannels() == 0)
        paintIfNoFileLoaded(g, thumbnailBounds);
    else
        paintIfFileLoaded(g, thumbnailBounds);
*/
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
        //thumbnail.setSource(new juce::FileInputSource(defaultSoundFile));
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
                //thumbnail.setSource (new juce::FileInputSource(file));

            }
        });
}
/*
void MainComponent::thumbnailChanged()
{
    repaint();
}

void MainComponent::paintIfNoFileLoaded(juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds)
{
    g.setColour(juce::Colours::darkgrey);
    g.fillRect(thumbnailBounds);
    g.setColour(juce::Colours::white);
    g.drawFittedText("No File Loaded", thumbnailBounds, juce::Justification::centred, 1);
}

void MainComponent::paintIfFileLoaded(juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds)
{
    g.setColour(juce::Colours::white);
    g.fillRect(thumbnailBounds);

    g.setColour(juce::Colours::red);                               // [8]

    thumbnail.drawChannels(g,                                      // [9]
        thumbnailBounds,
        0.0,                                    // start time
        thumbnail.getTotalLength(),             // end time
        1.0f);                                  // vertical zoom
}

void MainComponent::changeListenerCallback(juce::ChangeBroadcaster* source)
{
//    if (source == &transportSource) transportSourceChanged();
    if (source == &thumbnail)       thumbnailChanged();
}
*/
/** Compute grain in and out point based on GUI elements.
   
    */
void MainComponent::makeGrain()
{
    auto grainDuration = 0;
    if (minDuration != maxDuration)
    {
        grainDuration = juce::Random::getSystemRandom().nextInt (juce::Range<int> (minDuration, maxDuration));
    }
    else
    {
        grainDuration = minDuration;
    }

    grainDurationSample = int( grainDuration * samplingRate / 1000 );

    // Prevent from having grainStartSample + grainDuration > fileBuffer.getNumSamples()
    grainStartSample = juce::Random::getSystemRandom().nextInt(fileBuffer.getNumSamples()-grainDuration);
    grainStopSample = grainStartSample + grainDurationSample;
    
    auto grainStartSampleString    = juce::String (int (grainStartSample / samplingRate * 1000));
    auto grainStopSampleString     = juce::String (int (grainStopSample / samplingRate * 1000));
    auto grainDurationSampleString = juce::String (grainDuration);

    grainInPointFeedbackTE .setText (grainStartSampleString, juce::dontSendNotification);
    grainOutPointFeedbackTE.setText (grainStopSampleString, juce::dontSendNotification);
    grainDurationFeedbackTE.setText (grainDurationSampleString, juce::dontSendNotification);

    filePosition = grainStartSample;
}

void MainComponent::initStartStop()
{
    grainStartSample = 0;
    grainStopSample = fileBuffer.getNumSamples(); 
}

void MainComponent::timerCallback()
{
    //DBG ("Making a new grain every " << this->density << " ms");
    makeGrain ();
}

void MainComponent::applyDensityOrMetroDensity( int densityFromGUI)
{
    this->density = densityFromGUI;
    if (useMetro)
    {
        stopTimer();
        startTimer(density);
    }
    else
    {
        stopTimer();
    }
}