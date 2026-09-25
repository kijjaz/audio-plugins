cmake_minimum_required(VERSION 3.22)

project(AetherBeam VERSION 1.0.0 LANGUAGES CXX C)

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

# Locate local JUCE or fetch automatically from GitHub
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../FrankenStitch/JUCE")
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../FrankenStitch/JUCE" JUCE)
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/JUCE")
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/JUCE" JUCE)
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

juce_add_plugin(AetherBeam
    COMPANY_NAME "Kijjaz"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE 
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE "Kijj"
    PLUGIN_CODE "AeBm"
    VST3_AUTO_MANIFEST FALSE
    FORMATS ${FORMATS}
    PRODUCT_NAME "AetherBeam Reverb"
)

target_sources(AetherBeam
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h
        
        # Real-time DSP
        Source/DSP/AcousticDefinitions.h
        Source/DSP/AcousticDatabase.h
        Source/DSP/NonlinearDelayLine.h
        Source/DSP/NonlinearWaveguideArray.h
        Source/DSP/BeamSeededFDN.h
        Source/DSP/BinauralSpatializer.h
        Source/DSP/CleverTailDetector.h
        
        # Spatial Geometry & Tracing
        Source/Spatial/AcousticMeshTracer.h

        # UI
        Source/UI/SpaceVisualizer.h
)

target_compile_definitions(AetherBeam
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(AetherBeam
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_gui_basics
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
