cmake_minimum_required(VERSION 3.10)

# set the project name
project(yeti_comms VERSION 0.1)
# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)


configure_file(config.h.in config.h)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# add the executable
add_library(yeti_comms STATIC evSerial.cpp ../common/evThread.cpp protobuf/hi2lo.pb.c protobuf/lo2hi.pb.c nanopb/pb_decode.c nanopb/pb_encode.c nanopb/pb_common.c)

target_include_directories(yeti_comms PUBLIC "${PROJECT_BINARY_DIR}" PUBLIC "nanopb" PUBLIC "protobuf" PUBLIC "../common")
target_link_libraries(yeti_comms PRIVATE Pal::Sigslot Threads::Threads)

target_link_libraries(yeti_comms
    PRIVATE
        date::date
        date::date-tz
)
