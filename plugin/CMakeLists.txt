cmake_minimum_required(VERSION 3.22)

# ---- Project ----
project(MaxSynth VERSION 1.0.0 LANGUAGES CXX)

# JUCE needs at least C++17 for recent releases
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- Locate JUCE ----
# Prefer a local JUCE checkout (e.g. as a submodule), fall back to a system install.
# Option A: add_subdirectory(../JUCE)  # <- uncomment and point to your JUCE folder
# Option B (default): find a system-wide JUCE package


# ---- Sources ----
# Add your synth sources here (mirrors the Makefile’s lists)
set(MAXSYNTH_SOURCES
    # UI
    Components/ADSRComponent.cpp
    Components/FilterComponent.cpp
    Components/OscillatorComponent.cpp
    Components/LFOComponent.cpp

    # DSP / data
    Data/ADSRData.cpp
    Source/SynthVoice.cpp
    Source/SynthSound.cpp

    # Plugin
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
)

# ---- Plugin ----
# This reproduces the Makefile’s intent:
# - VST3 + Standalone
# - IsSynth, wants MIDI input, not a MIDI effect, no MIDI output
# - Company/product metadata & 4-char codes
juce_add_plugin(MaxSynth
    COMPANY_NAME                "MaxAudio"
    COMPANY_WEBSITE             ""
    COMPANY_EMAIL               ""
    COMPANY_CODE                Maxa                 # Changed to be unique
    PLUGIN_NAME                 "MaxSynth"
    PLUGIN_DESCRIPTION          "MaxSynth Audio Synthesizer"
    PLUGIN_CODE                 Mxsy                 # Changed to be unique
    BUNDLE_ID                   com.maxaudio.MaxSynth

    VERSION                     ${PROJECT_VERSION}

    # Build targets:
    FORMATS                     VST3 Standalone

    # Behaviour:
    IS_SYNTH                    TRUE
    NEEDS_MIDI_INPUT            TRUE
    PRODUCES_MIDI_OUTPUT        FALSE
    IS_MIDI_EFFECT              FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE

    # VST3 metadata:
    VST3_CATEGORIES             "Instrument|Synth"
    
    # Important: Ensure proper plugin identification
    PLUGIN_MANUFACTURER_CODE    Maxa
    PLUGIN_SUBTYPE_CODE         Mxsy

    # Nice to have:
    COPY_PLUGIN_AFTER_BUILD     TRUE
    
    # Fix VST3 binary name for better DAW compatibility
    VST3_AUTO_MANIFEST          FALSE
)

target_sources(MaxSynth PRIVATE ${MAXSYNTH_SOURCES})

target_include_directories(MaxSynth
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/JuceLibraryCode
)

# ---- JUCE modules (mirrors what the Makefile compiled) ----
# You likely don’t need *all* of these, but they match your reference.
target_link_libraries(MaxSynth
    PRIVATE
        #juce::juce_analytics
        juce::juce_audio_basics
        juce::juce_audio_devices
        #juce::juce_audio_formats
        juce::juce_audio_plugin_client
        juce::juce_audio_processors
        juce::juce_audio_utils
        #uce::juce_box2d
        juce::juce_core
        #juce::juce_cryptography
        #juce::juce_data_structures
        juce::juce_dsp
        #uce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
        juce::juce_midi_ci
        #juce::juce_opengl
        juce::juce_osc
        #juce::juce_product_unlocking
        #juce::juce_video
)


target_compile_definitions(MaxSynth
    PRIVATE
        JUCE_VST3_CAN_REPLACE_VST2=0
        JucePlugin_Build_VST=0
        JucePlugin_Build_VST3=1
        JucePlugin_Build_Standalone=1

        # If you don't need image loading
        JUCE_INCLUDE_JPEGLIB_CODE=0
        JUCE_INCLUDE_PNGLIB_CODE=0
)
# ---- Build-type defines to mirror Projucer flags ----
# JUCE sets NDEBUG for Release automatically. Add the same feature toggles you had:
# target_compile_definitions(MaxSynth
#     PRIVATE
#         JUCE_DISPLAY_SPLASH_SCREEN=1
#         JUCE_USE_DARK_SPLASH_SCREEN=1
#         JUCE_VST3_CAN_REPLACE_VST2=0
#         JUCE_STRICT_REFCOUNTEDPOINTER=1

#         # These were in the Makefile; juce_add_plugin already controls formats,
#         # so the explicit *Build_* defines aren’t strictly necessary.
#         # Keeping these here for parity with the reference:
#         JucePlugin_Build_VST=0
#         JucePlugin_Build_VST3=1
#         JucePlugin_Build_AU=0
#         JucePlugin_Build_AUv3=0
#         JucePlugin_Build_AAX=0
#         JucePlugin_Build_Standalone=1
#         JucePlugin_Build_Unity=0
#         JucePlugin_Build_LV2=0
#         JucePlugin_Enable_IAA=0
#         JucePlugin_Enable_ARA=0

#         # Standalone app flag (as in your Makefile):
#         # Optional: match the helper behavior your Makefile used
#         JUCE_USE_EXTERNAL_TEMPORARY_SUBPROCESS=1
# )

# ---- Optimisation/debug flags comparable to the Makefile ----
# (CMake chooses sensible defaults; you can tweak if you really want parity.)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(MaxSynth PRIVATE -g -ggdb -O0)
else()
    target_compile_options(MaxSynth PRIVATE -O3)
endif()

# ---- Linux-specific system libs (Pro-tip: JUCE usually pulls most of these in automatically) ----
if(UNIX AND NOT APPLE)
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        # Mirror your pkg-config deps; these are optional because many are brought in by JUCE.
        foreach(pkg IN ITEMS
            alsa freetype2 gl libcurl zlib libjpeg libpng flac vorbis vorbisfile vorbisenc ogg jack
            webkit2gtk-4.0 gtk+-x11-3.0
        )
            pkg_check_modules(${pkg}_PKG QUIET ${pkg})
            if(${pkg}_PKG_FOUND)
                target_include_directories(MaxSynth PRIVATE ${${pkg}_PKG_INCLUDE_DIRS})
                target_link_directories(MaxSynth  PRIVATE ${${pkg}_PKG_LIBRARY_DIRS})
                target_link_libraries(MaxSynth   PRIVATE ${${pkg}_PKG_LIBRARIES})
            endif()
        endforeach()
    endif()

    # Match the explicit libs in your Makefile:
    target_link_libraries(MaxSynth PRIVATE pthread dl rt)
endif()

# ---- Nice defaults for warnings (optional) ----
if(MSVC)
    target_compile_options(MaxSynth PRIVATE /W4)
else()
    target_compile_options(MaxSynth PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ---- Installation (optional) ----
# `cmake --install .` will place the built plugin(s) appropriately.
include(GNUInstallDirs)
