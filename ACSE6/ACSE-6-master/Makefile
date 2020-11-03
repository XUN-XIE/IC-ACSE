SHELL = /bin/sh

CXX     = mpicxx

CXXFLAGS = -Wno-deprecated -std=c++14

MAKE    = make

# executable file name
TARGET = my_code

# object for input file name (i.e. input with .o swapped for .cpp)
OBJS = Example_1.o

@echo Linking
@$(CXX) -O3 $(LFLAGS) -o $(TARGET) $(OBJS)

@echo Compiling
@$(CXX) $(CXXFLAGS_DEBUG) -c $< -o $@