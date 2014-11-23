#######################################
# Generic Makefile
# for OpenCL Wrapper cppCL
#
# requires specific project
# folder structure:
# - src (source files)
# - include (header files)
# - build (object files)
# - bin (binary files)
#######################################

#######################################
# Compiler & Compiler Flags
#######################################
CC := g++
#CC := clang --analyze
CFLAGS := -g -Wall -Wextra -pedantic -std=c++14 -fdiagnostics-color=auto

#######################################
# Directories
#######################################
SRCDIR := src
BUILDDIR := build
INCLUDEDIR := include
BINDIR := bin
TESTDIR := test

#######################################
# File Extensions
#######################################
EXT_SRC := cpp
EXT_HEADER := hpp
EXT_TEMPLATE := tcc

#######################################
# Targets
#######################################
TARGET_MAIN := $(BINDIR)/main

#######################################
# Includes & Libraries
#######################################
PATH_INC := -I$(INCLUDEDIR) -I/opt/AMDAPP/SDK/include
PATH_LIB := -L/opt/AMDAPP/SDK/lib -lOpenCL

#######################################
# Generic catching of all source
# files in src/ directory and all
# object files in build/ directory.
#######################################
SRC_FILES := $(wildcard $(SRCDIR)/*.$(EXT_SRC))
#OBJ_FILES := $(pathsubst $(SRCDIR)/%.$(EXT_SRC), $(BUILDDIR)/%.o, $(SRC_FILES))
OBJ_FILES := $(addprefix $(BUILDDIR)/, $(notdir $(SRC_FILES:.$(EXT_SRC)=.o)))

#######################################
# linking of all object files
# for TARGET_MAIN.
#######################################
$(TARGET_MAIN): $(OBJ_FILES)
	@echo " Linking..."
	@echo " $(CC) $^ -o $(TARGET_MAIN) $(PATH_LIB)"; $(CC) $^ -o $(TARGET_MAIN) $(PATH_LIB)

#######################################
# Compiling of all source files
# within the given src directory.
#######################################
$(BUILDDIR)/%.o: $(SRCDIR)/%.$(EXT_SRC)
	@mkdir -p $(BUILDDIR)
	@echo " $(CC) $(CFLAGS) $(PATH_INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(PATH_INC) -c $< -o $@

#######################################
# Tests
#######################################
#tester:
#	$(CC) $(CFLAGS) $(TESTDIR)/tester.$(EXT_SRC) $(PATH_INC) $(PATH_LIB) -o $(BINDIR)/tester

#######################################
# Clean
#######################################
clean:
	@echo " Cleaning...";
	@echo " $(RM) -r $(BUILDDIR) $(TARGET_MAIN)"; $(RM) -r $(BUILDDIR) $(TARGET_MAIN)

#######################################
# PHONY declarations
#######################################
.PHONY: clean
