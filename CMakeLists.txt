cmake_minimum_required (VERSION 2.6)
project (AVR)

set(PYTHON_EXECUTABLE   "python")
set(MCU                 "atmega2560")
set(F_CPU               "16000000")
set(OPT                 "s")
set(DEBUG               "dwarf-2")
set(CSTANDARD           "-std=gnu99")
set(CXXSTANDARD         "-std=gnu++11")

set(COMMONDEFS  "-mmcu=${MCU} -DF_CPU=${F_CPU}UL -funsigned-char -funsigned-bitfields -fshort-enums -Wall -fdata-sections -ffunction-sections -Wl,--gc-sections")
set(CMAKE_C_FLAGS   "-g${DEBUG} ${COMMONDEFS} -O${OPT} -fpack-struct -Wstrict-prototypes ${CSTANDARD}")
set(CMAKE_CXX_FLAGS "-g${DEBUG} ${COMMONDEFS} -O${OPT}  -fno-exceptions -Wundef ${CXXSTANDARD}")

file(GLOB_RECURSE source_files "src/*.c" "src/*.cpp" "src/*.hpp")
include_directories("src" "module")

MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
      LIST(APPEND dirlist ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()

SUBDIRLIST(modules ${CMAKE_SOURCE_DIR}/module)

add_custom_command(
    OUTPUT ${CMAKE_SOURCE_DIR}/application.cpp
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/generate_application.py ${CMAKE_SOURCE_DIR}/src ${modules}
    DEPENDS ${CMAKE_SOURCE_DIR}/generate_application.py
    COMMENT "Generating code for application."
)

add_custom_target(GenerateApplication DEPENDS ${CMAKE_SOURCE_DIR}/application.cpp)
foreach(module ${modules})
    add_subdirectory(module/${module})
endforeach()

add_executable(AVR ${source_files})
add_dependencies(AVR GenerateApplication)
set_target_properties(AVR PROPERTIES OUTPUT_NAME "avr.elf")

set(EXTMEMOPTS  "")
set(PRINTFLIB   "-Wl,-u,vfprintf -lprintf_flt")
set(SCANFLIB    "")
set(MATHLIB     "-lm")
target_link_libraries(AVR ${PRINTFLIB} ${EXTMEMOPTS} ${SCANFLIB} ${MATHLIB} ${modules})
