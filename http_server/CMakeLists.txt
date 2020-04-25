cmake_minimum_required(VERSION 3.17)

project(${HTTP_SERVER_NAME} CXX)

set(HTTP_SERVER_SRCS
    "${PROJECT_SOURCE_DIR}/src/http/http.cpp"
    "${PROJECT_SOURCE_DIR}/src/http/request.cpp"
    "${PROJECT_SOURCE_DIR}/src/http/response.cpp"

    "${PROJECT_SOURCE_DIR}/src/utils/file_descriptor.cpp"
    "${PROJECT_SOURCE_DIR}/src/utils/parser.cpp"

    "${PROJECT_SOURCE_DIR}/src/exception.cpp"
    "${PROJECT_SOURCE_DIR}/src/connection.cpp"
    "${PROJECT_SOURCE_DIR}/src/server.cpp"
    "${PROJECT_SOURCE_DIR}/src/main.cpp"
)

find_package (Threads)

add_executable(${PROJECT_NAME} ${HTTP_SERVER_SRCS})

target_link_libraries(${PROJECT_NAME}
    ${CMAKE_THREAD_LIBS_INIT}
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        "${PROJECT_SOURCE_DIR}/include"
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/${PROJECT_NAME}"
)