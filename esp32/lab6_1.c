#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <wiringPi.h>

#define LED_PIN 26 // GPIO pin number to which the LED is connected
#define DOT_DELAY 10 // Delay duration for a dot (ms)
#define CHAR_DELAY (DOT_DELAY * 3) // Delay between characters (ms)
#define WORD_DELAY (DOT_DELAY * 7) // Delay between words (ms)
#define MAX_MORSE_MESSAGE 1000000

const char* morseCode[] = {
    ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---", "-.-", ".-..",
    "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-", "..-", "...-", ".--", "-..-",
    "-.--", "--..", "-----", ".----", "..---", "...--", "....-", ".....", "-....",
    "--...", "---..", "----.", "/" // Morse code for space (word separator)
};

void dot(void) {
    digitalWrite(LED_PIN, HIGH);
    delay(DOT_DELAY);
    digitalWrite(LED_PIN, LOW); // Turn off the LED
    delay(DOT_DELAY);
}

void dash(void) {
    digitalWrite(LED_PIN, HIGH);
    delay(DOT_DELAY * 3);
    digitalWrite(LED_PIN, LOW); // Turn off the LED
    delay(DOT_DELAY);
}

void morseChar(char ch, char* morseMessage) {
    if (isalpha(ch)) {
        int index = toupper(ch) - 'A';
        const char* morse = morseCode[index];
        strcat(morseMessage, morse);
        strcat(morseMessage, " "); // Space between characters
    } else if (isdigit(ch)) {
        int index = ch - '0' + 26;
        const char* morse = morseCode[index];
        strcat(morseMessage, morse);
        strcat(morseMessage, " "); // Space between characters
    } else if (ch == ' ') {
        strcat(morseMessage, "/"); // Space between words
    } else {
        // Skip unrecognized characters
    }
}

void morseString(const char* str, char* morseMessage) {
    int len = strlen(str);
    for (int i = 0; i < len; i++) {
        morseChar(str[i], morseMessage);
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Usage: %s <message>\n", argv[0]);
        return 1;
    }

    if (wiringPiSetupGpio() == -1) {
        printf("Failed to initialize WiringPi\n");
        return 1;
    }

    pinMode(LED_PIN, OUTPUT); // Set the GPIO pin as output
    int repeats = atoi(argv[1]);
    const char* message = argv[2];
    char morseMessage[MAX_MORSE_MESSAGE] = ""; // Array to hold the Morse code message
    morseString(message, morseMessage);

    int len = strlen(morseMessage);
    for(int j = 0; j<repeats;j++){
        for (int i = 0; i < len; i++) {
            if (morseMessage[i] == '.')
                dot();
            else if (morseMessage[i] == '-')
                dash();
            else if (morseMessage[i] == ' ')
                delay(CHAR_DELAY); // Space between characters
            else if (morseMessage[i] == '/')
                delay(WORD_DELAY); // Space between words
        }
    }
    digitalWrite(LED_PIN, LOW); // Turn off the LED after displaying the Morse code

    return 0;
}
