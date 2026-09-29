cmake_minimum_required(VERSION 3.22)

# Project Name
project(IronStack VERSION 0.0.2 LANGUAGES CXX C)

# --- Standard ---
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
    set(PLUGIN_FORMATS VST3 AU)
else()
    set(PLUGIN_FORMATS VST3)
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
juce_add_plugin(IronStack
    COMPANY_NAME "Kijjaz"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD FALSE # We sign manually via script on macOS
    PLUGIN_MANUFACTURER_CODE "Kijj"
    PLUGIN_CODE "IrSt"
    VST3_AUTO_MANIFEST FALSE
    FORMATS ${PLUGIN_FORMATS}
    PRODUCT_NAME "IronStack"
)

# --- Sources ---
target_sources(IronStack
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h
        Source/UI/IronStackLookAndFeel.h
        Source/UI/ToneVisualizerComponent.h
        # DSP Headers (Header-only for now)
        Source/DSP/ToneStack.h
        Source/DSP/TubeStage.h
        Source/DSP/Cabinet.h
)

# --- Linking ---
target_link_libraries(IronStack
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_gui_basics
        juce::juce_dsp
)

# --- Compile Definitions ---
target_compile_definitions(IronStack
    PUBLIC
        _USE_MATH_DEFINES
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

# --- RPATH ---
set_target_properties(IronStack PROPERTIES
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH "@loader_path"
    JUCE_VST3_MANIFEST_FILE "OFF"
)

# --- Post-Build Signing (Apple Only) ---
if(APPLE)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../scripts/sign_plugin.sh")
        add_custom_command(TARGET IronStack_VST3 POST_BUILD
            COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/../scripts/sign_plugin.sh" "$<TARGET_FILE_DIR:IronStack_VST3>/../../"
            COMMENT "Signing IronStack VST3..."
        )

        if(TARGET IronStack_AU)
            add_custom_command(TARGET IronStack_AU POST_BUILD
                COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/../scripts/sign_plugin.sh" "$<TARGET_FILE_DIR:IronStack_AU>/../../"
                COMMENT "Signing IronStack AU..."
            )
        endif()
    endif()
endif()
