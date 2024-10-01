idf_component_register(SRCS "lab4_2.cpp"
                            "DFRobot_LCD.cpp"
                            "dfrobot_c_funcs.c"
                    INCLUDE_DIRS ".")
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-unused-const-variable)