cmake_minimum_required(VERSION 3.14)
project(InterferenceSim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(interference_sim
    lightinterference.cpp
    Wave.cpp
    PointSource.cpp
    SlitSource.cpp
    CompositeWave.cpp
    Screen.cpp
    InterferenceExperiment.cpp
)

target_compile_options(interference_sim PRIVATE
    # GCC / Clang
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra>

    # MSVC – warnings only; let CMake's per-config flags handle /Od vs /O2
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# On MSVC, strip /RTC1 (runtime checks) in Release/RelWithDebInfo because
# it is incompatible with optimisation flags (/O2, /O1).
if(MSVC)
    foreach(config RELEASE RELWITHDEBINFO)
        string(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_${config}
               "${CMAKE_CXX_FLAGS_${config}}")
    endforeach()
endif()