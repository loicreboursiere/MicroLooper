#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  :  public juce::AudioAppComponent,
                        private juce::MidiInputCallback,
                        private juce::MidiKeyboardStateListener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;
    void loadDefaultSoundFile();
	void openButtonClicked();
    void loadSoundFile(juce::String filePath);
    void makegrain();
    int msToSample(int input);
	
	struct Grain
	{
		int grainStart = 0;
		int grainStop = 0;
		int grainDuration = 0;
		int grainDurationSample = 0;
	};
	
	Grain newGrain(int start, int duration);
    
private:
    //==============================================================================
    // Your private member variables go here...
	juce::Label title;
    juce::TextButton openButton;
	std::unique_ptr<juce::FileChooser> chooser;
	juce::Label filePathLabel;
	
    juce::TextButton defaultLoadButton;
		
    juce::Label durationLabel;
    juce::Slider sliderMin;
    juce::Slider sliderMax;
	
    juce::Label densityLabel;
	juce::Slider densitySlider;
	
    juce::TextButton grainBtn;
    juce::ToggleButton metroCheckBox;
	
	juce::TextButton randomToggle;
	juce::TextButton speedBtn;
		
	juce::Slider bpmSlider;
    juce::MidiKeyboardComponent pianoVisu;
    juce::MidiKeyboardState keyboardState;
    
    juce::TextButton ppBtn;
    
    juce::String defaultSoundFilePath = "/Users/jairaume/ISEN/M1/M&T/sounds/son.mp3";
    
    juce::AudioFormatManager formatManager;
    juce::AudioSampleBuffer fileBuffer;
    
    int filePosition = 0;
    int samplingRate = 0;
    int cpt = 0;
    
    int valDurationMin = 100;
    int valDurationMax = 400;
    int valDensity = 100;
    int valDensitySample=1000;
    
    int maxFileSize = 0;

	Grain myGrain;
	Grain grainArray[17] = {};
	int noteIndex = 0;
    
    bool isGraining;
    bool isPlaying;
    bool isMetro;
	bool isRandom;
	int speed;
    
    int gap = 15;
    int eltWidth = 100;
    int eltHeight = 30;
	int sliderHeight;
	int sliderWidth;
    
    // handle midi inputs
    void handleIncomingMidiMessage (juce::MidiInput* source, const juce::MidiMessage& message) override;

    void handleNoteOn (juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;
    
    void handleNoteOff (juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float /*velocity*/) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
