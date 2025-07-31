# Compiler
CXX = g++

# Compiler flags
# -std=c++17 -> Use the C++17 standard
# -Wall      -> Show all warnings
# -g         -> Include debugging information
CXXFLAGS = -std=c++17 -Wall -g

# Header files directory
INCLUDES = -Iinclude

# App name
TARGET = differentiator

# List of all sources 
SRCS = main.cpp src/ast.cpp src/parser.cpp src/differentiator.cpp

# Create object file using sources list
OBJS = $(SRCS:.cpp=.o)


# Default rule: build target
all: $(TARGET)

# Rule to link the target executable
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

# Rule to compile source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Dir cleanup
clean:
	rm -f $(OBJS) $(TARGET)