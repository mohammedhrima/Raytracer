CXX = c++
CXXFLAGS = -Wall -Wextra

SOURCES = main.cpp math.cpp raytracing.cpp
HEADERS = header.hpp

OBJDIR = objects
OBJECTS = $(SOURCES:%.cpp=$(OBJDIR)/%.o)

EXEC = exec

all: $(EXEC)

$(EXEC): $(OBJECTS)
	mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -lSDL2 -o $(EXEC)

$(OBJDIR)/%.o: %.cpp $(HEADERS)
	mkdir -p $(OBJDIR)  
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(EXEC)

.PHONY: all clean
