project(test_metal)

if (NOT APPLE)
    message(FATAL_ERROR "BUILD_METAL_TESTS requires macOS")
endif()

if (NOT METALCPP_ROOT)
    message(FATAL_ERROR
            "BUILD_METAL_TESTS requires METALCPP_ROOT to point to the extracted metal-cpp headers")
endif()

find_library(FOUNDATION_FRAMEWORK Foundation REQUIRED)
find_library(METAL_FRAMEWORK Metal REQUIRED)
find_library(QUARTZCORE_FRAMEWORK QuartzCore REQUIRED)

add_executable(test_metalcode_runtime
               test_metalcode_runtime.cpp)
target_compile_features(test_metalcode_runtime PRIVATE cxx_std_17)
target_include_directories(test_metalcode_runtime PRIVATE
                           ${METALCPP_ROOT})
target_link_libraries(test_metalcode_runtime PRIVATE
                      symengine
                      catch
                      ${FOUNDATION_FRAMEWORK}
                      ${METAL_FRAMEWORK}
                      ${QUARTZCORE_FRAMEWORK})

add_test(NAME test_metalcode_runtime COMMAND $<TARGET_FILE:test_metalcode_runtime>)
