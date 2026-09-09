__component_add_include_dirs(.)

# The KW307 driver, history ring and analysis live in components/radar so a
# product app can use them without copying this probe.
target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/main.c)

__set_main_file(main.c)
setup_project(kw307_radar_test)
