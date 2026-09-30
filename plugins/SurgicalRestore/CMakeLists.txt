cmake_minimum_required(VERSION 3.22)

project(SurgicalRestore VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
    set(FORMATS VST3 AU Standalone)
else()
    set(FORMATS VST3 Standalone)
endif()

# Locate local JUCE or fetch
include(FetchContent)
FetchContent_Declare(
    juce
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.4
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(juce)

# Binary resources (embedded ONNX models)
juce_add_binary_data(SurgicalRestore_Models
    SOURCES
        Source/Models/click_detector.onnx
        Source/Models/spectral_denoiser.onnx
)

juce_add_plugin(SurgicalRestore
    COMPANY_NAME "Kijjaz"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE "Kijj"
    PLUGIN_CODE "SrgR"
    VST3_AUTO_MANIFEST FALSE
    FORMATS ${FORMATS}
    PRODUCT_NAME "Surgical Restore"
    DESCRIPTION "Neural Mastering Audio Restoration & Vinyl Denoiser (ARA 2 / VST3)"
)

target_sources(SurgicalRestore
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h
        Source/DSP/LPCResidualEngine.h
        Source/DSP/ARInpainter.h
        Source/DSP/DeCrackleEngine.h
        Source/DSP/SpectralDeNoiser.h
        Source/UI/CarbonGoldLookAndFeel.h
        Source/UI/CarbonGoldLookAndFeel.cpp
)

target_compile_definitions(SurgicalRestore
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
)

target_link_libraries(SurgicalRestore
    PRIVATE
        SurgicalRestore_Models
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_dsp
        juce::juce_gui_basics
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# Stress test binary
add_executable(SurgicalRestore_StressTest
    Source/StressTest.cpp
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/UI/CarbonGoldLookAndFeel.cpp
)

target_link_libraries(SurgicalRestore_StressTest
    PRIVATE
        SurgicalRestore_Models
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_dsp
        juce::juce_gui_basics
        juce::juce_gui_extra
)
