NAME     = exec
CXX      = c++
CXXFLAGS = -std=c++98 -O2 -Wall -Wextra -Werror -pthread -Isrc $(shell sdl2-config --cflags)
LDFLAGS  = -pthread
LDLIBS   = $(shell sdl2-config --libs)

SRC_DIR  = src
OBJ_DIR  = objects
SOURCES  = $(shell find $(SRC_DIR) -name '*.cpp')
OBJECTS  = $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
DEPS     = $(OBJECTS:.o=.d)

all: $(NAME)

$(NAME): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) $(LDLIBS) -o $(NAME)

# -MMD -MP writes .d files so editing a header rebuilds what includes it.
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(NAME)

re: clean all

-include $(DEPS)

.PHONY: all clean re
