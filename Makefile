SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin
INCLUDE_DIR := include
SRCS := $(wildcard $(SRC_DIR)/*.cc)
OBJS := $(patsubst $(SRC_DIR)/%.cc, $(OBJ_DIR)/%.o, $(SRCS))
# EXES := $(SRCS: %.cc=%)  # 从.cc生成可执行文件
EXE := main

CC := g++
CFLAGS  := -Wall -g -std=c++11 #-fsanitize=leak
LDFLAGS := -pthread -lmysqlclient
INCLUFLAGS:= $(addprefix -I, $(INCLUDE_DIR))
CPPFLAGS:= #-DEBUG
# 开启内存泄露检查功能：-fsanitize=leak
# 开启地址越界检查功能：-fsanitize=address
# 开启越界详细错误信息：-fno-omit-frame-pointer
# -llog4cpp  
# 取消编译器优化 -fno-elide-constructors
# echo执行查看Makefile的变量: @echo $(INCLUFLAGS)
.PHONY: out clean rebulid init test
out:$(EXE)

$(EXE):$(OBJS)
	$(CC) $^ -o $@ $(CFLAGS) $(INCLUFLAGS) $(LDFLAGS) $(CPPFLAGS) 
#	$(RM) $(OBJ_DIR)/%.o

$(OBJ_DIR)/%.o:$(SRC_DIR)/%.cc
	$(CC) $< -c -o $@ $(CFLAGS) $(INCLUFLAGS) $(CPPFLAGS)

clean:
	$(RM) $(EXE)
	$(RM) $(OBJS)

rebulid: clean out

init:
	mkdir -p $(SRC_DIR) $(OBJ_DIR)

test:
	g++ ./src/main.cc ./src/WebPageQuery.cc -o test $(CFLAGS) $(INCLUFLAGS) $(LDFLAGS) $(CPPFLAGS)


