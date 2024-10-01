#include <stdio.h>
#include <string.h>
#include <esp_log.h>
#include <esp_system.h>
#include <esp_console.h>
#include <esp_vfs.h>
#include <esp_vfs_fat.h>
#include <esp_spiffs.h>
#include <esp_http_server.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


// Define your filesystem type and base path
#define FILESYSTEM_TYPE "spiffs"
#define BASE_PATH "/spiffs"



static const char *TAG = "FileSystem";
// Initialize filesystem
esp_err_t init_filesystem() {
    esp_vfs_spiffs_conf_t conf = {
        .base_path = BASE_PATH,
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true
    };
    
    esp_err_t result = esp_vfs_spiffs_register(&conf);
    
    if (result != ESP_OK) {
        if (result == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (result == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(result));
        }
        return ESP_FAIL;
    }
    printf("debug init_filesystem 2\n");
    size_t total = 0, used = 0;
    result = esp_spiffs_info(NULL, &total, &used);
    if (result != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(result));
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }

    FILE* f = fopen("/spiffs/assets/index-D0Ffin-u.js", "r");
    if(f == NULL) {
        ESP_LOGE(TAG, "failed to open file for reading");
        return ESP_FAIL;
    }
    printf("debug init_filesystem 3\n");
    char line[64];
    fgets(line, sizeof(line), f);
    fclose(f);
    printf("%s\n", line);
    return ESP_OK;
}

size_t my_strlen(const char *str) {
    size_t length = 0;  // Initialize length counter
    while (str[length] != '\0') {  // Loop until null terminator is found
        length++;  // Increment length for each character
    }
    return length;  // Return the final length
}
// Serve static files

#define IS_FILE_EXT(filename, ext) \
    (strcasecmp(&filename[strlen(filename) - sizeof(ext) + 1], ext) == 0)

/* Set HTTP response content type according to file extension */
esp_err_t get_content_type_from_file(httpd_req_t *req, const char *filename) {
    if (IS_FILE_EXT(filename, ".pdf")) {
        return httpd_resp_set_type(req, "application/pdf");
    } else if (IS_FILE_EXT(filename, ".html") || IS_FILE_EXT(filename, ".html.gz")) {
        return httpd_resp_set_type(req, "text/html");
    } else if (IS_FILE_EXT(filename, ".jpeg")) {
        return httpd_resp_set_type(req, "image/jpeg");
    } else if (IS_FILE_EXT(filename, ".ico")) {
        return httpd_resp_set_type(req, "image/x-icon");
    } else if (IS_FILE_EXT(filename, ".js") || IS_FILE_EXT(filename, ".js.gz")){
        return httpd_resp_set_type(req, "application/javascript");
    } else if (IS_FILE_EXT(filename, ".css") || IS_FILE_EXT(filename, ".css.gz")){
        return httpd_resp_set_type(req, "text/css");
    } else if (IS_FILE_EXT(filename, ".png")){
        return httpd_resp_set_type(req, "image/png");
    } else if (IS_FILE_EXT(filename, ".svg")) {
        return httpd_resp_set_type(req, "image/svg+xml");
    } else if (IS_FILE_EXT(filename, ".mp4")) {
        return httpd_resp_set_type(req, "video/mp4");
    }
    /* This is a limited set of types */
    /* For any other type always set as plain text */
    return httpd_resp_set_type(req, "text/plain");
}

static esp_err_t file_get_handler(httpd_req_t *req) {
    printf("debug file_get_handler 1\n");
    // char filepath[1000];
    char *filepath = (char *)malloc(my_strlen(BASE_PATH) + my_strlen(req->uri) + 1);
    if (filepath == NULL) {
        ESP_LOGE("HTTP", "Failed to allocate memory for filepath");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    printf("BASE_PATH: %s length: %d\n", BASE_PATH, my_strlen(BASE_PATH));
    printf("req->uri: %s length: %d\n", req->uri, my_strlen(req->uri));
    // Check if the requested URI is "/"
    if (req->uri[0]== '/' && my_strlen(req->uri)==1) {
        // Serve index.html for the root path
        snprintf(filepath, my_strlen(BASE_PATH) + 12, "%s/index.html", BASE_PATH);
    } else {
        // Serve the requested file
        snprintf(filepath, my_strlen(BASE_PATH) + my_strlen(req->uri) + 1, "%s%s", BASE_PATH, req->uri);
    }
    //////////////// WORK ON THIS 
    // if (get_content_type_from_file(req,filepath) == ESP_OK) {
    //     ESP_LOGE("HTTP", "get_content_type_from_file error");
    //     httpd_resp_send_500(req);
    //     return ESP_FAIL;
    // }
    if (IS_FILE_EXT(filepath, ".pdf")) {
        httpd_resp_set_type(req, "application/pdf");
    } else if (IS_FILE_EXT(filepath, ".html") || IS_FILE_EXT(filepath, ".html.gz")) {
        httpd_resp_set_type(req, "text/html");////////////////somethin aint right
    } else if (IS_FILE_EXT(filepath, ".jpeg")) {
        httpd_resp_set_type(req, "image/jpeg");
    } else if (IS_FILE_EXT(filepath, ".ico")) {
        httpd_resp_set_type(req, "image/x-icon");
    } else if (IS_FILE_EXT(filepath, ".js") || IS_FILE_EXT(filepath, ".js.gz")){
        httpd_resp_set_type(req, "application/javascript");
    } else if (IS_FILE_EXT(filepath, ".css") || IS_FILE_EXT(filepath, ".css.gz")){
        httpd_resp_set_type(req, "text/css");
    } else if (IS_FILE_EXT(filepath, ".png")){
        httpd_resp_set_type(req, "image/png");
    } else if (IS_FILE_EXT(filepath, ".svg")) {
        httpd_resp_set_type(req, "image/svg+xml");
    } else if (IS_FILE_EXT(filepath, ".mp4")) {
        httpd_resp_set_type(req, "video/mp4");
    }
    /* This is a limited set of types */
    /* For any other type always set as plain text */
    httpd_resp_set_type(req, "text/plain");
    ////////////////



    // httpd_resp_set_type
    // httpd_uri_match_wildcard

    printf("debug file_get_handler 2\n");
    printf("filepath: %s\n", filepath);
    FILE *file = fopen(filepath, "r");
    if (file == NULL) {
        ESP_LOGE("HTTP", "File not found: %s", filepath);
        httpd_resp_send_404(req);
        return ESP_OK;
    }
    printf("debug file_get_handler 3\n");
    char *buf = (char *)malloc(512);
    if (buf == NULL) {
        ESP_LOGE("HTTP", "Failed to allocate memory for buffer");
        fclose(file);
        return ESP_FAIL;
    }
    size_t read_len;
    while ((read_len = fread(buf, sizeof(char), 511, file)) > 0) {
        if (httpd_resp_send_chunk(req, buf, read_len) != ESP_OK) {
            fclose(file);
            return ESP_FAIL;
        }
        vTaskDelay(pdMS_TO_TICKS(2));
    }
    free(buf);
    printf("debug file_get_handler 4\n");
    fclose(file);
    free(filepath); 
    vTaskDelay(pdMS_TO_TICKS(20));
    httpd_resp_send_chunk(req, NULL, 0); // End the response
    printf("debug file_get_handler 5\n");
    size_t total = 0, used = 0;
esp_spiffs_info(NULL, &total, &used);
ESP_LOGI(TAG, "SPIFFS size: total: %d, used: %d", total, used);

    return ESP_OK;
}
httpd_uri_t uri_get = {
            .uri       = "/*", // Match all URIs
            .method    = HTTP_GET,
            .handler   = file_get_handler,
            .user_ctx  = NULL
        };

httpd_uri_t uri_assets_get_js = {
    .uri       = "/assets/index-D0Ffin-u.js", // Match all URIs starting with /assets
    .method    = HTTP_GET,
    .handler   = file_get_handler,  // Your file handler
    .user_ctx  = NULL
};

httpd_uri_t uri_assets_get_css = {
    .uri       = "/assets/index-DDqFzPXY.css", // Match all URIs starting with /assets
    .method    = HTTP_GET,
    .handler   = file_get_handler,  // Your file handler
    .user_ctx  = NULL
};

httpd_uri_t vite = {
    .uri       = "/vite.svg", // Match all URIs starting with /assets
    .method    = HTTP_GET,
    .handler   = file_get_handler,  // Your file handler
    .user_ctx  = NULL
};
// Start HTTP server
httpd_handle_t start_webserver(void) {
    printf("debug start web 1\n");
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    config.uri_match_fn = httpd_uri_match_wildcard;
    config.stack_size = 8192;
    if (init_filesystem() != ESP_OK) {
        return NULL;
    }
    printf("debug start web 2\n");
    // Initialize HTTP server
    ESP_LOGI("HTTP", "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        
        
        httpd_register_uri_handler(server, &uri_assets_get_js);
        httpd_register_uri_handler(server, &uri_assets_get_css);
        httpd_register_uri_handler(server, &vite);
        httpd_register_uri_handler(server, &uri_get);
        return server;
    }
    printf("debug start web 3\n");
    ESP_LOGI("HTTP", "Error starting server!");
    return NULL;
}

static void stop_webserver(httpd_handle_t server) {
    httpd_stop(server);
}
