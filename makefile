CXX = g++
CXXFLAGS = -O2
TARGET = kosaraju.bin
SRC = kosaraju.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)
