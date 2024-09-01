#
# Set up our executable
add_executable(dependency_injection
    "dependency_injection.cpp"
 )

#
# Set output directory
set_target_properties(dependency_injection PROPERTIES
    CXX_STANDARD             20
    CXX_STANDARD_REQUIRED    TRUE
    RUNTIME_OUTPUT_DIRECTORY ${DEPENDENCY_INJECTION_OUTPUT_PATH})

# Set target includes
target_include_directories(dependency_injection
    PRIVATE ${CMAKE_SOURCE_DIR}/include
    PUBLIC  ${CMAKE_SOURCE_DIR}/include)
