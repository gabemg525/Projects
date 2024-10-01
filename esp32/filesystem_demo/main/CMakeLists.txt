idf_component_register(SRCS "filesystem_demo.c"
                       INCLUDE_DIRS ".")
spiffs_create_partition_image(storage ../partition FLASH_IN_PROJECT)

# # Add the partition CSV file if it's not the default one
# idf_set_global_property(PARTITION_TABLE_FILENAME "partitions.csv")
