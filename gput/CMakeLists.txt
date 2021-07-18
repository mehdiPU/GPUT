cmake_minimum_required(VERSION 3.10)

project(gput)

set(GPUT_DEPENDENCIES_DIR dependencies)

set(GPUT_DEPENDENCIES
   c-logger
   glad
)

set(GPUT_LINK_LIBS
   EGL
   gbm
   logger_static
   glad
)

set(GPUT_SRC_FILES
   src/gput.c
   src/gputDebug.c
)

set(GPUT_EXTERN_INCLUDE_DIRS
   include
)

add_library(${PROJECT_NAME} STATIC ${GPUT_SRC_FILES})

# TODO this should be bound to a build configuration
target_compile_definitions(${PROJECT_NAME}
   PRIVATE GPUT_DEBUG
)

foreach(DEPENDENCY IN LISTS GPUT_DEPENDENCIES)

   add_subdirectory(${GPUT_DEPENDENCIES_DIR}/${DEPENDENCY})

endforeach()

target_include_directories(${PROJECT_NAME}
   INTERFACE ${GPUT_EXTERN_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PUBLIC ${GPUT_LINK_LIBS})
