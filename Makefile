CXX = c++
CXXFLAGS = -Wall -Wextra

SOURCES = main.cpp math.cpp raytracing.cpp new.cpp window.cpp
HEADERS = header.hpp

OBJDIR = objects
OBJECTS = $(SOURCES:%.cpp=$(OBJDIR)/%.o)

EXEC = exec

all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -lSDL2 -o $(EXEC)

$(OBJDIR)/%.o: %.cpp $(HEADERS) | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR) $(EXEC)

re: clean all

.PHONY: all clean re
