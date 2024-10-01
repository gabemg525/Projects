#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_system.h"
#include "time.h"

#define ADC_CHANNEL ADC_CHANNEL_1
#define MAX_MORSE_MESSAGE 10000000
#define MAX_FINAL_MESSAGE 10000

void morseToText(char* morseCode) {
    const char* morseLetters[] = {".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---", "-.-", ".-..", "--",
                                  "-.", "---", ".--.", "--.-", ".-.", "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--.."};
    const char* morseDigits[] = {"-----", ".----", "..---", "...--", "....-", ".....", "-....", "--...", "---..", "----."};

    char ch;
    char* final_message;
    final_message = (char*)malloc(sizeof(char)*MAX_FINAL_MESSAGE);
    int final_message_index = 0;
    
    printf("morse message: %s\nstrlen(morseCode): %d\n", morseCode, strlen(morseCode));
    char* token;
    
    while ((token = strtok_r(morseCode, "/", &morseCode))) {
        char* charToken;
        char* charCopy = strdup(token);

        // Tokenize each character within the word
        charToken = strtok(charCopy, " ");
        while (charToken != NULL) {
            // Check if token is a letter
            for (int i = 0; i < 26; i++) {
                if (strcmp(charToken, morseLetters[i]) == 0) {
                    printf("%c", 'A' + i);
                    break;
                }
            }
            
            // Check if token is a digit
            for (int i = 0; i < 10; i++) {
                if (strcmp(charToken, morseDigits[i]) == 0) {
                    printf("%c", '0' + i);
                    break;
                }
            }

            // Move to the next character token
            charToken = strtok(NULL, " ");
        }

        free(charCopy);

        // Print a space between words
        printf(" ");
        
        // Move to the next word token

    }
    printf("\n");
    // TOKENIZE ON '/'

    // TOKENIZE THOSE TOKENS ON ' '
    // for(int i=0; i<strlen(morseCode);i++){
    //     ch = morseCode[i];
    //     if(ch=='/'){
    //         final_message[final_message_index]= ' ';
    //         final_message_index++;
    //     }else if(ch==' '){
            
    //     }
    // }
}


void app_main() {
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN_DB_11);
    int val = adc1_get_raw(ADC_CHANNEL);
    int last_val = val;
    int total_up = 0;
    int total_down = 0;
    int time_units = 0;
    char* full_morse_message;
    full_morse_message = (char*)malloc(sizeof(char)*1000);
    int morse_index = 0;
    int first_time_on = 0;
    clock_t start = clock();


 
    // Print the execution time
    
    while(time_units<100 || first_time_on==0){
    // while(1){
        
        val = adc1_get_raw(ADC_CHANNEL);
        if(val<last_val-40){
            // printf("voltage down\n");
            total_down++;
            // printf("total down: %d\n", total_down);
            // printf("time units off: %d\n", time_units);
            if(time_units<8 && time_units>2){
                printf(" ");
                full_morse_message[morse_index]=' ';
                morse_index++;
       
                
            }
            if(time_units>9&&first_time_on!=0){
                printf("\n/\n");
                // full_morse_message[morse_index]='/';
                full_morse_message[morse_index]='/';
                morse_index++;
     
                
            }
            time_units = 0;

        }
        if(val>last_val+40){
            // printf("voltage up\n");
            first_time_on = 1;
            total_up++;
            // printf("total up: %d\n", total_up);
            // printf("time units on: %d\n", time_units);
            if(time_units<2){
                printf(".");
                // full_morse_message[morse_index]='.';
                full_morse_message[morse_index]='.';
                morse_index++;

    
            }
            if(time_units>=2){
                printf("-");
                // full_morse_message[morse_index]='-';
                full_morse_message[morse_index]='-';
                morse_index++;
           
            }
            time_units = 0;
        }
        time_units++;

        // printf("RRaw voltage: %d\n", val);
        vTaskDelay(pdMS_TO_TICKS(10));
        last_val = val;
    }
    printf("\n");
    full_morse_message[morse_index]=NULL;
    printf("Full Morse Message: %s\n", full_morse_message);
    morseToText(full_morse_message);

    // Stop the timer
    clock_t end = clock();

    // Calculate the execution time in seconds
    double executionTime = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Execution Time: %.6f seconds\n", executionTime);
    

    
}
