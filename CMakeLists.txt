﻿# CMakeList.txt : projet CMake pour MyDS, incluez la source et définissez
# la logique spécifique au projet ici.
#
cmake_minimum_required (VERSION 3.8)

# Activez Rechargement à chaud pour les compilateurs MSVC si cela est pris en charge.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("MyDS")

# Ajoutez une source à l'exécutable de ce projet.
add_executable (MyDS "src/MyDS.cpp" "src/MyDS.h" "src/Cpu.h" "src/Cpu.cpp"  "src/arm9_mem.h" "src/arm7_mem.h" "src/arm_mem.cpp" "src/arm_mem.h"   "src/ndsrom.h" "src/ndsrom.cpp" "src/instructions.h"   "src/instructions.cpp"  "src/breakpoints.h" "src/breakpoints.cpp" "src/cpu_instructions.cpp" "src/cpu_misc_instructions.cpp" "src/cpu_multiply_instructions.cpp" "src/cpu_extraloadstore_instructions.cpp" "src/cpu_media_instructions.cpp" "src/cpu_unconditional_instructions.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET MyDS PROPERTY CXX_STANDARD 20)
endif()

# TODO: Ajoutez des tests et installez des cibles si nécessaire.
