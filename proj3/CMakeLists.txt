file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS *.hpp)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS *.cpp)

add_binary_bundle(proj3_shaders
        NAME phong_vert PATH "shaders/phong.vert"
        NAME phong_frag PATH "shaders/phong.frag")

add_executable(proj3 ${SOURCES} ${HEADERS})
target_link_libraries(proj3 PRIVATE glpp glfw commons proj3_shaders imgui)