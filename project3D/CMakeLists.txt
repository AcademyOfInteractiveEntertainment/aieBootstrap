cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(project3D)

add_executable(${PROJECT_NAME}
    main.cpp
    Application3D.cpp
    )
target_link_libraries(${PROJECT_NAME} aieBootstrap)
