cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 23)

project("ctr-plugin" LANGUAGES C CXX ASM)

set(CMAKE_EXE_LINKER_FLAGS "-L${CTR_ROOT}/lib -L${DEVKITPRO}/portlibs/3ds/lib -L${DEVKITPRO}/libctrpf/lib -T ${PROJECT_SOURCE_DIR}/3gx.ld -Os -Wl,-Map,${PROJECT_NAME}.map,--gc-sections")
set(CMAKE_CXX_STANDARD_LIBRARIES "-lm -lctrpf -lctru")

file(GLOB_RECURSE CXX_SOURCE 
"${PROJECT_SOURCE_DIR}/src/*.cpp"
"${PROJECT_SOURCE_DIR}/vendor/mk7-memory/vendor/sead/modules/src/math/*.cpp"
"${PROJECT_SOURCE_DIR}/vendor/mk7-memory/vendor/sead/modules/src/random/*.cpp"
)

add_executable(${PROJECT_NAME} ${CXX_SOURCE}) # create .elf file

target_include_directories(${PROJECT_NAME} PUBLIC 
"${PROJECT_SOURCE_DIR}/include"
"${PROJECT_SOURCE_DIR}/include/sead/include"
"${DEVKITPRO}/libctrpf/include"
"${DEVKITPRO}/libctru/include"
)

dkp_target_generate_symbol_list(${PROJECT_NAME})