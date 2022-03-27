include(GNUInstallDirs)

# configuration

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR})

# dependencies


# jartsy core files

set(jartsy_core
    jartsy.h
	util.cpp
	jartsyla.h
	camera.h
	ray.h
	entities.h
	entities.cpp
    framebuffer/framebuffer.h
    framebuffer/framebuffer.cpp)

# jartsy output formats

set(jartsy_export
    exportfile/exportpgm.h
    exportfile/exportpgm.cpp)

# jartsy platform specific files

set(jartsy_platform
    platform/platform.h
    platform/platform.cpp)


# jartsy command-line executable

if(ENABLE_CLI)
    add_executable(jartsy
        ${jartsy_core}
        ${jartsy_export}
        ${jartsy_platform}
        platform/entrycli.cpp)

    if(MSVC)
        set_target_properties(jartsy PROPERTIES
            LINK_FLAGS "/INCREMENTAL:NO /OPT:REF")
    endif()

	target_include_directories(jartsy
		PUBLIC ${CMAKE_SOURCE_DIR}/include)
endif()
