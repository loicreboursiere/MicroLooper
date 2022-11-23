#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
    Sources : 
    * https://docs.juce.com/master/tutorial_looping_audio_sample_buffer.html
    * https://github.com/passivist/GRNLR
    * https://docs.juce.com/master/tutorial_radio_buttons_checkboxes.html
    * To move to clean sound player : https://docs.juce.com/master/tutorial_looping_audio_sample_buffer_advanced.html
    * For this tutorial, it will be simple and straightforward
*/
class MainComponent  : public juce::AudioAppComponent,
                       //public juce::ChangeListener,
                       private juce::Timer
{
public:
    //==============================================================================
    MainComponent ();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay     (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources  () override;

    //==============================================================================
    void paint   (juce::Graphics& g) override;
    void resized () override;

    // Added functions
    void loadDefaultSound ();
    void openButtonClicked ();

    // From https://docs.juce.com/master/tutorial_audio_thumbnail.html
    //void thumbnailChanged       ();
    //void paintIfNoFileLoaded    (juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds);
    //void paintIfFileLoaded      (juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds);
    //void changeListenerCallback (juce::ChangeBroadcaster* source) override;

    /** Init grainStartSample and grainStopSample when a file is loaded.

        grainStartSample is initialized to 0 and grainStopSample to fileBuffer->getNumSamples();

        These two variables may be override by makeGrain() function.
    */
    void initStartStop();
    
    /** Compute grain in and out point based on variables gathering values of GUI elements.
        
        Use minDuration, maxDuration and samplingRate to move from ms to sample :
        grainStartSample, grainStopSample, grainDurationSample.
        Those variables are used in getNextAudioBlock processing

        Send also feedbacks (sample to ms to String) to Grain GUI elements : 
        grainInPointFeedbackLabel, grainOutPointFeedbackLabel, grainDurationFeedbackLabel
    */
    void makeGrain();

    /** Change the speed of the grain generation.

        This implementation is temporary as for now only one grain is generated at a time.
        The parameter density is then use as a Timer interval definition.

    */
    void applyDensityOrMetroDensity( int densityFromGUI );


private:
    //==============================================================================
    // Your private member variables go here...
    void timerCallback() final;

    juce::TextButton   loadButton;
    juce::TextButton   loadDefaultSoundButton;
    juce::ToggleButton loopButton { "Loop" };
    juce::ToggleButton granularButton { "Granular" };
    juce::TextButton   makeGrainButton { "Grain" };
    juce::ToggleButton densityMetroButton{ "Use density as metro" };

    juce::Label        durationLabel;
    juce::Label        durationMinLabel;
    juce::Label        durationMaxLabel;
    juce::Label        durationMinTE;
    juce::Label        durationMaxTE;
    juce::Label        densityLabel;
    juce::Label        densityTE;
    juce::Label        grainLabel;
    juce::Label        grainInPointFeedbackLabel;
    juce::Label        grainOutPointFeedbackLabel;
    juce::Label        grainDurationFeedbackLabel;
    juce::Label        grainInPointFeedbackTE;
    juce::Label        grainOutPointFeedbackTE;
    juce::Label        grainDurationFeedbackTE;

    std::unique_ptr<juce::FileChooser> chooser;

    juce::AudioFormatManager formatManager;
    juce::AudioSampleBuffer  fileBuffer;
    
    const juce::String defaultSoundFilePath = "D:\\NewTmpWorkingRepo\\Cours\\AudioProcessing\\JUCE\\violin.wav";
    //juce::AudioThumbnailCache thumbnailCache;   
    //juce::AudioThumbnail thumbnail;
    //int thumbnailHorizontalPosition;

    bool isLooping = false, isGranularizing=false, useMetro=false, computeGrainInAndOut=false;

    int filePosition, grainStartSample, grainStopSample, grainDurationSample;

    double samplingRate;

    // Granular synth parameters
    int minDuration, maxDuration;
    int density;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
