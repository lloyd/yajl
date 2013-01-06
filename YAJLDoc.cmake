FIND_PROGRAM(doxygenPath doxygen)

IF (doxygenPath)
  SET(YAJL_VERSION ${YAJL_MAJOR}.${YAJL_MINOR}.${YAJL_MICRO})
  SET(docPath "${CMAKE_BINARY_DIR}/share")
  MESSAGE("-- Using doxygen at: ${doxygenPath}")
  MESSAGE("-- Documentation output to: ${docPath}")

  CONFIGURE_FILE(src/YAJL.dxy
                 ${CMAKE_BINARY_DIR}/YAJL.dxy @ONLY)

  FILE(MAKE_DIRECTORY "${docPath}/doc/libyajl-doc")

  ADD_CUSTOM_TARGET(doc ALL
                    ${doxygenPath} ${CMAKE_BINARY_DIR}/YAJL.dxy
                    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

  INSTALL(DIRECTORY ${CMAKE_BINARY_DIR}/share/ DESTINATION share)
ELSE (doxygenPath)
  MESSAGE("-- Doxygen not found, not generating documentation")
  ADD_CUSTOM_TARGET(
    doc
    echo doxygen not installed, not generating documentation
  )
ENDIF (doxygenPath)
