TARGET_NAMES = endpoints

BUILD_DIR ?= build

TARGETS = $(TARGET_NAMES:%=$(BUILD_DIR)/%)
TARGETS_SRCS = $(TARGET_NAMES:%=%.cpp)

MONGOOSE_DIR ?= mongoose
MONGOOSE_SRC ?= $(MONGOOSE_DIR)/mongoose.c

SRCS := $(TARGETS_SRCS) $(MONGOOSE_SRC)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(SRCS:=.d)

INC_DIRS := $(MONGOOSE_DIR) ../
INC_FLAGS := $(addprefix -I ,$(INC_DIRS))

LDFLAGS := 
CPPFLAGS ?= $(INC_FLAGS) -MMD 
CFLAGS ?= -Wall -Wno-missing-braces
CC = gcc
CXX = g++

MKDIR_P ?= mkdir -p

all: $(TARGETS)

$(BUILD_DIR)/%: $(BUILD_DIR)/%.cpp.o $(BUILD_DIR)/$(MONGOOSE_SRC:=.o)
	$(CXX) $(LDFLAGS) $^ -o $@ 

$(BUILD_DIR)/%.cpp.o: %.cpp
	@$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CFLAGS) -c $< -o $@	

$(BUILD_DIR)/%.c.o: %.c
	@$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@	

.PRECIOUS: $(OBJS)

.PHONY: clean
clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)
