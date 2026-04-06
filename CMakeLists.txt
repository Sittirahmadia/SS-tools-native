cmake_minimum_required(VERSION 3.16)
project(SS-TOOLS-Improved VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Platform-specific settings
if(NOT WIN32)
    message(FATAL_ERROR "SS-TOOLS requires Windows platform")
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ═══════════════════════════════════════════════════════════════════════════
# DEPENDENCIES
# ═══════════════════════════════════════════════════════════════════════════

# ImGUI
set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/third_party/imgui")
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_win32.cpp
    ${IMGUI_DIR}/backends/imgui_impl_dx11.cpp
)

# Core header files
set(CORE_HEADERS
    include/core.h
    include/kernel_scanner.h
    include/false_positive_filter.h
    include/advanced_scanners.h
    include/imgui_interface.h
)

# Core source files
set(CORE_SOURCES
    src/kernel_scanner.cpp
    src/false_positive_filter.cpp
    src/advanced_scanners.cpp
    src/imgui_interface.cpp
    src/mods_scanner.cpp
    src/main.cpp
)

# ═══════════════════════════════════════════════════════════════════════════
# EXECUTABLE
# ═══════════════════════════════════════════════════════════════════════════

add_executable(ss-tools-improved
    ${CORE_HEADERS}
    ${CORE_SOURCES}
    ${IMGUI_SOURCES}
)

# Include directories
target_include_directories(ss-tools-improved PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

# ═══════════════════════════════════════════════════════════════════════════
# LIBRARIES & LINKING
# ═══════════════════════════════════════════════════════════════════════════

# Windows libraries
target_link_libraries(ss-tools-improved PRIVATE
    # Core Windows APIs
    kernel32
    user32
    gdi32
    shell32
    ole32
    oleaut32
    uuid
    comdlg32
    advapi32
    comctl32
    shlwapi
    
    # Process/Memory APIs
    psapi
    ntdll
    
    # Security/Crypto
    crypt32
    wintrust
    
    # COM/WMI
    wbemuuid
    
    # DirectX 11 (for ImGUI rendering)
    d3d11
    dxgi
    d3dcompiler
)

# ═══════════════════════════════════════════════════════════════════════════
# COMPILER FLAGS
# ═══════════════════════════════════════════════════════════════════════════

if(MSVC)
    # MSVC-specific flags
    target_compile_options(ss-tools-improved PRIVATE
        /W4              # Warning level 4
        /WX              # Warnings as errors
        /Ox              # Optimization
        /EHsc            # Exception handling
        /permissive-     # Standards conformance
    )
    
    # Multi-threaded runtime
    target_compile_options(ss-tools-improved PRIVATE /MT)
else()
    # GCC/Clang flags
    target_compile_options(ss-tools-improved PRIVATE
        -Wall
        -Wextra
        -O3
    )
endif()

# ═══════════════════════════════════════════════════════════════════════════
# RESOURCE FILE (Icon, Version Info)
# ═══════════════════════════════════════════════════════════════════════════

if(MSVC)
    enable_language(RC)
    set(RC_FILE "${CMAKE_SOURCE_DIR}/resources/resource.rc")
    if(EXISTS ${RC_FILE})
        target_sources(ss-tools-improved PRIVATE ${RC_FILE})
    endif()
endif()

# ═══════════════════════════════════════════════════════════════════════════
# BUILD OPTIONS
# ═══════════════════════════════════════════════════════════════════════════

option(ENABLE_KERNEL_SCAN "Enable kernel-level scanning" ON)
option(ENABLE_MEMORY_SCAN "Enable memory scanning" ON)
option(ENABLE_PROCESS_SCAN "Enable process scanning" ON)
option(BUILD_TESTS "Build test suite" OFF)

# Define preprocessor flags
if(ENABLE_KERNEL_SCAN)
    target_compile_definitions(ss-tools-improved PRIVATE ENABLE_KERNEL_SCAN)
endif()

if(ENABLE_MEMORY_SCAN)
    target_compile_definitions(ss-tools-improved PRIVATE ENABLE_MEMORY_SCAN)
endif()

if(ENABLE_PROCESS_SCAN)
    target_compile_definitions(ss-tools-improved PRIVATE ENABLE_PROCESS_SCAN)
endif()

# ═══════════════════════════════════════════════════════════════════════════
# INSTALLATION
# ═══════════════════════════════════════════════════════════════════════════

install(TARGETS ss-tools-improved
    RUNTIME DESTINATION bin
)

install(DIRECTORY resources/
    DESTINATION resources
    OPTIONAL
)

# ═══════════════════════════════════════════════════════════════════════════
# TESTING
# ═══════════════════════════════════════════════════════════════════════════

if(BUILD_TESTS)
    enable_testing()
    # Add test sources here
endif()

# ═══════════════════════════════════════════════════════════════════════════
# SUMMARY
# ═══════════════════════════════════════════════════════════════════════════

message(STATUS "")
message(STATUS "SS-TOOLS Improved Configuration Summary")
message(STATUS "========================================")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Kernel Scan: ${ENABLE_KERNEL_SCAN}")
message(STATUS "Memory Scan: ${ENABLE_MEMORY_SCAN}")
message(STATUS "Process Scan: ${ENABLE_PROCESS_SCAN}")
message(STATUS "")
