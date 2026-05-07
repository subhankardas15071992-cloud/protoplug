cmake_minimum_required(VERSION 3.22)

if(APPLE)
    if(NOT CMAKE_OSX_DEPLOYMENT_TARGET)
        set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE STRING "Minimum macOS deployment target")
    endif()

    if(NOT CMAKE_OSX_ARCHITECTURES)
        set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "macOS architectures")
    endif()
endif()

project(Protoplug VERSION 1.5.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(PROTOPLUG_ENABLE_CLAP "Build CLAP plugins with clap-juce-extensions" ON)
option(PROTOPLUG_COPY_PLUGIN_AFTER_BUILD "Copy built plugins to the system plugin folders" OFF)

set(PROTOPLUG_JUCE_DIR "" CACHE PATH "Path to an existing JUCE checkout. If empty, JUCE is fetched.")
set(PROTOPLUG_CLAP_JUCE_EXTENSIONS_DIR "" CACHE PATH "Path to an existing clap-juce-extensions checkout. If empty, it is fetched.")

include(FetchContent)

if(PROTOPLUG_JUCE_DIR)
    add_subdirectory("${PROTOPLUG_JUCE_DIR}" "${CMAKE_BINARY_DIR}/_deps/juce-build" EXCLUDE_FROM_ALL)
else()
    FetchContent_Declare(
        JUCE
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG 8.0.12
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(JUCE)
endif()

if(PROTOPLUG_ENABLE_CLAP)
    if(PROTOPLUG_CLAP_JUCE_EXTENSIONS_DIR)
        add_subdirectory("${PROTOPLUG_CLAP_JUCE_EXTENSIONS_DIR}" "${CMAKE_BINARY_DIR}/_deps/clap-juce-extensions-build" EXCLUDE_FROM_ALL)
    else()
        FetchContent_Declare(
            clap_juce_extensions
            GIT_REPOSITORY https://github.com/free-audio/clap-juce-extensions.git
            GIT_TAG e8de9e8571626633b8541a54c2406fccc4272767
            GIT_SUBMODULES_RECURSE TRUE
        )
        FetchContent_MakeAvailable(clap_juce_extensions)
    endif()
endif()

set(PROTOPLUG_FORMATS VST3 LV2)

if(APPLE)
    list(PREPEND PROTOPLUG_FORMATS AU)
endif()

set(PROTOPLUG_SOURCES
    Source/LuaCodeTokeniser.cpp
    Source/LuaCodeTokeniser.h
    Source/LuaLink.cpp
    Source/LuaLink.h
    Source/LuaState.cpp
    Source/LuaState.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/ProtoplugConstants.h
    Source/ProtoplugDir.cpp
    Source/ProtoplugDir.h
    Source/ProtoplugExports.h
    Source/exports/pAudioFormatReader.h
    Source/exports/pColourGradient.h
    Source/exports/pComponent.h
    Source/exports/pFillType.h
    Source/exports/pFont.h
    Source/exports/pGraphics.h
    Source/exports/pImage.h
    Source/exports/pImageFileFormat.h
    Source/exports/pLagrangeInterpolator.h
    Source/exports/pMidiBuffer.h
    Source/exports/pPath.h
    Source/exports/typedefs.h
    Source/guiclasses/AboutBox.h
    Source/guiclasses/BottomPane.cpp
    Source/guiclasses/BottomPane.h
    Source/guiclasses/CustomGuiPanel.cpp
    Source/guiclasses/CustomGuiPanel.h
    Source/guiclasses/DarkSplitter.h
    Source/guiclasses/Dockable.cpp
    Source/guiclasses/Dockable.h
    Source/guiclasses/HintedFeel.cpp
    Source/guiclasses/HintedFeel.h
    Source/guiclasses/LuaEditor.h
    Source/guiclasses/ParameterPanel.cpp
    Source/guiclasses/ParameterPanel.h
    Source/guiclasses/ProtoPopout.cpp
    Source/guiclasses/ProtoPopout.h
    Source/guiclasses/ProtoTabButton.h
    Source/guiclasses/ProtoWindow.cpp
    Source/guiclasses/ProtoWindow.h
    Source/vflib/BinaryDejaVu.cpp
    Source/vflib/BinaryDejaVu.h
    Source/vflib/BinarySourceCodePro.cpp
    Source/vflib/BinarySourceCodePro.h
)

function(protoplug_add_plugin target)
    set(options IS_SYNTH)
    set(one_value_args
        PRODUCT_NAME
        PLUGIN_NAME
        DESCRIPTION
        BUNDLE_ID
        PLUGIN_CODE
        LV2_URI
        CLAP_ID
    )
    set(multi_value_args CLAP_FEATURES VST3_CATEGORIES)
    cmake_parse_arguments(PROTOPLUG "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

    juce_add_plugin(${target}
        VERSION ${PROJECT_VERSION}
        COMPANY_NAME "pac"
        COMPANY_WEBSITE "http://www.osar.fr/protoplug"
        BUNDLE_ID "${PROTOPLUG_BUNDLE_ID}"
        PLUGIN_NAME "${PROTOPLUG_PLUGIN_NAME}"
        PRODUCT_NAME "${PROTOPLUG_PRODUCT_NAME}"
        DESCRIPTION "${PROTOPLUG_DESCRIPTION}"
        PLUGIN_MANUFACTURER_CODE "_PAC"
        PLUGIN_CODE "${PROTOPLUG_PLUGIN_CODE}"
        IS_SYNTH ${PROTOPLUG_IS_SYNTH}
        NEEDS_MIDI_INPUT TRUE
        NEEDS_MIDI_OUTPUT TRUE
        EDITOR_WANTS_KEYBOARD_FOCUS TRUE
        COPY_PLUGIN_AFTER_BUILD ${PROTOPLUG_COPY_PLUGIN_AFTER_BUILD}
        LV2URI "${PROTOPLUG_LV2_URI}"
        VST3_CATEGORIES ${PROTOPLUG_VST3_CATEGORIES}
        FORMATS ${PROTOPLUG_FORMATS}
        ICON_SMALL "${CMAKE_CURRENT_SOURCE_DIR}/Source/res/lua-16.png"
        ICON_BIG "${CMAKE_CURRENT_SOURCE_DIR}/Source/res/lua-32.png"
    )

    juce_generate_juce_header(${target})

    target_sources(${target} PRIVATE ${PROTOPLUG_SOURCES})

    target_include_directories(${target}
        PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/Source"
            "${CMAKE_CURRENT_SOURCE_DIR}/Source/exports"
    )

    target_compile_definitions(${target}
        PUBLIC
            JUCE_DISPLAY_SPLASH_SCREEN=0
            JUCE_REPORT_APP_USAGE=0
            JUCE_WEB_BROWSER=0
            JUCE_USE_CURL=0
            JUCE_MODAL_LOOPS_PERMITTED=1
            JUCE_VST3_CAN_REPLACE_VST2=0
    )

    target_link_libraries(${target}
        PRIVATE
            juce::juce_audio_basics
            juce::juce_audio_devices
            juce::juce_audio_formats
            juce::juce_audio_processors
            juce::juce_gui_extra
        PUBLIC
            juce::juce_recommended_config_flags
    )

    set_target_properties(${target} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
    )

    if(PROTOPLUG_ENABLE_CLAP)
        clap_juce_extensions_plugin(
            TARGET ${target}
            CLAP_ID "${PROTOPLUG_CLAP_ID}"
            CLAP_FEATURES ${PROTOPLUG_CLAP_FEATURES}
            CLAP_MANUAL_URL "http://www.osar.fr/protoplug"
            CLAP_SUPPORT_URL "https://github.com/subhankardas15071992-cloud/protoplug"
        )
    endif()
endfunction()

protoplug_add_plugin(protoplug_fx
    PRODUCT_NAME "Lua Protoplug Fx"
    PLUGIN_NAME "Lua Protoplug Fx"
    DESCRIPTION "Lua Protoplug Effect"
    BUNDLE_ID "com.pac.protoplugfx"
    PLUGIN_CODE "ppgf"
    LV2_URI "urn:pac:protoplug:fx"
    CLAP_ID "com.pac.protoplug.fx"
    CLAP_FEATURES audio-effect stereo
    VST3_CATEGORIES Fx
)

protoplug_add_plugin(protoplug_gen
    IS_SYNTH
    PRODUCT_NAME "Lua Protoplug Gen"
    PLUGIN_NAME "Lua Protoplug Gen"
    DESCRIPTION "Lua Protoplug Generator (Instrument)"
    BUNDLE_ID "com.pac.protopluggen"
    PLUGIN_CODE "ppgg"
    LV2_URI "urn:pac:protoplug:gen"
    CLAP_ID "com.pac.protoplug.gen"
    CLAP_FEATURES instrument stereo
    VST3_CATEGORIES Instrument Synth
)

target_compile_definitions(protoplug_gen PRIVATE _PROTOGEN=1)
