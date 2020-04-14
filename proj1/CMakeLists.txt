file(GLOB_RECURSE PROJ1_HEADERS CONFIGURE_DEPENDS *.hpp)
file(GLOB_RECURSE PROJ1_SOURCES CONFIGURE_DEPENDS *.cpp)

add_binary_bundle(shaders
        NAME axes_vert PATH "shaders/axes.vert"
        NAME axes_frag PATH "shaders/axes.frag"
        NAME grid_vert PATH "shaders/grid.vert"
        NAME grid_vec_geom PATH "shaders/grid_vec.geom"
        NAME grid_tetra_geom PATH "shaders/grid_tetra.geom"
        NAME grid_plain_frag PATH "shaders/grid_plain.frag"
        NAME grid_phong_frag PATH "shaders/grid_pong.frag")

add_executable(proj1 ${PROJ1_SOURCES} ${PROJ1_HEADERS})
target_link_libraries(proj1 PRIVATE glpp glfw glm shaders imgui)
#target_compile_options(proj1 PRIVATE -pg)
#target_link_options(proj1 PRIVATE -pg)