# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++20 -Wall

# Output binary name
TARGET = iftp

# Source files
SRCS = main.cpp client.cpp server.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Build target
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Compile source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(TARGET) $(OBJS)

