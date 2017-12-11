# Emu64 CMakeLists.txt
# by thorsten kattanek

cmake_minimum_required (VERSION 3.7)

project (emu64) 

# VersionString
add_definitions(-Dstr_emu64_version="\\"5.0.16\\"")

# ArchitectureString
add_definitions(-Dstr_system_arch="\\"LINUX\\"")

add_subdirectory(src)
