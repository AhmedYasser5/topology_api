VERSION := 0

SRCDIR := ./src
INCDIR := ./include
OBJDIR := ./build/obj
DEPDIR := ./build/deps
BINDIR := .

TARGET := TopologyAPI.so

LIB := lib$(TARGET)

MY_PATHS := $(BINDIR) $(INCDIR)
MY_FLAGS := -fPIC

###### complier set-up ######
CC = gcc
CFLAGS = $(MY_FLAGS) -Wextra -Wno-unused-result
CXX = g++
CXXFLAGS = $(CFLAGS) -std=c++17
LD = g++
LDFLAGS = -shared -Wl,-soname,$(TARGET).$(VERSION) $(CXXFLAGS)
DEBUGGER = gdb

CFLAGS += -O2 -ftree-vectorize -fomit-frame-pointer -march=native
# Link Time Optimization
CFLAGS += -flto

CFLAGS += -MMD -MP -I$(SRCDIR) $(foreach i,$(MY_PATHS),-I$(i))

SRCS := $(wildcard $(SRCDIR)/*.cpp)
DEPS := $(patsubst $(SRCDIR)/%,$(DEPDIR)/%.d,$(SRCS))
OBJS := $(patsubst $(SRCDIR)/%,$(OBJDIR)/%.o,$(SRCS))

.PHONY: all
all : $(LIB)

.PHONY: init
init :
	-@rm -rf build $(wildcard *.exe)
	@mkdir -p $(SRCDIR) $(INCDIR) $(OBJDIR) $(DEPDIR)
	-@for i in $(wildcard *.cpp) $(wildcard *.c) $(wildcard *.tpp); do mv ./$$i $(SRCDIR)/$$i; done
	-@for i in $(wildcard *.h) $(wildcard *.hpp); do mv ./$$i $(INCDIR)/$$i; done
	-@$(file >$(SRCDIR)/.clang_complete)\
		$(foreach i,$(MY_PATHS),\
			$(file >>$(SRCDIR)/.clang_complete,-I$(i))\
			$(file >>$(SRCDIR)/.clang_complete,-I../$(i)))

$(LIB): $(OBJS)
	@echo LD $(maketype) "$(<D)/*.o" "->" $@ && \
		$(LD) -o $@.$(VERSION) $(OBJS) $(LDFLAGS)
	@/sbin/ldconfig -n .
	@ln -sf $@.$(VERSION) $@

$(OBJDIR)/%.cpp.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR) $(DEPDIR)
	-@echo CXX $(maketype) $< "->" $@ && \
		$(CXX) -c $< -o $@ -MF $(DEPDIR)/$(<F).d $(CXXFLAGS)

.PHONY: clean
clean : 
	-$(RM) $(OBJS) $(DEPS) $(wildcard *$(TARGET)*)

.PHONY: debug
debug : $(TARGET)
	$(DEBUGGER) $(TARGET)

-include $(DEPS)
