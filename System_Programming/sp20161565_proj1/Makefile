CC =gcc
CFLAGS = -Wall
TARGET = 20161565.out
OBJECTS = 20161565.o input_handling.o shell_command.o memory_command.o opcode_command.o

all : $(TARGET)

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

	
clean :
	rm -f *.o
	rm -f $(TARGET)
