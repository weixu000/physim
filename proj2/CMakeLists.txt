file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS *.hpp)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS *.cpp)

add_binary_bundle(proj2_shaders
        NAME sph_vert PATH "shaders/sph.vert"
        NAME sph_frag PATH "shaders/sph.frag")

add_executable(proj2 ${SOURCES} ${HEADERS})
target_link_libraries(proj2 PRIVATE glpp glfw commons proj2_shaders)
