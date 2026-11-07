cmake_minimum_required(VERSION 3.22)

project(SilkenTofu VERSION 0.0.2 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
    set(FORMATS VST3 AU Standalone)
elseif(WIN32)
    set(FORMATS VST3 Standalone)
else()
    set(FORMATS VST3 Standalone)
endif()

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.4
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(JUCE)

# --- 1970s Vintage Hardware Assets ---
juce_add_binary_data(SilkenTofuAssets
    SOURCES
        "Assets/chassis_faceplate.png"
        "Assets/vu_meter_vintage.png"
        "Assets/t4b_xray_window.png"
)

juce_add_plugin(SilkenTofu
    COMPANY_NAME "Kijjaz"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE "Kijj"
    PLUGIN_CODE "SkTf"
    VST3_AUTO_MANIFEST FALSE
    FORMATS ${FORMATS}
    PRODUCT_NAME "Silken Tofu"
    DESCRIPTION "Teletronix LA-2A & T4B Electro-Optical Leveling Amplifier + Oversampled Tube/Iron Suite"
)

target_sources(SilkenTofu
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h
        Source/DSP/SilkenTofuDSP.h
        Source/UI/SilkenTofuLookAndFeel.h
)

target_include_directories(SilkenTofu
    PRIVATE
        Source
)

target_compile_definitions(SilkenTofu
    PUBLIC
        _USE_MATH_DEFINES
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(SilkenTofu
    PRIVATE
        SilkenTofuAssets
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_gui_basics
        juce::juce_gui_extra
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# --- Automated Calibration & 96 kHz Sample-Rate Invariance Harness ---
add_executable(silken_tofu_calibrate
    tools/CalibrateSilkenTofu.cpp
    Source/DSP/SilkenTofuDSP.h
)

target_include_directories(silken_tofu_calibrate
    PRIVATE
        Source
)

target_compile_definitions(silken_tofu_calibrate
    PUBLIC
        _USE_MATH_DEFINES
)

