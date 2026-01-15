cmake_minimum_required(VERSION 3.22)

# Project Name
project(Discrete808 VERSION 0.1.0 LANGUAGES CXX C)

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
    set(FORMATS VST3 AU Standalone)
else()
    set(FORMATS VST3 Standalone)
endif()

juce_add_plugin(Discrete808
    COMPANY_NAME "Kijjaz"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE 
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE "Kijj"
    PLUGIN_CODE "D808"
    VST3_AUTO_MANIFEST FALSE
    FORMATS ${FORMATS}
    PRODUCT_NAME "Discrete 808"
)

# --- Sources ---
target_sources(Discrete808
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h
        
        # DSP Headers
        Source/DSP/ZDFResonator.h
        Source/DSP/PolyBLEPOscillator.h
        Source/DSP/AnalogKit.h
        
        # Voice Headers
        Source/Voices/Voice.h
        Source/Voices/KickVoice.h
        Source/Voices/SnareVoice.h
        Source/Voices/HiHatVoice.h
        Source/Voices/TomVoice.h
        Source/Voices/ClapVoice.h
        Source/Voices/RimshotVoice.h
        Source/Voices/CowbellVoice.h
        Source/Voices/MaracasVoice.h
        Source/Voices/HiHatVoice.h
        Source/Voices/CymbalVoice.h
        
        # UI Headers
        Source/UI/HelpComponent.h
)

# --- Linking ---
target_link_libraries(Discrete808
    PRIVATE
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_extra
        juce::juce_dsp
)

# --- Compile Definitions ---
target_compile_definitions(Discrete808
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

# --- RPATH ---
set_target_properties(Discrete808 PROPERTIES
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH "@loader_path"
    JUCE_VST3_MANIFEST_FILE "OFF"
)

# --- Post-Build Signing (Apple Only) ---
if(APPLE)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../scripts/sign_plugin.sh")
        add_custom_command(TARGET Discrete808_VST3 POST_BUILD
            COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/../scripts/sign_plugin.sh" "$<TARGET_FILE_DIR:Discrete808_VST3>/../../"
            COMMENT "Signing Discrete808 VST3..."
        )
        add_custom_command(TARGET Discrete808_AU POST_BUILD
            COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/../scripts/sign_plugin.sh" "$<TARGET_FILE_DIR:Discrete808_AU>/../../"
            COMMENT "Signing Discrete808 AU..."
        )
        add_custom_command(TARGET Discrete808_Standalone POST_BUILD
            COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/../scripts/sign_plugin.sh" "$<TARGET_FILE_DIR:Discrete808_Standalone>/../../"
            COMMENT "Signing Discrete808 Standalone..."
        )
    endif()
endif()
