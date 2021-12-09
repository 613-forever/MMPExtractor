cmake_minimum_required(VERSION 3.10)
project(MMPExtractor)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

find_package(Common613 1.1.0 REQUIRED)

add_executable(MMPExtractor_GIMFromCSB extract_gim_from_csb.cpp)
target_link_libraries(MMPExtractor_GIMFromCSB PUBLIC Common613::Common613)

add_executable(MMPExtractor_GIM16 gim16.cpp)
target_include_directories(MMPExtractor_GIM16 PUBLIC ${OpenCV_INCLUDE_DIRS})
target_link_libraries(MMPExtractor_GIM16 PUBLIC ${OpenCV_LIBS} Common613::Common613)

add_executable(MMPExtractor_StandAssemble main.cpp entrances.h entrances.cpp png_buffer.h png_buffer.cpp)
target_include_directories(MMPExtractor_StandAssemble PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(MMPExtractor_StandAssemble PRIVATE ${OpenCV_LIBS} Common613::Common613)

add_executable(MMPExtractor_StandPos pos.cpp)
target_link_libraries(MMPExtractor_StandPos PRIVATE Common613::Common613)
