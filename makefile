CXX = g++
CXXFLAGS = -std=c++17 

SRC = main.cpp client.cpp server.cpp
TARGET = iftp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)

