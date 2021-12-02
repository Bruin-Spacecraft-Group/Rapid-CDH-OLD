TARGET_EXEC ?= RapidCDH

BUILD_DIR ?= ./obj
SRC_DIRS ?= src

SRCS := $(shell find $(SRC_DIRS) -type f -name *.cpp)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

INC_DIRS := comlib
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

LD := clang++
LDFLAGS := -lmraa -lpthread -lm

CC := clang++
CPPFLAGS ?= $(INC_FLAGS) -std=c++20 -MMD -MP -O0 -g3 -Wall

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(LD) $(OBJS) -o $@ $(LDFLAGS)

# cpp source
$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

.PHONY: clean
clean:
	$(RM) -r $(BUILD_DIR)


-include $(DEPS)

MKDIR_P ?= mkdir -p
