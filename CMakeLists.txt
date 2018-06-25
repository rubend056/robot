project (robot)

cmake_minimum_required(VERSION 3.5)

set (CMAKE_CXX_STANDARD 11)

set (VERSION_MAJOR 0)
set (VERSION_MINOR 1)
set (DEBUG 1)

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
file(
    COPY
    ${PROJECT_SOURCE_DIR}/logo.png
    ${PROJECT_SOURCE_DIR}/test1.png
    ${PROJECT_SOURCE_DIR}/test2.png
    DESTINATION
    ${PROJECT_BINARY_DIR}
)


# find_package(OpenGL REQUIRED)
find_package(OpenCV REQUIRED)

include_directories(
    ${PROJECT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}

    
)

# link_directories(
#     libs/core/allegro5/lib
#     libs/core/assimp/lib
#     libs/core/glm/lib
# )

# add_subdirectory(ownlibs)
# add_subdirectory(scripts)

set(LIBS
    # ${OPENGL_LIBRARY}
    
    ${OpenCV_LIBS}
)

add_executable(
    ${PROJECT_NAME}
    
	#camera.cpp
    main.cpp
	
)

target_link_libraries(${PROJECT_NAME} ${LIBS})
