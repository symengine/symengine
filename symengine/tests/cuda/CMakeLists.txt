find_package(CUDAToolkit REQUIRED)
configure_file(cudacode_runtime_config.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/cudacode_runtime_config.h
               @ONLY)

add_executable(test_cudacode_runtime
               test_cudacode_runtime.cpp)
target_include_directories(test_cudacode_runtime PRIVATE
                           ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(test_cudacode_runtime PRIVATE
                      catch
                      CUDA::cudart
                      CUDA::cuda_driver
                      CUDA::nvrtc)

add_test(NAME test_cudacode_runtime COMMAND $<TARGET_FILE:test_cudacode_runtime>)
