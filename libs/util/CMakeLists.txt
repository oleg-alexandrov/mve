file (GLOB HEADERS "*.h")
file (GLOB SOURCES "[^_]*.cc")

set(LIBRARY mveUtil)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/..")

add_library(${LIBRARY} SHARED ${SOURCES})

target_link_libraries(${LIBRARY} ${JPEG_LIBRARIES} ${PNG_LIBRARIES} ${TIFF_LIBRARIES})

target_compile_options(${LIBRARY} PRIVATE -fPIC)

# Set RPATHS 
if(APPLE)
  set_target_properties(mveUtil PROPERTIES
    INSTALL_RPATH "@loader_path;@loader_path/../lib;${MULTIVIEW_DEPS_DIR}/lib")
elseif(UNIX) # Unix which is not Apple
  set_target_properties(mveUtil PROPERTIES
    INSTALL_RPATH "$ORIGIN:$ORIGIN/../lib:${MULTIVIEW_DEPS_DIR}/lib")
endif()

# Install the lib and the tools
install(TARGETS mveUtil LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

