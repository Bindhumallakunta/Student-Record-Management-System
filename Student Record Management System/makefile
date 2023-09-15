CC = g++
CFLAGS = -std=c++11 -Wall

SRCS = ex.cpp logger.cpp
OBJS = $(SRCS:.cpp=.o)
DEPS = logger.h

TARGET = student

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) -lpqxx -lpq -pthread

%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)