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
file(
    COPY
    ${PROJECT_SOURCE_DIR}/test_images/logo.png
    ${PROJECT_SOURCE_DIR}/test_images/test1.png
	${PROJECT_SOURCE_DIR}/test_images/test2.png
	${PROJECT_SOURCE_DIR}/test_images/test3.jpg
    DESTINATION
    ${PROJECT_BINARY_DIR}
)


# find_package(OpenGL REQUIRED)
find_package(OpenCV REQUIRED)
find_package(Boost COMPONENTS filesystem REQUIRED) # I spent 5 hours straight figuring out that I had to add "COMPONENTS filesystem" for the thing to work, I'm taking a break now
#find_package(Fann REQUIRED)

include_directories(
    ${PROJECT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}
	
    libs
)


# link_directories(
#     libs/core/allegro5/lib
#     libs/core/assimp/lib
#     libs/core/glm/lib
# )

# add_subdirectory(ownlibs)
# add_subdirectory(scripts)

#SET(GCC_COVERAGE_LINK_FLAGS "-libfann")
#SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")

set(LIBS
    # ${OPENGL_LIBRARY}
    
	${OpenCV_LIBS}
	${Boost_LIBRARIES}
	
	/usr/local/lib/libfann.so
	/usr/local/lib/libfloatfann.so
)

add_executable(
    "${PROJECT_NAME}_train"
	main_common.cpp
	main_train.cpp
	comands.cpp
	logic_process.cpp
    img_process.cpp
	nnetwork.cpp
    misc.cpp
)
# add_executable(
#     "${PROJECT_NAME}_execute"
	
# 	main_execute.cpp
# 	main_common.cpp
# 	comands.cpp
# 	logic_process.cpp
#   img_process.cpp
# 	nnetwork.cpp
# )


target_link_libraries("${PROJECT_NAME}_train" ${LIBS})
# target_link_libraries("${PROJECT_NAME}_execute" ${LIBS})

