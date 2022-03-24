cmake_minimum_required(VERSION 3.6)

#project(libcamera-still)
#add_executable(libcamera-still libcamera_still.cpp)
#target_link_libraries(libcamera-still libcamera_app images)

#project(libcamera-vid)
#add_executable(libcamera-vid libcamera_vid.cpp)
#target_link_libraries(libcamera-vid libcamera_app encoders outputs)

#project(libcamera-hello)
#add_executable(libcamera-hello libcamera_hello.cpp)
#target_link_libraries(libcamera-hello libcamera_app)

project(libcamera-raw)
add_executable(libcamera-raw libcamera_raw.cpp)
target_link_libraries(libcamera-raw libcamera_app encoders outputs)

#project(libcamera-jpeg)
#add_executable(libcamera-jpeg libcamera_jpeg.cpp)
#target_link_libraries(libcamera-jpeg libcamera_app images)

set(EXECUTABLES libcamera-raw)

if (ENABLE_TFLITE)
    project(libcamera-detect)
    add_executable(libcamera-detect libcamera_detect.cpp)
    target_link_libraries(libcamera-detect libcamera_app images)
    set(EXECUTABLES ${EXECUTABLES} libcamera-detect)
endif()

set(EXECUTABLE_OUTPUT_PATH  ${CMAKE_BINARY_DIR})
install(TARGETS ${EXECUTABLES} RUNTIME DESTINATION bin)


add_library(camera_raw_api libcamera_raw_api.cpp buffer_output.cpp)
target_link_libraries(camera_raw_api  libcamera_app images encoders outputs)
set_target_properties(camera_raw_api PROPERTIES PUBLIC_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/libcamera_raw_api.h)


add_library(camera_vid_api libcamera_vid_api.cpp buffer_output.cpp)
target_link_libraries(camera_vid_api  libcamera_app images encoders outputs)
set_target_properties(camera_vid_api PROPERTIES PUBLIC_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/libcamera_vid_api.h)


INSTALL(TARGETS camera_raw_api 
        LIBRARY DESTINATION lib
		RUNTIME DESTINATION bin
        PUBLIC_HEADER DESTINATION include)

INSTALL(TARGETS camera_vid_api 
        LIBRARY DESTINATION lib
		RUNTIME DESTINATION bin
        PUBLIC_HEADER DESTINATION include)

#set(head_files
#	${CMAKE_CURRENT_SOURCE_DIR}/libcamera_raw_api.h)
	
#install(TARGETS ${head_files} DESTINATION  ${CMAKE_BINARY_DIR}/include)


