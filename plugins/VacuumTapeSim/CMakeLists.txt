cmake_minimum_required(VERSION 3.22)

# Project Name
project(VacuumTapeSim VERSION 1.0.0 LANGUAGES CXX C)

# --- Standard ---
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
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
if(APPLE)
    set(FORMATS VST3 AU)
else()
    set(FORMATS VST3)
endif()

juce_add_plugin(VacuumTapeSim
    COMPANY_NAME "Kijjaz"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE "Kijj"
    PLUGIN_CODE "VTS1"
    VST3_AUTO_MANIFEST FALSE
    FORMATS ${FORMATS}
    PRODUCT_NAME "Vacuum Tape Sim"
)

# --- Resources ---
juce_add_binary_data(VacuumTapeSim_Resources
    SOURCES
        Resources/background.png
        Resources/Fonts/PlayfairDisplay-Regular.ttf
        Resources/Fonts/PlayfairDisplay-Italic.ttf
        Resources/Fonts/PlayfairDisplay-Bold.ttf
)

# --- Sources ---
target_sources(VacuumTapeSim
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h
        # DSP Headers
        Source/DSP/VacuumCompressor.h
        Source/DSP/TapeHysteresis.h
        Source/DSP/TapeLoss.h
        Source/DSP/TapeTransport.h
)

# --- Linking ---
target_link_libraries(VacuumTapeSim
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_gui_basics
        juce::juce_dsp
        VacuumTapeSim_Resources
)

# --- Compile Definitions ---
target_compile_definitions(VacuumTapeSim
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

# --- RPATH ---
set_target_properties(VacuumTapeSim PROPERTIES
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH "@loader_path"
    JUCE_VST3_MANIFEST_FILE "OFF"
)

# --- Post-Build Signing (Apple Only) ---
if(APPLE)
    # VST3
    add_custom_command(TARGET VacuumTapeSim_VST3 POST_BUILD
        COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/../scripts/sign_plugin.sh" "$<TARGET_FILE_DIR:VacuumTapeSim_VST3>/../../"
        COMMENT "Signing Vacuum Tape Sim VST3..."
    )

    # AU
    add_custom_command(TARGET VacuumTapeSim_AU POST_BUILD
        COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/../scripts/sign_plugin.sh" "$<TARGET_FILE_DIR:VacuumTapeSim_AU>/../../"
        COMMENT "Signing Vacuum Tape Sim AU..."
    )
endif()
