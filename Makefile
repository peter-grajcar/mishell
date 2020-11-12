CC			:= gcc
SRC_DIR		:= src
TESTS_DIR	:= tests
INCLUDE_DIR	:= include
BUILD_DIR	:= build
OBJ_DIR		:= $(BUILD_DIR)/obj
GEN_DIR		:= $(BUILD_DIR)/generated

CC_FLAGS	:= -I$(INCLUDE_DIR) -I$(GEN_DIR) -g -Wall -Wextra -Werror -Wno-unused-parameter -Wmissing-prototypes
LD			:= gcc
LD_FLAGS	:= -lreadline
FLEX		:= flex
FLEX_FLAGS	:=
BISON		:= bison
BISON_FLAGS	:= -d --report=all --report-file=$(GEN_DIR)/bison.out

SRCS		:= $(shell find $(SRC_DIR) -name '*.[cyl]')
OBJS		:= $(subst $(SRC_DIR),$(OBJ_DIR), $(addsuffix .o, $(subst .c,,$(subst .l,.lex,$(subst .y,.tab,$(SRCS))))))
EXECUTABLE	:= mishell

.PHONY: clean distclean stylecheck test

.PRECIOUS: $(GEN_DIR)/%.lex.c

$(EXECUTABLE): $(OBJS)
	$(LD) -o $@ $^ $(LD_FLAGS)

$(BUILD_DIR):
	mkdir -p $@

$(OBJ_DIR): $(BUILD_DIR)
	mkdir -p $@

$(GEN_DIR): $(BUILD_DIR)
	mkdir -p $@

stef:
	git clone https://github.com/devnull-cz/stef
	chmod +x stef/stef.sh

test: stef $(EXECUTABLE)
	cd $(TESTS_DIR); ./run-tests.sh $(cat phase-1.tests)

$(GEN_DIR)/lexer.lex.c: $(SRC_DIR)/lexer.l $(GEN_DIR)
	$(FLEX) $(FLEX_FLAGS) -o $@ $<

$(GEN_DIR)/parser.tab.c: $(SRC_DIR)/parser.y $(GEN_DIR)
	$(BISON) $(BISON_FLAGS) -o $@ $<

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(OBJ_DIR)
	@printf "\033[33m" # print the stylecheck result in yellow
	@./cstyle.pl $<
	@printf "\033[0m"
	$(CC) $(CC_FLAGS) -o $@ -c $<

$(OBJ_DIR)/parser.tab.o: $(GEN_DIR)/parser.tab.c $(OBJ_DIR) $(GEN_DIR)
	$(CC) $(CC_FLAGS) -o $@ -c $<

$(OBJ_DIR)/lexer.lex.o: $(GEN_DIR)/lexer.lex.c $(GEN_DIR)/parser.tab.c $(OBJ_DIR) $(GEN_DIR)
	$(CC) $(CC_FLAGS) -o $@ -c $<
	
distclean:
	find . -name '*.o' -exec rm -f {} \;
	rm -rf $(GEN_DIR)

clean:
	rm -f $(EXECUTABLE)
	rm -rf $(BUILD_DIR)

