cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(aieBootstrap)

add_library(${PROJECT_NAME}
    Application.cpp
    Font.cpp
    Gizmos.cpp
    Input.cpp
    Renderer2D.cpp
    Texture.cpp
    gl_core_4_4.c
    imgui_glfw3.cpp
    ${CMAKE_SOURCE_DIR}/dependencies/imgui/imgui_draw.cpp 
    ${CMAKE_SOURCE_DIR}/dependencies/imgui/imgui.cpp 
 )

target_link_libraries(${PROJECT_NAME}
    glfw
    ${OPENGL_gl_LIBRARY}
    )

target_include_directories(${PROJECT_NAME} PUBLIC
    ${PROJECT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/dependencies/glm
    ${CMAKE_SOURCE_DIR}/dependencies/imgui
    ${CMAKE_SOURCE_DIR}/dependencies/stb
    ${CMAKE_SOURCE_DIR}/dependencies/glfw/include
    ${CMAKE_SOURCE_DIR}/dependencies/openal/include
    )
