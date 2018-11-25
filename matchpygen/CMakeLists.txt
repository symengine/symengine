project(matchpygen)

include_directories(BEFORE ${symengine_SOURCE_DIR})
include_directories(BEFORE ${symengine_BINARY_DIR})
include_directories(BEFORE ${teuchos_SOURCE_DIR})
include_directories(BEFORE ${teuchos_BINARY_DIR})

# find_package(Boost REQUIRED)  # 1.65.1 COMPONENTS coroutine2 context REQUIRED)
#find_package(Boost COMPONENTS system filesystem coroutine context REQUIRED)
#include_directories(${Boost_INCLUDE_DIR})
#link_directories(${Boost_LIBRARY_DIR})

#SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -lboost_context -lboost_coroutine ")
#set(CMAKE_VERBOSE_MAKEFILE on)

#link_directories(${Boost_LIBRARY_DIRS})
#include_directories(${Boost_INCLUDE_DIR})

#set(BOOST_PARAMS "-I/usr/include/  -lboost_context -lboost_coroutine")
#message(WARNING ${BOOST_PARAMS})

add_executable(test_bipartite test_bipartite.cpp)
target_link_libraries(test_bipartite symengine)
