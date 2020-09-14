file(GLOB_RECURSE LVGLPP_HEADER "includes/*.hpp")

idf_component_register(SRCS ${LVGLPP_HEADER}
        INCLUDE_DIRS "includes"
        REQUIRES quickjs-esp32)