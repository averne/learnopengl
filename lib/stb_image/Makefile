TARGET            =    libstbi
EXTENSION         =    a
OUT               =    lib
BUILD             =    build
SOURCES           =    src
INCLUDES          =    include
LIBS              =

ARCH              =    -march=native
FLAGS             =    -Wall -pipe -O2 -ffunction-sections -fdata-sections
CFLAGS            =    -std=gnu11
CXXFLAGS          =    -std=gnu++17
ASFLAGS           =
ARFLAGS           =    -qc

PREFIX            =
CC                =    $(PREFIX)gcc
CXX               =    $(PREFIX)g++
AS                =    $(PREFIX)as
AR                =    $(PREFIX)ar
RANLIB            =    $(PREFIX)ranlib

# -----------------------------------------------

CFILES            =    $(shell find $(SOURCES) -name *.c)
CPPFILES          =    $(shell find $(SOURCES) -name *.cpp)
SFILES            =    $(shell find $(SOURCES) -name *.s -or -name *.S)
OFILES            =    $(CFILES:%=$(BUILD)/%.o) $(CPPFILES:%=$(BUILD)/%.o) $(SFILES:%=$(BUILD)/%.o)
DFILES            =    $(OFILES:.o=.d)

LIB_TARGET        =    $(if $(OUT:=), $(OUT)/$(TARGET).$(EXTENSION), .$(OUT)/$(TARGET).$(EXTENSION))

INCLUDE_FLAGS     =    $(addprefix -I,$(INCLUDES)) $(foreach dir,$(LIBS),-I$(dir)/include)

# -----------------------------------------------

.SUFFIXES:

.PHONY: all clean

all: $(LIB_TARGET)

$(LIB_TARGET): $(OFILES)
	@echo " AR  " $@
	@mkdir -p $(dir $@)
	@$(AR) $(ARFLAGS) $@ $<
	@$(RANLIB) $@
	@echo "Built" $(notdir $@)

$(BUILD)/%.c.o: %.c
	@echo " CC  " $@
	@mkdir -p $(dir $@)
	@$(CC) -MMD -MP $(ARCH) $(FLAGS) $(CFLAGS) $(DEFINES) $(INCLUDE_FLAGS) -c $< -o $@

$(BUILD)/%.cpp.o: %.cpp
	@echo " CXX " $@
	@mkdir -p $(dir $@)
	@$(CXX) -MMD -MP $(ARCH) $(FLAGS) $(CXXFLAGS) $(DEFINES) $(INCLUDE_FLAGS) -c $< -o $@

$(BUILD)/%.s.o: %.s %.S
	@echo " AS  " $@
	@mkdir -p $(dir $@)
	@$(AS) -MMD -MP -x assembler-with-cpp $(ARCH) $(FLAGS) $(ASFLAGS) $(INCLUDE_FLAGS) -c $< -o $@

clean:
	@echo Cleaning...
	@rm -rf $(BUILD) $(TARGET) $(OUT)

-include $(DFILES)
