#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace Speka
{

class SpeakerViewComponent : public juce::Component
{
public:
    SpeakerViewComponent();
    ~SpeakerViewComponent() override = default;

    void setSpeakerType (int type); // 0 = Paper Cone, 1 = Piezo Buzzer
    void setExcursion (float exc);  // 0.0 to 1.0

    void paint (juce::Graphics& g) override;

private:
    int speakerType = 0;
    float currentExcursion = 0.0f;
    float smoothedExcursion = 0.0f;

    juce::Image coneImage;
    juce::Image piezoImage;

    void loadAssets();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpeakerViewComponent)
};

} // namespace Speka
