project (robot)

cmake_minimum_required(VERSION 3.5)

set (CMAKE_CXX_STANDARD 11)

set (VERSION_MAJOR 0)
set (VERSION_MINOR 1)
set (DEBUG 0)


# This will pass arguments from cmake to the code
configure_file (
  "${PROJECT_SOURCE_DIR}/cmakeconfig.h.in"
  "${PROJECT_BINARY_DIR}/cmakeconfig.h"
)

# # This is to copy a file to the bin dir, will not be replaced if already there
# configure_file(
#    "${PROJECT_SOURCE_DIR}/logo.png"
#    "${PROJECT_BINARY_DIR}/logo.png"
#    COPYONLY
# )
# configure_file(
#     "${PROJECT_SOURCE_DIR}/assets/default.ply"
#     "${PROJECT_BINARY_DIR}/assets/default.ply"
#     COPYONLY
# )

#This will copy the file replacing it
file( GLOB TEST_IMAGES "${CMAKE_CURRENT_SOURCE_DIR}/test_images/*" )
foreach( file ${TEST_IMAGES} )
    file(
        COPY
        ${file}
        DESTINATION
        ${PROJECT_BINARY_DIR}
    )
endforeach( file ${TEST_IMAGES} )

find_library(wiringPi_LIB wiringPi)
# find_package(OpenGL REQUIRED)
find_package(OpenCV REQUIRED)
find_package(Boost COMPONENTS filesystem REQUIRED) # I spent 5 hours straight figuring out that I had to add "COMPONENTS filesystem" for the thing to work... dude
#find_package(Fann REQUIRED)

add_definitions("-lwiringPi -lcrypt -lm -lrt")
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

include_directories(
    ${PROJECT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}

    /libs/fann/include
    
    libs
    standard_libs
    /usr/local/include/opencv4
    /home/odroid/wiringPi


)


# link_directories(
#     libs/fann/lib
# )

# add_subdirectory(ownlibs)
# add_subdirectory(scripts)

#set(LIB_FOLDER
#    /usr/local/lib # Change this if you see it doesn't find the fann library
#)

add_subdirectory(libs)
add_subdirectory(standard_libs)

set(LIBS
	#${OPENGL_LIBRARY}
	
	robot
	common
    # /usr/local/lib/libfann.so.2   

    ${OpenCV_LIBS}
    ${Boost_LIBRARIES}
    ${wiringPi_LIB}
    pthread
    crypt
    m
    rt
	# "libfann.so"
	# "libfloatfann.so"
)


file( GLOB APP_SOURCES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" )
foreach( src_file ${APP_SOURCES} )
    # I used a simple string replace, to cut off .cpp.
    message(${src_file})
    string( REPLACE ".cpp" "" nme ${src_file} )
    string( REPLACE "src/" "" nme ${nme} )
    add_executable( ${nme} ${src_file} )
    # Make sure YourLib is linked to each app
    target_link_libraries( ${nme} ${LIBS})
endforeach( src_file ${APP_SOURCES} )

# add_executable(
#     "${PROJECT_NAME}_train"
# 	main_train.cpp
# )

# add_executable(
#     "${PROJECT_NAME}_execute"
# 	main_execute.cpp
# )

# target_link_libraries("${PROJECT_NAME}_train" ${LIBS})
# target_link_libraries("${PROJECT_NAME}_execute" ${LIBS})

