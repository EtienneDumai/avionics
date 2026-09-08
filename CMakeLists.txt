cmake_minimum_required(VERSION 3.16)
project(avionics CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Sources "logique pure" : aucune dépendance SDL2 (pas Window/Display).
# Réutilisées par la cible de tests, sur les deux OS, indépendamment
# de la façon dont l'appli elle-même est construite.
set(CORE_SOURCES
    ${CMAKE_SOURCE_DIR}/src/core/Engine/Engine.cpp
    ${CMAKE_SOURCE_DIR}/src/core/Simulator/Simulator.cpp
    ${CMAKE_SOURCE_DIR}/src/math/Vec3/Vec3.cpp
    ${CMAKE_SOURCE_DIR}/src/math/Quaternion/Quaternion.cpp
    ${CMAKE_SOURCE_DIR}/src/state/AirplaneState.cpp
)

# ==================================================================
# Build de l'appli
# ==================================================================
if(WIN32)
    # Windows : lien 100% statique -> un seul .exe, zéro DLL à côté.
    # Nécessite SDL2 et SDL2_ttf compilées en statique (triplet vcpkg
    # x64-windows-static, voir instructions plus bas).
    file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp)

    add_executable(avionics ${APP_SOURCES})
    target_compile_options(avionics PRIVATE -Wall -Wextra)

    # Force le runtime MSVC statique (/MT) pour ne dépendre d'aucune
    # vcruntime*.dll / msvcp*.dll non plus.
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

    # Variantes statiques exposées par vcpkg avec le triplet *-static
    find_package(SDL2 CONFIG REQUIRED)
    find_package(sdl2-ttf CONFIG REQUIRED)

    target_link_libraries(avionics PRIVATE
        SDL2::SDL2-static
        SDL2_ttf::SDL2_ttf-static
    )

    # Filet de sécurité : libs système dont SDL2/SDL2_ttf statiques
    # ont besoin sous Windows (normalement déjà tirées par vcpkg,
    # mais on les liste au cas où le linker râle).
    target_link_libraries(avionics PRIVATE
        winmm imm32 version setupapi ole32 oleaut32 uuid advapi32 shell32
    )

    # Copie les assets (police, etc.) à côté de l'exe généré
    add_custom_command(TARGET avionics POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:avionics>/assets
    )
else()
    # Linux : on ne réécrit rien, on délègue au Makefile qui marche déjà.
    # Simple cible custom -> n'arrête pas le traitement du script,
    # contrairement à un return() : la suite (tests) s'exécute quand même.
    add_custom_target(avionics ALL
        COMMAND $(MAKE) -C ${CMAKE_SOURCE_DIR}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Build via le Makefile existant (Linux)"
        VERBATIM
    )

    # Équivalent du "make clean" que faisaient compile.sh/run.sh avant
    # chaque build. Nom différent de "clean" (déjà réservé par CMake).
    add_custom_target(clean_avionics
        COMMAND $(MAKE) clean -C ${CMAKE_SOURCE_DIR}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "make clean via le Makefile existant (Linux)"
        VERBATIM
    )
endif()

# ==================================================================
# Tests unitaires (GoogleTest) — indépendants du Makefile ET de l'exe.
# CMake compile lui-même CORE_SOURCES + les fichiers de test : ça
# marche sur les deux OS, sans dépendre de SDL2 ni du build appli.
# ==================================================================
option(BUILD_TESTING "Compiler les tests unitaires" ON)

if(BUILD_TESTING AND EXISTS ${CMAKE_SOURCE_DIR}/src/test)
    include(FetchContent)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)  # OFF si build Windows /MT
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.15.2
    )
    FetchContent_MakeAvailable(googletest)

    enable_testing()
    include(GoogleTest)

    # GLOB_RECURSE (pas GLOB) : nécessaire pour descendre dans les
    # sous-dossiers miroirs de src/ (src/test/state/, src/test/math/Vec3/, ...)
    file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/test/*.cpp)

    add_executable(unit_tests ${CORE_SOURCES} ${TEST_SOURCES})
    target_include_directories(unit_tests PRIVATE ${CMAKE_SOURCE_DIR}/src)
    target_link_libraries(unit_tests PRIVATE gtest_main)

    gtest_discover_tests(unit_tests)
endif()
