file(GLOB_RECURSE PROJ1_HEADERS CONFIGURE_DEPENDS *.hpp)
file(GLOB_RECURSE PROJ1_SOURCES CONFIGURE_DEPENDS *.cpp)

add_executable(proj1 ${PROJ1_SOURCES} ${PROJ1_HEADERS})
target_link_libraries(proj1 PRIVATE glbinding::glbinding glbinding::glbinding-aux globjects::globjects glfw glm)