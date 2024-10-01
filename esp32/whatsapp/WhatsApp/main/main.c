#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "sdkconfig.h"
#include "esp_http_client.h"
#include "connect.h"
#include "esp_log.h"
#include "esp_check.h"

#define CONFIG_STA_SSID "ringringoda2G_2GEXT"
#define CONFIG_STA_PASSWORD "ZMZCLGTMGHF286LC"
#define CONFIG_ESP_AP_SSID "esp32s3"
#define CONFIG_ESP_AP_PASSWORD "abcd1234"
#define PHONE_NUMBER "15624127275"
#define TEXT_MESSAGE "Hello20Embedded40320Gabriel20G"
#define API_KEY "3459277"













const static char *TAG = "WHATSAPP";
const static char *CONFIG_WIFI_SSID = CONFIG_STA_SSID;
const static char *CONFIG_WIFI_PASSWORD = CONFIG_STA_PASSWORD;

void send_whatsapp_message(void);
void wifi_init(void);
esp_err_t wifi_connect_sta(const char *ssid, const char *pass, int timeout);

void app_main(void){
    nvs_flash_init();
    wifi_init();
  // specify SSID and Password, 10 sec timeout
    ESP_ERROR_CHECK( wifi_connect_sta( CONFIG_WIFI_SSID, CONFIG_WIFI_PASSWORD, 10000 ));

    send_whatsapp_message();
}


void send_whatsapp_message( void ){
    char callmebot_url[] = 
        "https://api.callmebot.com/whatsapp.php?phone=%s&text=%s&apikey=%s";
    char URL[strlen(callmebot_url)];

    //sprintf( URL, callmebot_url, "1234567890", 
    //         url_encode("this is a test"), "xxx" );
    printf("debug 1\n");
    sprintf( URL, callmebot_url, PHONE_NUMBER, 
             TEXT_MESSAGE, API_KEY );
    printf("URL: %s\n", URL);
    printf("debug 2\n");
    esp_http_client_config_t config = {
        .url = URL,
        .method = HTTP_METHOD_GET,

        .cert_pem = NULL, // If you want to disable verification
        .skip_cert_common_name_check = true // To skip common name verification
    };
    printf("debug 3\n");

    esp_http_client_handle_t client = esp_http_client_init( &config );
    printf("debug 4\n");
    esp_err_t err = esp_http_client_perform( client );
    printf("debug 5\n");
    
}