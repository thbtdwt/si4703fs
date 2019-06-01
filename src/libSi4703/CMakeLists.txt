
set(EXE si4703)

add_compile_options("$<$<CONFIG:RELEASE>:-O2>")
add_compile_options("$<$<CONFIG:DEBUG>:-O0;-g;-DDEBUG_TRACE_ON>")

add_compile_options(-Wall -Werror -Wextra -std=c++1z)
add_definitions(-D_FILE_OFFSET_BITS=64)

set(SRC
    si4703_exception.cpp
    si4703_low.cpp
    si4703_high.cpp
    )

#set(CMAKE_INSTALL_PREFIX "/" CACHE PATH "" FORCE)

add_executable(${EXE} ${SRC})
#target_link_libraries(${EXE} ${FUSE_LIBRARIES})

#install(TARGETS ${EXE} DESTINATION usr/bin)
#install(FILES max9744d.service DESTINATION etc/systemd/system)
