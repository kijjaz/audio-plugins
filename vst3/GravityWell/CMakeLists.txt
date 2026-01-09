cmake_minimum_required(VERSION 3.15)
project(GravityWell VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add JUCE (assuming it's in the parent folder's build deps or we re-fetch)
# For simplicity in this environment, let's fetch it again or assume the user has it.
# We will use the standard CPM or FetchContent pattern if I can.
# Actually, I can allow the user to use the same JUCE as Cable Lift if I knew where it was.
# Let's try to link to the one in ../build/_deps/juce-src if it exists?
# Safest: Standard fetch.

# add_subdirectory(../vst3/build/_deps/juce-src juce-build EXCLUDE_FROM_ALL)
# Reusing JUCE from previous build is risky with paths. Using FetchContent.

include(FetchContent)
FetchContent_Declare(
    juce
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        7.0.12
)
FetchContent_MakeAvailable(juce)

juce_add_plugin(GravityWell
    COMPANY_NAME "Kijjaz"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE "Kijj"
    PLUGIN_CODE "GvWl"
    FORMATS VST3 AU
    PRODUCT_NAME "Gravity Well"
)

target_sources(GravityWell
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h
        Source/GravityData.h
)

target_compile_definitions(GravityWell PRIVATE
    JUCE_VST3_CAN_REPLACE_VST2=0
    JucePlugin_Build_VST2=0
    JucePlugin_Enable_VST2_Compatible_Classes=0
)

# Universal Binary for macOS
if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13")
endif()

target_link_libraries(GravityWell
    PRIVATE
    juce::juce_core
        juce::juce_data_structures
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_dsp
)

juce_add_binary_data(GravityWellAssets
    SOURCES
        Resources/gravity_well_bg_no_text.png
)

target_link_libraries(GravityWell PRIVATE GravityWellAssets)

juce_generate_juce_header(GravityWell)
