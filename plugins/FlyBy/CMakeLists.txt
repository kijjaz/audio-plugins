cmake_minimum_required(VERSION 3.22)

project(FlyBy VERSION 1.0.0 LANGUAGES CXX C)

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

# Locate local JUCE
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../FriedTofuMultiband/JUCE")
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../FriedTofuMultiband/JUCE" JUCE)
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../JUCE")
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../../JUCE" JUCE)
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../FrankenStitch/JUCE")
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../FrankenStitch/JUCE" JUCE)
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../FriedTofu/JUCE")
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../FriedTofu/JUCE" JUCE)
else()
    include(FetchContent)
    FetchContent_Declare(
        JUCE
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG        8.0.4
        GIT_SHALLOW    TRUE
    )
    FetchContent_MakeAvailable(JUCE)
endif()

juce_add_plugin(FlyBy
    COMPANY_NAME "Kijjaz"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE "Kijj"
    PLUGIN_CODE "FlBy"
    VST3_AUTO_MANIFEST FALSE
    FORMATS ${FORMATS}
    PRODUCT_NAME "FlyBy"
    DESCRIPTION "3D Binaural Elevation & Flight Trajectory Spatial Audio Plugin"
)

target_sources(FlyBy
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h
        Source/DSP/SpatialMath.h
        Source/DSP/FractionalDelayLine.h
        Source/DSP/HeadShadowFilter.h
        Source/DSP/PinnaFilter.h
        Source/DSP/AnthropometricModel.h
        Source/DSP/FlightPathGenerator.h
        Source/UI/FlyByLookAndFeel.h
        Source/UI/FlyByLookAndFeel.cpp
        Source/UI/Radar3DComponent.h
        Source/UI/Radar3DComponent.cpp
)

target_include_directories(FlyBy
    PRIVATE
        Source
)

target_compile_definitions(FlyBy
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(FlyBy
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_gui_basics
        juce::juce_gui_extra
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
)
