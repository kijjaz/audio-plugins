cmake_minimum_required(VERSION 3.15)

project(CableLiftEnhancer VERSION 1.0.0)

# JUCE Integration
include(FetchContent)
FetchContent_Declare(
    juce
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.0
)
FetchContent_MakeAvailable(juce)

# Plugin Configuration
juce_add_plugin(CableLiftEnhancer
    COMPANY_NAME "Kijjaz"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    PLUGIN_MANUFACTURER_CODE KiJz
    PLUGIN_CODE Cble
    FORMATS VST3 AU
    PRODUCT_NAME "Cable Lift Enhancer"
     plugin_name "Cable Lift Enhancer"
)

# Source Files
target_sources(CableLiftEnhancer PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
)

# Assets (BinaryData)
juce_add_binary_data(CableLiftAssets
    SOURCES
        Assets/cable_lift_stand.png
)

# Compile Options (C++17, macOS Archs)
target_compile_features(CableLiftEnhancer PRIVATE cxx_std_17)
target_compile_definitions(CableLiftEnhancer PRIVATE 
    JUCE_VST3_CAN_REPLACE_VST2=0
    JucePlugin_Build_VST2=0
    JucePlugin_Enable_VST2_Compatible_Classes=0
)

if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13")
endif()

# Linkage
target_link_libraries(CableLiftEnhancer PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_core
    juce::juce_data_structures
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
    CableLiftAssets
)
