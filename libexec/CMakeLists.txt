project_variable(LIBEXEC_DIR ${CMAKE_INSTALL_LIBEXECDIR})
if (cetmodules_IN_TREE AND TARGET cetmodules::filter-output)
  set(only_qual ONLY_QUALIFIED_TARGET)
else()
  set(only_qual)
endif()
cet_script(filter-output ${only_qual}
  DESTINATION ${${PROJECT_NAME}_LIBEXEC_DIR}
)
