cmake_minimum_required(VERSION 3.15)
project(medicore_hospital_system C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

# Include directories
include_directories(
    include
    include/third_party
    include/third_party/asio
)

# SQLite library compiled as C
add_library(sqlite3 STATIC src/third_party/sqlite3.c)

# Source files for core DSA, Repositories, and Services
set(MEDICORE_CORE_SRCS
    src/dsa/patient_hash_table.cpp
    src/dsa/visit_linked_list.cpp
    src/dsa/appointment_queue.cpp
    src/dsa/emergency_heap.cpp
    src/dsa/doctor_bst.cpp
    src/dsa/undo_stack.cpp
    src/dsa/hospital_graph.cpp
    src/dsa/dsa_algorithms.cpp
    src/repositories/database.cpp
    src/services/hospital_service.cpp
)

# 1. Main REST API Server Executable
add_executable(medicore_server
    src/main.cpp
    ${MEDICORE_CORE_SRCS}
)

if (WIN32)
    target_link_libraries(medicore_server PRIVATE sqlite3 ws2_32 mswsock)
else()
    target_link_libraries(medicore_server PRIVATE sqlite3 pthread dl)
endif()

# 2. Automated DSA Test Suite Executable
add_executable(test_dsa
    tests/test_dsa.cpp
    src/dsa/patient_hash_table.cpp
    src/dsa/visit_linked_list.cpp
    src/dsa/appointment_queue.cpp
    src/dsa/emergency_heap.cpp
    src/dsa/doctor_bst.cpp
    src/dsa/undo_stack.cpp
    src/dsa/hospital_graph.cpp
    src/dsa/dsa_algorithms.cpp
)

# 3. Automated Backend & Database Integration Test Suite Executable
add_executable(test_backend
    tests/test_backend.cpp
    ${MEDICORE_CORE_SRCS}
)

if (WIN32)
    target_link_libraries(test_backend PRIVATE sqlite3 ws2_32 mswsock)
else()
    target_link_libraries(test_backend PRIVATE sqlite3 pthread dl)
endif()

enable_testing()
add_test(NAME DSASuite COMMAND test_dsa)
add_test(NAME BackendSuite COMMAND test_backend)
