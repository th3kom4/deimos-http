CXX = g++

CXXFLAGS = -Wall -Wextra -Wextra -std=c++23 -Iinclude

TARGET = server
OBJS = build/main.o build/TcpSocket.o build/ClientConnection.o build/HttpRequest.o

all: build_dir $(TARGET)

build_dir:
	mkdir -p build

$(TARGET): $(OBJS) 
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

build/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf build server
