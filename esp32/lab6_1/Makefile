CC = gcc
CFLAGS = -Wall -Wextra
TARGET = led_program
SRCS = led_program.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^ 

clean:
	rm -f $(TARGET)