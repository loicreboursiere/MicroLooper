#include "MainComponent.h"
#include <iostream>

//==============================================================================
MainComponent::MainComponent()
{
    //Cleaner layout, code more understandable
    addAndMakeVisible(loadButton);
    addAndMakeVisible(loadDefaultSoundButton);
    addAndMakeVisible(loopButton);
    addAndMakeVisible(durationLabel);
    addAndMakeVisible(durationMinTE);
    addAndMakeVisible(durationMaxTE);
    addAndMakeVisible(densityLabel);
    addAndMakeVisible(densityTE);
    addAndMakeVisible(grainLabel);
    addAndMakeVisible(grainInPointFeedbackLabel);
    addAndMakeVisible(grainOutPointFeedbackLabel);
    addAndMakeVisible(grainDurationFeedbackLabel);
    addAndMakeVisible(grainInPointFeedbackTE);
    addAndMakeVisible(grainOutPointFeedbackTE);
    addAndMakeVisible(grainDurationFeedbackTE);
    addAndMakeVisible(granularButton);
    addAndMakeVisible(makeGrainButton);
    addAndMakeVisible(densityMetroButton);
    addAndMakeVisible(stereoButton);
    addAndMakeVisible(stereoSlider);
    addAndMakeVisible(sinusButton);
    addAndMakeVisible(titleLabel);
    addAndMakeVisible(textPannerLabel);
    addAndMakeVisible(lfoModeLabel);
    addAndMakeVisible(stereoRandomButton);
    addAndMakeVisible(sliderButton);
    addAndMakeVisible(sinusButton);





    durationMinTE.setEditable(true);
    durationMaxTE.setEditable(true);
    densityTE.setEditable(true);

    durationLabel.setText("Duration (ms)", juce::dontSendNotification);
    durationMinTE.setText("10", juce::dontSendNotification);
    durationMaxTE.setText("200", juce::dontSendNotification);
    densityLabel.setText("Density", juce::dontSendNotification);
    densityTE.setText("500", juce::dontSendNotification);
    grainLabel.setText("GRAIN", juce::dontSendNotification);
    grainInPointFeedbackLabel.setText("In", juce::dontSendNotification);
    grainOutPointFeedbackLabel.setText("Out", juce::dontSendNotification);
    grainDurationFeedbackLabel.setText("Duration", juce::dontSendNotification);
    titleLabel.setText("Granular Synthesis Panner", juce::dontSendNotification);
    textPannerLabel.setText("Left      .      .      .      .      .      .      .      Right", juce::dontSendNotification);
    lfoModeLabel.setText("MODES :", juce::dontSendNotification);

    titleLabel.setFont(juce::Font(20.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::darkorange);

    stereoSlider.setRange(-100, 100);
    stereoSlider.setTextBoxStyle(juce::Slider::NoTextBox, false,0,0);

    


    durationLabel.setJustificationType(juce::Justification::right);
    durationMinTE.setJustificationType(juce::Justification::right);
    durationMaxTE.setJustificationType(juce::Justification::right);
    densityLabel.setJustificationType(juce::Justification::right);
    densityTE.setJustificationType(juce::Justification::right);
    titleLabel.setJustificationType(juce::Justification::right);
    textPannerLabel.setJustificationType(juce::Justification::centred);
    lfoModeLabel.setJustificationType(juce::Justification::right);

    durationMinTE.setColour(juce::Label::outlineColourId, juce::Colours::darkorange);
    durationMaxTE.setColour(juce::Label::outlineColourId, juce::Colours::darkorange);
    densityTE.setColour(juce::Label::outlineColourId, juce::Colours::darkorange);
    grainInPointFeedbackTE.setColour(juce::Label::outlineColourId, juce::Colours::darkorange);
    grainOutPointFeedbackTE.setColour(juce::Label::outlineColourId, juce::Colours::darkorange);
    grainDurationFeedbackTE.setColour(juce::Label::outlineColourId, juce::Colours::darkorange);


    ranges.minDuration = durationMinTE.getText().getIntValue();
    ranges.maxDuration = durationMaxTE.getText().getIntValue();
    ranges.density = densityTE.getText().getIntValue();

    loadButton.onClick = [this] { openButtonClicked(); initStartStop(); };
    loadDefaultSoundButton.onClick = [this] { loadDefaultSound(); initStartStop();  };
    loopButton.onClick = [this] { loopButton.getToggleState() ? isLooping = true : isLooping = false; };

    stereoButton.onClick = [this] { stereoButton.getToggleState() ? isLeft = false : isLeft = true; };

    durationMinTE.onTextChange = [this] { ranges.minDuration = durationMinTE.getText().getIntValue(); };
    durationMaxTE.onTextChange = [this] { ranges.maxDuration = durationMaxTE.getText().getIntValue(); };
    densityTE.onTextChange = [this] { applyDensityOrMetroDensity(densityTE.getText().getIntValue()); };
    granularButton.onClick = [this] { granularButton.getToggleState() ? isGranularizing = true : isGranularizing = false; };
    stereoRandomButton.onClick = [this] { stereoRandomButton.getToggleState() ? isRandomStereo = true : isRandomStereo = false; };

    // The line below works, because ranges.minduration and ranges.maxDuration have been initialized above with related GUI elements values
    makeGrainButton.onClick = [this] { makeGrain(ranges.minDuration, ranges.maxDuration); };

    densityMetroButton.onClick = [this] {
        densityMetroButton.getToggleState() ? useMetro = true : useMetro = false;
        applyDensityOrMetroDensity(densityTE.getText().getIntValue());
    };

    stereoSlider.onValueChange = [this] {stereoFrame.position = stereoSlider.getValue(); };

    sinusButton.onClick =        [this] {sinusButton.getToggleState() ? isSinus = true : isSinus = false; };

    sliderButton.onClick =       [this] {sliderButton.getToggleState() ? isSliderOn = true : isSliderOn = false; };
    

        
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (700, 300);

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

    const int numSamplesInBlock = bufferToFill.buffer->getNumSamples();//taille du buffer
    const int numSamplesInFile  = fileBuffer.getNumSamples();//taille du .wav

    bufferToFill.buffer->clear(); // To be sure nothing is remaining in the buffer

    // return from the function if there's nothing in our buffer
    if (fileBuffer.getNumSamples() == 0) return;

    if (isSliderOn) panning(stereoFrame.position);
 
    if (isSinus);
    
    for (int sample = 0; sample < numSamplesInBlock; ++sample) 
    {     
        const float* fileData;
        float* channelData;    


        for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel) 
        {
            fileData = fileBuffer.getReadPointer(channel % fileBuffer.getNumChannels());
            channelData = bufferToFill.buffer->getWritePointer(channel);
            switch (channel) {
            case 0: 
                channelData[sample] = fileData[filePosition]*stereoFrame.leftSample;
                break;
            case 1:
                channelData[sample] = fileData[filePosition]*stereoFrame.rightSample;
                break;
            }  
        }

        if (filePosition < currentGrain.stop_sample)
        {
            ++filePosition;
        }

        if (filePosition > numSamplesInFile)
        {
            filePosition = numSamplesInFile;
            DBG("filePosition trimed back to numSamplesInFile");
        }

        else if (isLooping)
        {
            filePosition = currentGrain.stop_sample;
        }
    }
}

float degreesToRadians(float degs) noexcept {
    return (3.1415 / 180.0f) * degs;
}

void MainComponent::panning(float position)
{
    if (isSinus) position = sineWaveLFO(position);

    position /= 100;
    
    double theta = position * 45.0;
    theta = degreesToRadians(theta);
    
    stereoFrame.leftSample = ((sqrt(2)/2) * (sin(theta) - cos(theta)));
    stereoFrame.rightSample = ((sqrt(2)/2) * (sin(theta) + cos(theta)));
    
    //DBG(stereoFrame.leftSample << "   " << stereoFrame.rightSample);
}

float MainComponent::sineWaveLFO(float position)
{
    float cyclesPerSample, currentSample, angleDelta;

    cyclesPerSample = position / sineWave.sampleRate;
    angleDelta = cyclesPerSample * 2.0 * juce::MathConstants<double>::pi;

    sineWave.currentAngle += angleDelta;
    currentSample = position*(float)std::sin(sineWave.currentAngle);

    DBG(currentSample);
    return currentSample;
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
    loadButton            .setBounds (10, 30, 50, 20);
    loadDefaultSoundButton.setBounds (10, 60, 50, 20);
    loopButton            .setBounds (7 , 90, 50, 20);
    durationLabel.setBounds ( 
        getWidth() - 400,
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
    stereoRandomButton.setBounds(
        densityMetroButton.getBounds().getTopLeft().getX(),
        densityMetroButton.getBounds().getTopLeft().getY()+30,
        150,
        20
    );
    

    titleLabel.setBounds(getWidth() / 8,
        0,
        300,
        20);
        

    grainLabel.setBounds (durationLabel.getBounds().getBottomLeft().getX(), densityMetroButton.getBounds().getBottom() + 20, 50, 20);
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

    stereoSlider.setBounds(loopButton.getBounds().getBottomLeft().getX(),
        loopButton.getBounds().getBottomLeft().getY() + 60,
        300,
        20);

    sliderButton.setBounds(stereoSlider.getBounds().getTopRight().getX()+10,
        stereoSlider.getBounds().getTopRight().getY(),
        300,
        20);

    
    

    textPannerLabel.setBounds(stereoSlider.getBounds().getTopLeft().getX(),
        stereoSlider.getBounds().getTopLeft().getY()-20,
        300,
        20);

    lfoModeLabel.setBounds(stereoSlider.getBounds().getBottomLeft().getX(),
        stereoSlider.getBounds().getBottomLeft().getY() + 10,
        50,
        20);
    sinusButton.setBounds(lfoModeLabel.getBounds().getTopRight().getX()+10,
        lfoModeLabel.getBounds().getTopRight().getY(),
        300,
        20);

}

void MainComponent::loadDefaultSound ()
{
    shutdownAudio();//eteint la source audio

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
        currentGrain.duration_ms = juce::Random::getSystemRandom().nextInt (juce::Range<int> (minDuration, maxDuration));//val aléatoire pour le grain
    }
    else
    {
        currentGrain.duration_ms = minDuration;
    }

    currentGrain.duration_sample = int( currentGrain.duration_ms * samplingRate / 1000 );// en seconde

    // Prevent from having grainStartSample + grainDuration > fileBuffer.getNumSamples()
    currentGrain.start_sample = juce::Random::getSystemRandom().nextInt( fileBuffer.getNumSamples()- currentGrain.duration_sample );//la ou demarre le Grain par rapport au buffer
    currentGrain.stop_sample  = currentGrain.start_sample + currentGrain.duration_sample;
    if (isRandomStereo) stereoFrame.position = juce::Random::getSystemRandom().nextInt(juce::Range<int>(stereoSlider.getMinimum(), stereoSlider.getMaximum()));
    
    
    
    auto grainStartSampleString    = juce::String (int (currentGrain.start_sample / samplingRate * 1000));//retour en ms
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
