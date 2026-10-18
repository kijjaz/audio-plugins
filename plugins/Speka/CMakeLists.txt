cmake_minimum_required(VERSION 3.22)

# Project Name
project(Speka VERSION 0.1.0 LANGUAGES CXX C)

# --- Standard ---
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
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../IronStack/build/_deps/juce-src")
    set(FETCHCONTENT_SOURCE_DIR_JUCE "${CMAKE_CURRENT_SOURCE_DIR}/../IronStack/build/_deps/juce-src")
endif()

FetchContent_Declare(
    juce
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        7.0.12
)
FetchContent_MakeAvailable(juce)

# --- Assets ---
juce_add_binary_data(SpekaAssets
    SOURCES
        "Assets/speaker_cone.png"
        "Assets/piezo_buzzer.png"
        "Assets/retro_knob.png"
)

# --- Plugin Target ---
juce_add_plugin(Speka
    COMPANY_NAME "Kijjaz"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE "Kijj"
    PLUGIN_CODE "Spka"
    VST3_AUTO_MANIFEST FALSE
    FORMATS ${PLUGIN_FORMATS}
    PRODUCT_NAME "Speka"
)

target_sources(Speka
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h
        Source/UI/RetroLookAndFeel.cpp
        Source/UI/RetroLookAndFeel.h
        Source/UI/SpeakerViewComponent.cpp
        Source/UI/SpeakerViewComponent.h
        Source/UI/RetroCrtComponent.cpp
        Source/UI/RetroCrtComponent.h
        Source/DSP/SpekaEngine.h
        Source/DSP/PITTimer.h
        Source/DSP/ElectricalCircuit.h
        Source/DSP/MechanicalSpeaker.h
        Source/DSP/ChassisAcoustics.h
)

target_include_directories(Speka
    PRIVATE
        Source
)

target_link_libraries(Speka
    PRIVATE
        SpekaAssets
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_gui_basics
        juce::juce_gui_extra
        juce::juce_dsp
)

target_compile_definitions(Speka
    PUBLIC
        _USE_MATH_DEFINES
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

# --- Post-Build Signing (Apple Only) ---
if(APPLE)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../scripts/sign_plugin.sh")
        add_custom_command(TARGET Speka_VST3 POST_BUILD
            COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/../scripts/sign_plugin.sh" "$<TARGET_FILE_DIR:Speka_VST3>/../../"
            COMMENT "Signing Speka VST3..."
        )

        if(TARGET Speka_AU)
            add_custom_command(TARGET Speka_AU POST_BUILD
                COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/../scripts/sign_plugin.sh" "$<TARGET_FILE_DIR:Speka_AU>/../../"
                COMMENT "Signing Speka AU..."
            )
        endif()
    endif()
endif()

# --- Speka CLI Tool ---
add_executable(speka_cli
    tools/SpekaCLI.cpp
    Source/DSP/SpekaEngine.h
    Source/DSP/PITTimer.h
    Source/DSP/ElectricalCircuit.h
    Source/DSP/MechanicalSpeaker.h
    Source/DSP/ChassisAcoustics.h
)

target_include_directories(speka_cli
    PRIVATE
        Source
)

target_compile_definitions(speka_cli
    PUBLIC
        _USE_MATH_DEFINES
)

if(MSVC)
    target_compile_options(speka_cli PRIVATE /W4 /O2)
else()
    target_compile_options(speka_cli PRIVATE -Wall -Wextra -O3)
endif()
