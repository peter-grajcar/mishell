CC			:= gcc
SRC_DIR		:= src
INCLUDE_DIR	:= include
BUILD_DIR	:= build
OBJ_DIR		:= $(BUILD_DIR)/obj
GEN_DIR		:= $(BUILD_DIR)/generated

CC_FLAGS	:= -I$(INCLUDE_DIR) -I$(GEN_DIR) $(shell pkg-config --cflags readline) -Wall -Wextra -Werror -Wno-unused-parameter -Wmissing-prototypes
LD			:= gcc
LD_FLAGS	:= $(shell pkg-config --libs readline) $(shell pkg-config --libs flex)
FLEX		:= flex
FLEX_FLAGS	:=
BISON		:= bison
BISON_FLAGS	:= -y

SRCS		:= $(shell find $(SRC_DIR) -name '*.[cly]')
OBJS		:= $(subst $(SRC_DIR),$(OBJ_DIR), $(addsuffix .o, $(subst .c,,$(subst .l,.lex,$(subst .y,.tab,$(SRCS))))))
EXECUTABLE	:= mishell

.PHONY: clean distclean

.PRECIOUS: $(GEN_DIR)/%.lex.c

$(EXECUTABLE): $(OBJS)
	$(LD) -o $@ $< $(LD_FLAGS)

$(BUILD_DIR):
	mkdir -p $@

$(OBJ_DIR): $(BUILD_DIR)
	mkdir -p $@

$(GEN_DIR): $(BUILD_DIR)
	mkdir -p $@

$(GEN_DIR)/%.lex.c: $(SRC_DIR)/%.l $(GEN_DIR)
	$(FLEX) $(FLEX_FLAGS) -o $@ $<

$(GEN_DIR)/%.tab.c: $(SRC_DIR)/%.y $(GEN_DIR)
	$(BISON) $(BISON_FLAGS) -o $@ $<

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(OBJ_DIR)
	$(CC) $(CC_FLAGS) -o $@ -c $<

$(OBJ_DIR)/%.tab.o: $(GEN_DIR)/%.tab.c $(OBJ_DIR) $(GEN_DIR)
	$(CC) $(CC_FLAGS) -o $@ -c $<

$(OBJ_DIR)/%.lex.o: $(GEN_DIR)/%.lex.c $(OBJ_DIR) $(GEN_DIR)
	$(CC) $(CC_FLAGS) -o $@ -c $<

distclean:
	find . -name '*.o' -exec rm -f {} \;
	rm -rf $(GEN_DIR)

clean:
	rm -f $(EXECUTABLE)
	rm -rf $(BUILD_DIR)

