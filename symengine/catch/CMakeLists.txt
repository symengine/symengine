project(catch)

set(SRC
    catch.cpp
)

add_library(catch STATIC ${SRC})

install(TARGETS catch
            RUNTIME DESTINATION bin
            ARCHIVE DESTINATION lib
            LIBRARY DESTINATION lib
        )
