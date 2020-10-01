file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS *.hpp)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS *.cpp)

add_binary_bundle(proj1_shaders
        NAME grid_vert PATH "shaders/grid.vert"
        NAME grid_vec_geom PATH "shaders/grid_vec.geom"
        NAME grid_tetra_geom PATH "shaders/grid_tetra.geom"
        NAME grid_plain_frag PATH "shaders/grid_plain.frag"
        NAME grid_phong_frag PATH "shaders/grid_pong.frag")

add_executable(proj1 ${SOURCES} ${HEADERS})
target_link_libraries(proj1 PRIVATE glpp glfw glm proj1_shaders imgui commons)
#target_compile_options(proj1 PRIVATE -pg)
#target_link_options(proj1 PRIVATE -pg)