cmake_minimum_required (VERSION 3.8)

find_package(Threads REQUIRED)
add_executable (PackageSolution "PackageSolution.cpp" "PackageSolution.h")
set_property(TARGET PackageSolution PROPERTY CXX_STANDARD 17)
target_link_libraries(PackageSolution PRIVATE Threads::Threads)