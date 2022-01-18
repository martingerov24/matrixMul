cmake_minimum_required(VERSION 3.20)

project(matrixMultiplication LANGUAGES CXX CUDA)
set(TOOLKIT_ROOT_DIR $ENV{CUDA_PATH})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
	main.cpp
	external/matrix.h
	external/CudaMatrix.h
	external/multiplication.cu
)

add_executable(matrixMultiplication ${SOURCES})

message(STATUS "====== OpenCV Located: ${OPENCV_PATH}")
message(STATUS "====== OpenCV LIBS: ${OpenCV_LIBS}")
message(STATUS "====== CUDA Located: ${CUDA_INCLUDE_DIRS}")
message(STATUS "====== IMGUI Located: ${IMGUI_DIR}")
message(STATUS "====== OpenGL Located: ${OPENGL_LIBRARIES}")

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CUDA_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    ${CUDA_LIBS}
    ${CMAKE_DL_LIBS}
)
target_compile_options(${PROJECT_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CUDA>:
                       --generate-line-info
                       --use_fast_math
                       >)

target_compile_options(${PROJECT_NAME} PRIVATE $<$<COMPILE_LANGUAGE:MSVC>:
                       >)