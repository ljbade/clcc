add_executable(clcc clcc.c compiler.c load_compiler_unx.c)
target_link_libraries(clcc ${CMAKE_DL_LIBS})
