set(example_programs
    2d_stencil_1
    2d_stencil_2
    2d_stencil_3
    test
)

foreach(example_program ${example_programs})

  set(${example_program}_FLAGS COMPONENT_DEPENDENCIES iostreams)

  set(sources ${example_program}.cpp)

  source_group("Source Files" FILES ${sources})

  add_hpx_executable(
    ${example_program}
    SOURCES ${sources} ${${example_program}_FLAGS}
  )

endforeach()
