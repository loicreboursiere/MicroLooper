#include "MainComponent.h"
#include <JuceHeader.h>

//==============================================================================
MainComponent::MainComponent()
:pianoVisu(keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    // Add and make visible all elements
	addAndMakeVisible(title);
    addAndMakeVisible(openButton);
	addAndMakeVisible(filePathLabel);
	
    addAndMakeVisible(defaultLoadButton);
	addAndMakeVisible(ppBtn);
	
    addAndMakeVisible(durationLabel);
    addAndMakeVisible(sliderMin);
	addAndMakeVisible(sliderMax);
	
    addAndMakeVisible(densityLabel);
	addAndMakeVisible(densitySlider);

    addAndMakeVisible(grainBtn);
    addAndMakeVisible(metroCheckBox);
	
	addAndMakeVisible(randomToggle);
	addAndMakeVisible(speedBtn);
	
	addAndMakeVisible(bpmSlider);
	
    addAndMakeVisible(pianoVisu);
    
    keyboardState.addListener (this);
	
    int keyPiano[17] = {81,90,83,69,68,70,84,71,89,72,85,74,75,79,76,80,77};
    for( unsigned int a = 0; a < 17; a++ ){
        pianoVisu.setKeyPressForNote(juce::KeyPress(keyPiano[a]), a);
        DBG(keyPiano[a] << " -> " << juce::String(a));
    }
	pianoVisu.setMidiChannelsToDisplay(1);
    
    // Set button texts
    openButton          .setButtonText("Open...");
    defaultLoadButton   .setButtonText("Load default sound");
    grainBtn            .setButtonText("GRAIN");
    ppBtn               .setButtonText("play");
    metroCheckBox       .setButtonText("Metro on/off");
	speedBtn			.setButtonText("Speed : 1x");
	randomToggle		.setButtonText("Random distribution : no");
    
    // Set label texts
	title				.setText("PIANO GRAINER", juce::dontSendNotification);
	filePathLabel		.setText("No file loaded...", juce::dontSendNotification);
    durationLabel       .setText("Duration min/max (ms)", juce::dontSendNotification);
    densityLabel        .setText("Density", juce::dontSendNotification);
	
	//set slider properties :
	densitySlider		.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	densitySlider		.setRange(0, 500, 1);
	densitySlider		.setTextBoxStyle(juce::Slider::TextBoxBelow, false, eltWidth/2, eltHeight);
	
	sliderMin			.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
	sliderMin			.setRange(0, 500, 1);
	sliderMin			.setTextBoxStyle(juce::Slider::TextBoxBelow, false, eltWidth/2, eltHeight);
	
	sliderMax			.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
	sliderMax			.setRange(0, 500, 1);
	sliderMax			.setTextBoxStyle(juce::Slider::TextBoxBelow, false, eltWidth/2, eltHeight);
	
	bpmSlider			.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	bpmSlider			.setRange(60, 200,1);
	bpmSlider			.setTextBoxStyle(juce::Slider::TextBoxBelow, false, eltWidth/2, eltHeight);
    
    // Init values
	densitySlider.setValue(300);
	sliderMin.setValue(100);
	sliderMax.setValue(400);
	bpmSlider.setValue(130);
	
    valDurationMin = sliderMin.getValue();
    valDurationMax = sliderMax.getValue();
	valDensity = densitySlider.getValue();
    valDensitySample = msToSample(densitySlider.getValue());

    // Set colours of label
	title				.setColour(juce::Label::textColourId, juce::Colours::mediumseagreen);
    grainBtn            .setColour(juce::TextButton::buttonColourId, juce::Colours::mediumseagreen);
    ppBtn               .setColour(juce::TextButton::buttonColourId, juce::Colours::mediumseagreen);
	speedBtn			.setColour(juce::TextButton::buttonColourId, juce::Colours::mediumseagreen);
	randomToggle		.setColour(juce::TextButton::buttonColourId, juce::Colours::mediumseagreen);
	openButton			.setColour(juce::TextButton::buttonColourId, juce::Colours::mediumseagreen);
	defaultLoadButton	.setColour(juce::TextButton::buttonColourId, juce::Colours::mediumseagreen);

    // Set justification type
	title				.setJustificationType(juce::Justification::centred);
    densityLabel         .setJustificationType(juce::Justification::centred);
	durationLabel        .setJustificationType(juce::Justification::centred);
	
	title				.setFont(juce::Font(30.0f,juce::Font::bold));
	
    // Event listeners
    openButton          .onClick = [this] {
        openButtonClicked();
    };
    defaultLoadButton   .onClick = [this] {
		loadDefaultSoundFile();
		pianoVisu.grabKeyboardFocus();
	};

	
    sliderMin         	.onValueChange = [this] {
		if(sliderMin.getValue() >= sliderMax.getValue()){
			sliderMax.setValue(sliderMin.getValue());
		}
		valDurationMin = sliderMin.getValue();
	};
    sliderMax         	.onValueChange = [this] {
		if(sliderMax.getValue() <= sliderMin.getValue()){
			sliderMin.setValue(sliderMax.getValue());
		}
		valDurationMax = sliderMax.getValue();
	};
	densitySlider		.onValueChange = [this]{valDensity=densitySlider.getValue();};
	
	
	
	
    grainBtn            .onClick = [this]{makegrain();};
    ppBtn               .onClick = [this]
    {
		DBG("pp");
        isPlaying = !isPlaying;
        if(isPlaying){ppBtn.setButtonText("pause");}
        else{ppBtn.setButtonText("play");}
    };
    metroCheckBox       .onClick = [this]
    {
        isMetro = !isMetro;
        if(isMetro)cpt=0;
    };
	randomToggle		.onClick = [this] {
		DBG("rand");
		isRandom = !isRandom;
		if(isRandom)randomToggle.setButtonText("Random distribution : yes");
		else{randomToggle.setButtonText("Random distribution : no");}
	};
	speedBtn			.onClick = [this] {
		if(speed == 2){
			DBG("speed 2");
			speedBtn.setButtonText("Speed : 1x");
			speed = 1;
		}
		else{
			DBG("speed 1");
			speedBtn.setButtonText("Speed : 0.5x");
			speed = 2;
		}
	};
        
    // Size of the window
    setSize (500, 450);
	
	
	
	isGraining = false;
	isPlaying = false;
	isMetro = false;
	isRandom = false;
	speed = 1;
    

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

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!

    // For more details, see the help for AudioProcessor::getNextAudioBlock()

    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)
    bufferToFill.clearActiveBufferRegion();

    const int numSamplesInBlock = bufferToFill.buffer->getNumSamples();
        
    bufferToFill.buffer->clear(); // To be sure nothing is remaining in the buffer

    // return from the function if there's nothing in our buffer
    if (fileBuffer.getNumSamples() == 0) return;

	
    for (int sample = 0; sample < numSamplesInBlock; sample=sample+2)
    {

        for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
        {
            const float* fileData = fileBuffer.getReadPointer(channel % fileBuffer.getNumChannels());
            float* channelData = bufferToFill.buffer->getWritePointer(channel);
			channelData[sample] = fileData[filePosition];
        }

        if(isPlaying){
            if(filePosition < myGrain.grainStart)
            {
                filePosition = myGrain.grainStart;
            }
            else if(isMetro && cpt > valDensity*samplingRate/1000)
            {
                cpt=0;
                filePosition=myGrain.grainStart;
            }
            else if (filePosition < myGrain.grainStop)
            {
                ++filePosition;
            }
            else
            {
                filePosition = myGrain.grainStart;
            }
            ++cpt;
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
    g.fillAll (juce::Colours::black);
	g.setColour(juce::Colours::white);
	
	
    // You can add your drawing code here!
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    eltWidth = getWidth()/4;
	sliderHeight = 3*getHeight()/4 -2*eltHeight;
	sliderWidth = eltWidth/1.2;
    
	
	title				.setBounds(getWidth()/3, 1, getWidth()/3, 2*eltHeight);

    openButton       	.setBounds(gap, gap, eltWidth, eltHeight);
	filePathLabel		.setBounds(gap, openButton.getBottom(), eltWidth, eltHeight);
    defaultLoadButton	.setBounds(gap, filePathLabel.getBottom(), eltWidth, eltHeight);
    ppBtn            	.setBounds(gap, defaultLoadButton.getBottom()+gap, eltWidth, eltHeight);
	bpmSlider			.setBounds(gap, ppBtn.getBottom()+gap, eltWidth, 3*eltHeight);

	
	grainBtn         	.setBounds(getWidth() - eltWidth - gap, gap, eltWidth, eltHeight);
	metroCheckBox    	.setBounds(grainBtn.getBounds().getX(), grainBtn.getBottom()+gap, eltWidth, eltHeight);
	
	sliderMin			.setBounds(openButton.getRight() + gap, 1.5*eltHeight, sliderWidth, sliderHeight-eltHeight);
	sliderMax			.setBounds(grainBtn.getX()-sliderWidth-gap, 1.5*eltHeight, sliderWidth, sliderHeight-eltHeight);
	durationLabel		.setBounds(getWidth()/3, sliderMin.getBottom()+gap/2, getWidth()/3, eltHeight);

    pianoVisu        	.setBounds(0, getHeight()-getHeight()/4, getWidth(), getHeight()/4);
	speedBtn		 	.setBounds(gap, pianoVisu.getY()-eltHeight-gap, eltWidth, eltHeight);
	randomToggle		.setBounds(getWidth()-eltWidth-gap, pianoVisu.getY()-eltHeight-gap, eltWidth, eltHeight);
	
	densitySlider      	.setBounds(getWidth()-eltWidth-gap, randomToggle.getY()-3*eltHeight-gap, eltWidth, 3*eltHeight);
	densityLabel		.setBounds(densitySlider.getX(), densitySlider.getY()-eltHeight, densitySlider.getWidth(), eltHeight);
	

}


void MainComponent::loadDefaultSoundFile()
{
    shutdownAudio();

    const juce::File defaultSoundFile(defaultSoundFilePath);
    if (defaultSoundFile.existsAsFile())
    {
        std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(defaultSoundFile));
        fileBuffer.setSize((int)reader->numChannels, (int)reader->lengthInSamples);
        reader->read(&fileBuffer, 0, (int)reader->lengthInSamples, 0, true, true);
        filePosition = 0;
        setAudioChannels(0, (int)reader->numChannels);
        if(maxFileSize == 0){
            maxFileSize = fileBuffer.getNumSamples();
        }

    }
    
	filePathLabel.setText(defaultSoundFile.getFileName(), juce::dontSendNotification);
	
	myGrain.grainStart = 0;
	myGrain.grainStop = maxFileSize;
}

void MainComponent::openButtonClicked(){
	shutdownAudio();                                                                            // [1]
	
	chooser = std::make_unique<juce::FileChooser>("Select a Wave file shorter than 2 seconds to play...",
												  juce::File{},
												  "*.wav ; *.mp3");
	auto chooserFlags = juce::FileBrowserComponent::openMode
	| juce::FileBrowserComponent::canSelectFiles;
	
	chooser->launchAsync(chooserFlags, [this](const juce::FileChooser& fc)
						 {
		auto file = fc.getResult();
		
		filePathLabel.setText(file.getFileName(), juce::dontSendNotification);
		
		if (file == juce::File{})
			return;
		
		std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));
		
		if (reader.get() != nullptr)
		{
			fileBuffer.setSize((int)reader->numChannels, (int)reader->lengthInSamples);
			reader->read(&fileBuffer,
						 0,
						 (int)reader->lengthInSamples,
						 0,
						 true,
						 true);
			filePosition = 0;
			setAudioChannels (0, (int)reader->numChannels);
			myGrain.grainStart = 0;
			maxFileSize = fileBuffer.getNumSamples();
			myGrain.grainStop = maxFileSize;
		}
	});
}

int MainComponent::msToSample(int input){
    auto output =  input*samplingRate/1000;
    DBG(input << " " << output);
    return output;
}

MainComponent::Grain MainComponent::newGrain(int start, int duration){
	Grain tmpGrain;
	if(duration != 0){
		tmpGrain.grainDuration=duration;
	}
	else if (valDurationMin != valDurationMax){
		tmpGrain.grainDuration = juce::Random::getSystemRandom().nextInt(juce::Range<int>(valDurationMin,valDurationMax));
	}
	else{
		tmpGrain.grainDuration = valDurationMax;
	}
	//tmpGrain.grainDuration = 1/(bpmSlider.getValue()/120)*1000;
	tmpGrain.grainDurationSample = tmpGrain.grainDuration * samplingRate / 1000;
	// generate grain start position (between 0 and end of file)
	if(start == 0){
		tmpGrain.grainStart = juce::Random::getSystemRandom().nextInt(maxFileSize-tmpGrain.grainDurationSample);
	}
	else{
		tmpGrain.grainStart = start;
	}
	tmpGrain.grainStop = tmpGrain.grainStart + tmpGrain.grainDurationSample;
	
	return tmpGrain;
}


void MainComponent::makegrain(){
	// make sure a sound is loaded
	DBG("New grains !");
	if(isRandom){
		DBG("They are random");
	}
	if(maxFileSize != 0){
		
		isGraining = true;
		cpt=0;
		
		for(int i=0; i<=17 ; i++){
			if(i>0 && !isRandom){
				grainArray[i] = newGrain(grainArray[i-1].grainStop, grainArray[0].grainDuration);
			}
			else{
				grainArray[i] = newGrain(0,0);
			}
		}
		myGrain = grainArray[0];
		DBG("el primero graino (" << myGrain.grainStart << " : " << myGrain.grainStop << ")");
	
	}
	
}


// handle midi events
void MainComponent::handleIncomingMidiMessage (juce::MidiInput* source, const juce::MidiMessage& message)
{
	keyboardState.processNextMidiEvent (message);
}


void MainComponent::handleNoteOn (juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity)
{
    auto m = juce::MidiMessage::noteOn (midiChannel, midiNoteNumber, velocity);
	noteIndex = m.getNoteNumber();
	if(noteIndex >= 72 && noteIndex < 72+17){
		myGrain = grainArray[noteIndex-72];
		DBG(m.getMidiNoteName(m.getNoteNumber(), true, true, 4));
	}
	else{
		DBG("Grain sampling is only available on the 5th scale");
	}
	isPlaying = true;
}


void MainComponent::handleNoteOff (juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float /*velocity*/)
{
    auto m = juce::MidiMessage::noteOff (midiChannel, midiNoteNumber);
	if(noteIndex == m.getNoteNumber()){
		isPlaying = false;
	}
}

