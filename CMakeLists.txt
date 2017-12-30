# Emu64 CMakeLists.txt
# by thorsten kattanek

cmake_minimum_required (VERSION 3.7)

project (emu64) 

# Holen des aktuellen Arbeitsbranches
execute_process(
  COMMAND git rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Holen des Hashecodes vom letzten Commits
execute_process(
  COMMAND git log -1 --format=%h
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_COMMIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Version aus dem Tag ermitteln
execute_process(
  COMMAND git describe --always --tags
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE VERSION_STRING
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

add_definitions("-DGIT_COMMIT_HASH=${GIT_COMMIT_HASH}")
add_definitions("-DGIT_BRANCH=${GIT_BRANCH}")
add_definitions("-DVERSION_STRING=${VERSION_STRING}")

# ArchitectureString
add_definitions(-Dstr_system_arch="\\"LINUX\\"")

configure_file(
  ${CMAKE_SOURCE_DIR}/src/version.h.in
  ${CMAKE_BINARY_DIR}/src/version.h
)

add_subdirectory(src)
