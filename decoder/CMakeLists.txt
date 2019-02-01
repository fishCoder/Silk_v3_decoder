cmake_minimum_required(VERSION 3.0.0)
project(silk_v3_decoder)

include_directories(src/main/cpp/include)

aux_source_directory(src/main/cpp/libsilk LIB_SILK)
aux_source_directory(src/main/cpp/libmad LIB_MAD)
aux_source_directory(src/main/cpp/libwav LIB_WAV)
aux_source_directory(src/main/cpp/libmp3lame LIB_LAME)
aux_source_directory(src/main/cpp/ JNI_SRC)

add_library(silk_v3_decoder SHARED ${JNI_SRC} ${LIB_SILK} ${LIB_MAD} ${LIB_LAME} ${LIB_WAV})
find_library(android-log log)

target_link_libraries(silk_v3_decoder ${android-log})