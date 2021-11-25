cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(audio)
# set(RTAUDIO_API_ALSA 1)
set(RTAUDIO_API_PULSE 1)


add_subdirectory(rtaudio)
# add_executable(input record.cpp)
# target_link_libraries(input PUBLIC rtaudio)

# add_executable(play play.cpp)
# target_link_libraries(play PUBLIC rtaudio)

# add_executable(duplex_method duplex.cpp)
# target_link_libraries(duplex_method PUBLIC rtaudio)



add_executable(audio audio.cpp)
target_link_libraries(audio PUBLIC rtaudio)


# add_executable(play1 play1.cpp)
# target_link_libraries(play1 PUBLIC rtaudio)