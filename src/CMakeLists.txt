include(GNUInstallDirs)

# configuration

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR})

# dependencies


# jartsy core files

set(jartsy_core
    jartsy.h
	util.cpp
	jartsyla.h
    film.h
	camera.h
	ray.h
    material.h
	entities.h
	entities.cpp
	lights.h
    scene.h
    framebuffer/framebuffer.h
    framebuffer/framebuffer.cpp)

# jartsy output formats

set(jartsy_export
    exportfile/exportpng.h
    exportfile/exportpng.cpp
    exportfile/exportpgm.h
    exportfile/exportpgm.cpp)

# jartsy input formats

set(jartsy_import
    nlohman/json.hpp)

# jartsy platform specific files

set(jartsy_platform
    platform/platform.h
    platform/platform.cpp)


# jartsy command-line executable

if(ENABLE_CLI)
    add_executable(jartsy
        ${jartsy_core}
        ${jartsy_export}
        ${jartsy_import}
        ${jartsy_platform}
        platform/entrycli.cpp)

if(ENABLE_OPENMP)
    target_link_libraries(jartsy PRIVATE jartsy_openmp)
endif()

    if(MSVC)
        set_target_properties(jartsy PROPERTIES
            LINK_FLAGS "/INCREMENTAL:NO /OPT:REF")
    endif()

	target_include_directories(jartsy
		PUBLIC ${CMAKE_SOURCE_DIR}/include)
endif()
