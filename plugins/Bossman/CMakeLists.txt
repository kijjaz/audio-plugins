cmake_minimum_required(VERSION 3.22)

# Project Name
project(Bossman VERSION 1.0.0 LANGUAGES CXX C)

# --- Standard ---
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")

# --- Dependencies: JUCE ---
include(FetchContent)
FetchContent_Declare(
    juce
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        7.0.12
)
FetchContent_MakeAvailable(juce)

# --- Plugin Definition ---
juce_add_plugin(Bossman
    COMPANY_NAME "Kijjaz"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD FALSE # We sign manually via script
    PLUGIN_MANUFACTURER_CODE "Kijj"
    PLUGIN_CODE "FB59"
    VST3_AUTO_MANIFEST FALSE
    FORMATS VST3 AU
    PRODUCT_NAME "Bossman"
)

# --- Sources ---
target_sources(Bossman
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h
        Source/UI/BossmanLookAndFeel.h
        Source/UI/ToneVisualizerComponent.h
        # DSP Headers (Header-only for now)
        Source/DSP/ToneStack.h
        Source/DSP/TubeStage.h
        Source/DSP/Cabinet.h
)

# --- Linking ---
target_link_libraries(Bossman
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_gui_basics
        juce::juce_dsp
)

# --- Compile Definitions ---
target_compile_definitions(Bossman
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

# --- RPATH ---
set_target_properties(Bossman PROPERTIES
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH "@loader_path"
    JUCE_VST3_MANIFEST_FILE "OFF"
)

# --- Post-Build Signing (Universal Script) ---
# VST3
add_custom_command(TARGET Bossman_VST3 POST_BUILD
    COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/../scripts/sign_plugin.sh" "$<TARGET_FILE_DIR:Bossman_VST3>/../../"
    COMMENT "Signing Bossman VST3..."
)

# AU
add_custom_command(TARGET Bossman_AU POST_BUILD
    COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/../scripts/sign_plugin.sh" "$<TARGET_FILE_DIR:Bossman_AU>/../../"
    COMMENT "Signing Bossman AU..."
)
