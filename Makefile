CXX = g++
#flagi 
CXXFLAGS = -Wall -Wextra -std=c++17

TARGET = klient

SOURCE = klient.c 

OBJS = $(SOURCE:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(OBJS) $(TARGET)
