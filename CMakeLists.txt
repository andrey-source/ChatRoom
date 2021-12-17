cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
project(audio LANGUAGES CXX)
# set(RTAUDIO_API_ALSA 1)
set(RTAUDIO_API_PULSE 1)

add_compile_options(-pthread) 
add_link_options(-pthread)

option(ADDRESS_SANITIZER "Add address sanitizer")
option(UNDEFINED_SANITIZER "Add undefined sanitizer")
option(THREAD_SANITIZER "Add thread sanitizer")
option(LEAK_SANITIZER "Add leak sanitizer")



# if(ADDRESS_SANITIZER)
#     add_compile_options(-fsanitize=address)
#     add_link_options(-fsanitize=address)
# endif()
# if(UNDEFINED_SANITIZER)
#     add_compile_options(-fsanitize=undefined)
#     add_link_options(-fsanitize=undefined)
# endif()
# if(THREAD_SANITIZER)
#     add_compile_options(-fsanitize=thread)
#     add_link_options(-fsanitize=thread)
# endif()
# if(LEAK_SANITIZER)
#     add_compile_options(-fsanitize=leak)
#     add_link_options(-fsanitize=leak)
# endif()



add_subdirectory(rtaudio)
add_library(audio_lib STATIC audio/audio.cpp)
target_include_directories(audio_lib PUBLIC audio)
target_link_libraries(audio_lib PUBLIC rtaudio)



add_executable(main main.cpp application/application.cpp)
target_link_libraries(main audio_lib)
target_include_directories(main PUBLIC application)


# add_executable(audio audio.cpp)
# target_link_libraries(audio PUBLIC rtaudio)



# find_package(Boost)
# include_directories(${Boost_INCLUDE_DIRS})


# #client
# add_executable(client  client.cpp)
# target_link_libraries(client  ${Boost_LIBRARIES})

# #server
# add_executable(server server.h server.cpp connection.h connection.cpp)
# target_link_libraries(server ${Boost_LIBRARIES})

# #sync client 
# add_executable(sync_client  sync_client.cpp)
# target_link_libraries(sync_client ${Boost_LIBRARIES})

# #sync server
# add_executable(sync_server  sync_server.cpp)
# target_link_libraries(sync_server ${Boost_LIBRARIES})

# #sync server
# add_executable(my_server  my_server.cpp)
# target_link_libraries(my_server ${Boost_LIBRARIES})