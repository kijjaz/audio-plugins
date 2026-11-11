cmake_minimum_required(VERSION 3.22)

project(Kindnes VERSION 0.0.4 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
    set(PLUGIN_FORMATS VST3 AU Standalone)
else()
    set(PLUGIN_FORMATS VST3 Standalone)
endif()

# --- Dependencies: JUCE ---
include(FetchContent)
FetchContent_Declare(
    juce
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        7.0.12
)
FetchContent_MakeAvailable(juce)

# --- Plugin Definition ---
juce_add_plugin(Kindnes
    COMPANY_NAME "Kijjaz"
    VERSION "0.0.4"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE "Kijj"
    PLUGIN_CODE "Kndn"
    VST3_AUTO_MANIFEST FALSE
    FORMATS ${PLUGIN_FORMATS}
    PRODUCT_NAME "Kindnes"
)

# --- Sources ---
target_sources(Kindnes
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h
        Source/UI/KindnesLookAndFeel.h
        Source/UI/APUVisualizer.h
        Source/Audio/KindnesPresets.h
        Source/DSP/PolyBLEP.h
        Source/DSP/PulseChannel.h
        Source/DSP/TriangleChannel.h
        Source/DSP/NoiseChannel.h
        Source/DSP/DPCMChannel.h
        Source/DSP/NonLinearDAC.h
        Source/DSP/AnalogFilter.h
        Source/DSP/ArpeggiatorMacro.h
        Source/DSP/ExpansionVRC6.h
        Source/DSP/FDSChannel.h
        Source/DSP/ChiptuneEmbellishments.h
        Source/DSP/ADSREnvelope.h
        Source/DSP/StateVariableFilter.h
        Source/DSP/ModulationLFO.h
        Source/DSP/KindnesEngine.h
)

# --- Include Directories & Link Libraries ---
target_include_directories(Kindnes
    PRIVATE
        Source
)

target_link_libraries(Kindnes
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_audio_formats
        juce::juce_gui_basics
        juce::juce_dsp
)

# --- Compile Definitions ---
target_compile_definitions(Kindnes
    PUBLIC
        _USE_MATH_DEFINES
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

# --- RPATH ---
set_target_properties(Kindnes PROPERTIES
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH "@loader_path"
    JUCE_VST3_MANIFEST_FILE "OFF"
)

# --- Post-Build Signing (Apple Only) ---
if(APPLE)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../scripts/sign_plugin.sh")
        add_custom_command(TARGET Kindnes_VST3 POST_BUILD
            COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/../scripts/sign_plugin.sh" "$<TARGET_FILE_DIR:Kindnes_VST3>/../../"
            COMMENT "Signing Kindnes VST3..."
        )

        if(TARGET Kindnes_AU)
            add_custom_command(TARGET Kindnes_AU POST_BUILD
                COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/../scripts/sign_plugin.sh" "$<TARGET_FILE_DIR:Kindnes_AU>/../../"
                COMMENT "Signing Kindnes AU..."
            )
        endif()
    endif()
endif()

# --- Verification & Calibration Tools ---
add_executable(calibrate_kindnes tools/CalibratePresets.cpp)
target_include_directories(calibrate_kindnes PRIVATE Source)

add_executable(test_kindnes_96k tools/TestSineSweep96k.cpp)
target_include_directories(test_kindnes_96k PRIVATE Source)
