file(GLOB EXTERN_DIRS LIST_DIRECTORIES true CONFIGURE_DEPENDS */)
foreach (EXTERN_DIR ${EXTERN_DIRS})
    if (EXISTS "${EXTERN_DIR}/CMakeLists.txt")
        add_subdirectory("${EXTERN_DIR}")
    endif ()
endforeach ()

include(FetchContent)
# Add ImGui
FetchContent_Declare(imgui GIT_REPOSITORY https://github.com/ocornut/imgui.git)
FetchContent_Populate(imgui)
set(IMGUI_ROOT "${imgui_SOURCE_DIR}")
file(GLOB IMGUI_SOURCES CONFIGURE_DEPENDS ${IMGUI_ROOT}/*.cpp ${IMGUI_ROOT}/*.h)
set(IMGUI_IMPL_GLFW "${IMGUI_ROOT}/examples/imgui_impl_glfw.cpp" "${IMGUI_ROOT}/examples/imgui_impl_glfw.h")
set(IMGUI_IMPL_OPENGL "${IMGUI_ROOT}/examples/imgui_impl_opengl3.cpp" "${IMGUI_ROOT}/examples/imgui_impl_opengl3.h")
add_library(imgui STATIC ${IMGUI_SOURCES} ${IMGUI_IMPL_GLFW} ${IMGUI_IMPL_OPENGL})
target_include_directories(imgui PUBLIC ${IMGUI_ROOT} ${IMGUI_ROOT}/examples)
target_link_libraries(imgui PUBLIC glad glfw)
target_compile_definitions(imgui PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLAD)

# Add GLFW3
include(FetchContent)
if (NOT TARGET glfw)
    FetchContent_Declare(glfw3 GIT_REPOSITORY https://github.com/glfw/glfw.git)
    FetchContent_MakeAvailable(glfw3)
endif ()