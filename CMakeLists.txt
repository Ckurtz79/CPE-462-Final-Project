cmake_minimum_required(VERSION "3.17")
project(ImgProcFinal)
set(OpenCV_DIR /Users/jon/opencv/install/lib/cmake/opencv4)
include(CTest)
set(CMAKE_CXX_STANDARD 17)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
find_package( OpenCV REQUIRED )


include_directories( ${OpenCV_INCLUDE_DIRS} )


add_executable(IPRFin ImgProcSuite.cpp)


target_link_libraries(IPRFin ${OpenCV_LIBS} )
