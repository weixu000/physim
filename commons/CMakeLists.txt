file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS *.hpp)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS *.cpp)

add_binary_bundle(common_shaders
        NAME axes_vert PATH "shaders/axes.vert"
        NAME axes_frag PATH "shaders/axes.frag"
        )

add_library(commons ${HEADERS} ${SOURCES})
target_link_libraries(commons
        PUBLIC glpp glm::glm
        PRIVATE common_shaders)
target_include_directories(commons PUBLIC .)
