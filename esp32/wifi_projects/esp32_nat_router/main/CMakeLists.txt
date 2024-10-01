idf_component_register(
    SRCS "esp32_nat_router.c"
         "http_server.c"
    INCLUDE_DIRS "."
)



spiffs_create_partition_image(spiffs ../components/spiffs_data FLASH_IN_PROJECT)
