cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

# use ccache if available
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    message(STATUS "Found ccache in ${CCACHE_PROGRAM}")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

project(Diabolika VERSION 1.2.0)

if(IS_DIRECTORY "${PROJECT_SOURCE_DIR}/../DGEngine.core")
    add_subdirectory(${PROJECT_SOURCE_DIR}/../DGEngine.core DGEngine.core)
else()
    add_subdirectory(DGEngine.core)
endif()

option(DGENGINE_COMPRESS_BINARY "Compress binary" FALSE)
option(DGENGINE_OPTIMISE "Optimize build" FALSE)

set(SOURCE_FILES
    src/RegisterHooks.cpp
    src/RegisterHooks.h
    src/Game/Actions/ActLevel.h
    src/Game/Level/Level.cpp
    src/Game/Level/Level.h
    src/Game/Level/LevelBoard.cpp
    src/Game/Level/LevelBoard.h
    src/Game/Level/LevelCell.h
    src/Game/Level/LevelDraw.cpp
    src/Game/Level/LevelDraw.h
    src/Game/Level/LevelOptions.cpp
    src/Game/Level/LevelOptions.h
    src/Game/Level/LevelOptionsManager.cpp
    src/Game/Level/LevelOptionsManager.h
    src/Game/Level/LevelSave.cpp
    src/Game/Level/LevelSave.h
    src/Game/Level/LevelSaveObject.h
    src/Game/Level/LevelScore.h
    src/Game/Level/LevelScoreManager.cpp
    src/Game/Level/LevelScoreManager.h
    src/Game/Level/LevelState.h
    src/Game/Level/LevelStateObject.h
    src/Game/Level/LevelStats.cpp
    src/Game/Level/LevelStats.h
    src/Game/Level/LevelUIObject.cpp
    src/Game/Level/LevelUIObject.h
    src/Game/Level/LevelUnitManager.cpp
    src/Game/Level/LevelUnitManager.h
    src/Game/Level/LevelUnitQueue.h
    src/Game/Properties/UnitLink.h
    src/Game/Unit/Unit.cpp
    src/Game/Unit/Unit.h
    src/Game/Unit/UnitClass.cpp
    src/Game/Unit/UnitClass.h
    src/Game/Utils/GameUtils2.cpp
    src/Game/Utils/GameUtils2.h
    src/Parser/ParseAction2.cpp
    src/Parser/ParseAction2.h
    src/Parser/ParseFile2.cpp
    src/Parser/ParseFile2.h
    src/Parser/Actions/ParseLevelActions.cpp
    src/Parser/Actions/ParseLevelActions.h
    src/Parser/Level/ParseLevel.cpp
    src/Parser/Level/ParseLevel.h
    src/Parser/Level/ParseLevelOptions.cpp
    src/Parser/Level/ParseLevelOptions.h
    src/Parser/Level/ParseLevelSave.cpp
    src/Parser/Level/ParseLevelSave.h
    src/Parser/Level/ParseLevelScores.cpp
    src/Parser/Level/ParseLevelScores.h
    src/Parser/Unit/ParseUnit.cpp
    src/Parser/Unit/ParseUnit.h
    src/Parser/Unit/ParseUnitClass.cpp
    src/Parser/Unit/ParseUnitClass.h
    src/Parser/Utils/ParseUtilsGame.h
    src/Parser/Utils/ParseUtilsGameKey.cpp
    src/Parser/Utils/ParseUtilsGameKey.h
)

if(DGENGINE_STATIC_CRT AND WIN32)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()
endif()

add_library(${PROJECT_NAME}Lib ${SOURCE_FILES})

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(${PROJECT_NAME}Lib PUBLIC -Wall -stdlib=libc++)
    target_link_options(${PROJECT_NAME}Lib PUBLIC -stdlib=libc++)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 11.1)
        message(FATAL_ERROR "GCC version must be at least 11.1!")
    endif()
    target_compile_options(${PROJECT_NAME}Lib PUBLIC -Wall -Wpedantic)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(${PROJECT_NAME}Lib PUBLIC /W3 /wd4250 /wd4996)
endif()

target_include_directories(${PROJECT_NAME}Lib PUBLIC src)
target_link_libraries(${PROJECT_NAME}Lib PUBLIC DGEngine.core)

add_executable(${PROJECT_NAME} src/Main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}Lib)

if(DGENGINE_OPTIMISE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        target_compile_options(${PROJECT_NAME} PRIVATE -O3)
        set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS_RELEASE -s)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        target_compile_options(${PROJECT_NAME} PRIVATE /GF /EHsc)
        target_link_options(${PROJECT_NAME} PRIVATE /OPT:ICF /OPT:REF)
    endif()
    if(DGENGINE_COMPRESS_BINARY)
        include(FindSelfPackers)
        if(SELF_PACKER_FOR_EXECUTABLE)
            ADD_CUSTOM_COMMAND(
                COMMAND ${SELF_PACKER_FOR_EXECUTABLE} $<TARGET_FILE:${PROJECT_NAME}>
                ARGS ${SELF_PACKER_FOR_EXECUTABLE_FLAGS} -9q
                TARGET ${PROJECT_NAME}
            )
        endif()
    endif()
endif()
