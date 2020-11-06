CC			:= gcc
SRC_DIR		:= src
INCLUDE_DIR	:= dir
BUILD_DIR	:= build
OBJ_DIR		:= $(BUILD_DIR)/obj
GEN_DIR		:= $(BUILD_DIR)/generated

CC_FLAGS	:= -I$(INCLUDE_DIR) -I$(GEN_DIR) -Wall -Wextra -Werror -Wno-unused-parameter -Wmissing-prototypes
LD			:= gcc
LD_FLAGS	:=
FLEX		:= flex
FLEX_FLAGS	:=
BISON		:= bison
BISON_FLAGS	:= -y

SRCS		:= $(shell find $(SRC_DIR) -name '*.[cly]')
OBJS		:= $(subst $(SRC_DIR),$(OBJ_DIR), $(addsuffix .o, $(SRCS:.c=)))
EXECUTABLE	:= mishell

.PHONY: clean distclean test

test:
	@echo "sources: " $(SRCS)
	@echo "objects: " $(OBJS)


$(EXECUTABLE): $(OBJS)
	$(LD) $(LD_FLAGS) -o $@ $<

$(BUILD_DIR):
	mkdir -p $@

$(OBJ_DIR): $(BUILD_DIR)
	mkdir -p $@

$(GEN_DIR): $(BUILD_DIR)
	mkdir -p $@

$(GEN_DIR)/%.l.c: $(SRC_DIR)/%.l $(GEN_DIR)
	$(FLEX) $(FLEX_FLAGS) -o $@ $<

$(GEN_DIR)/%.y.c: $(SRC_DIR)/%.y $(GEN_DIR)
	$(BISON) $(BISON_FLAGS) -o $@ $<

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(OBJ_DIR)
	$(CC) $(CC_FLAGS) -o $@ $<

$(OBJ_DIR)/%.y.o: $(GEN_DIR)/%.y.c $(OBJ_DIR) $(GEN_DIR)
	$(CC) $(CC_FLAGS) -o $@ $<

$(OBJ_DIR)/%.l.o: $(GEN_DIR)/%.l.c $(OBJ_DIR) $(GEN_DIR)
	$(CC) $(CC_FLAGS) -o $@ $<

distclean:
	find . -name '*.o' -exec rm -f {} \;
	rm -rf $(GEN_DIR)

clean:
	rm -f $(EXECUTABLE)
	rm -rf $(BUILD_DIR)

