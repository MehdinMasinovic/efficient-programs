# https://raw.githubusercontent.com/TheNetAdmin/Makefile-Templates/refs/heads/master/SmallProject/Template/Makefile

# tool macros
CXX := g++
CXXFLAGS :=
DBGFLAGS := -g
COBJFLAGS := $(CXXFLAGS) -c

# path macros
BIN_PATH := bin
OBJ_PATH := obj
SRC_PATH := src
DBG_PATH := debug

# compile macros
TARGET_NAME := baseline

ifeq ($(OS),Windows_NT)
	TARGET_NAME := $(addsuffix .exe,$(TARGET_NAME))
endif

TARGET := $(BIN_PATH)/$(TARGET_NAME)
TARGET_DEBUG := $(DBG_PATH)/$(TARGET_NAME)

# src files & obj files
SRC := $(SRC_PATH)/$(TARGET_NAME).cpp
OBJ := $(OBJ_PATH)/$(TARGET_NAME).o
OBJ_DEBUG := $(DBG_PATH)/$(TARGET_NAME).o

# clean files list
DISTCLEAN_LIST := $(OBJ) \
                  $(OBJ_DEBUG)
CLEAN_LIST := $(TARGET) \
			  $(TARGET_DEBUG) \
			  $(DISTCLEAN_LIST)

# data
DATASET_SMALL = data/a.csv data/b.csv data/c.csv data/d.csv
DATASET_SMALL_RESULT = data/abcd.csv
DATASET = /localtmp/efficient24/f1.csv /localtmp/efficient24/f2.csv /localtmp/efficient24/f3.csv /localtmp/efficient24/f4.csv
DATASET_RESULT = /localtmp/efficient24/output.csv

# default rule
default: makedir all

# non-phony targets
$(TARGET): $(OBJ)
	$(CXX) -o $@ $(OBJ) $(CXXFLAGS)

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c*
	$(CXX) $(COBJFLAGS) -o $@ $<

$(DBG_PATH)/%.o: $(SRC_PATH)/%.c*
	$(CXX) $(COBJFLAGS) $(DBGFLAGS) -o $@ $<

$(TARGET_DEBUG): $(OBJ_DEBUG)
	$(CXX) $(CXXFLAGS) $(DBGFLAGS) $(OBJ_DEBUG) -o $@

# phony rules
.PHONY: makedir
makedir:
	@mkdir -p $(BIN_PATH) $(OBJ_PATH) $(DBG_PATH)

.PHONY: all
all: $(TARGET)

.PHONY: debug
debug: $(TARGET_DEBUG)

.PHONY: verify_small
verify_small: $(TARGET)
	$(TARGET) $(DATASET_SMALL) | sort | diff - $(DATASET_SMALL_RESULT)

.PHONY: verify
verify: $(TARGET)
	$(TARGET) $(DATASET) | sort | diff - $(DATASET_RESULT)

.PHONY: run
run: $(TARGET)
	$(TARGET) $(DATASET)

.PHONY: analyze
analyze: $(TARGET)
	run_perf_analysis.sh $(TARGET_NAME) "$(TARGET) $(DATASET)" && \
	mv $(TARGET_NAME)_performance.csv results/

.PHONY: clean
clean:
	@echo CLEAN $(CLEAN_LIST)
	@rm -f $(CLEAN_LIST)

.PHONY: distclean
distclean:
	@echo CLEAN $(DISTCLEAN_LIST)
	@rm -f $(DISTCLEAN_LIST)
