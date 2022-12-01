cmake_minimum_required(VERSION 3.12.4 FATAL_ERROR)

# use ccache if available
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    message(STATUS "Found ccache in ${CCACHE_PROGRAM}")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

project(Diabolika)

if(IS_DIRECTORY "${PROJECT_SOURCE_DIR}/../DGEngine.core")
    add_subdirectory(${PROJECT_SOURCE_DIR}/../DGEngine.core DGEngine.core)
else()
    add_subdirectory(DGEngine.core)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake_modules")

if(MOVIE_SUPPORT)
    find_package(FFmpeg COMPONENTS avcodec avformat avutil swscale)
endif()
find_package(PhysFS REQUIRED)
find_package(SFML 2.6 COMPONENTS audio graphics REQUIRED)

include_directories(src)

set(SOURCE_FILES
    src/Main.cpp
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

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -stdlib=libc++)
    target_link_options(${PROJECT_NAME} PRIVATE -stdlib=libc++)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 11.1)
        message(FATAL_ERROR "GCC version must be at least 11.1!")
    endif()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wpedantic)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(${PROJECT_NAME} PRIVATE /GF /EHsc /W3 /wd4250 /wd4996)
    target_link_options(${PROJECT_NAME} PRIVATE /OPT:ICF /OPT:REF)
endif()

if(FFmpeg_FOUND)
    include_directories(${FFmpeg_INCLUDES})
    target_link_libraries(${PROJECT_NAME} ${FFmpeg_LIBRARIES})
endif()

include_directories(${PHYSFS_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${PHYSFS_LIBRARY} sfml-audio sfml-graphics)

target_link_libraries(${PROJECT_NAME} DGEngine.core)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
