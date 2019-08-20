TARGET            =    $(notdir $(CURDIR))
EXTENSION         =    elf
OUT               =    out
RELEASE           =    release
DEBUG             =    debug
SOURCES           =    src
INCLUDES          =    ../common
LIBS              =    ../lib/glad ../lib/stb_image

ARCH              =    -march=native -fpie
FLAGS             =    -Wall -pipe
CFLAGS            =    -std=gnu11
CXXFLAGS          =    -std=gnu++17
ASFLAGS           =
LDFLAGS           =    -Wl,-pie
LINKS             =    -lglfw -lGL -lglad -ldl -lstbi

RELEASE_FLAGS     =    $(FLAGS) -O2 -DNDEBUG=1 -ffunction-sections -fdata-sections -flto
RELEASE_CFLAGS    =    $(CFLAGS)
RELEASE_CXXFLAGS  =    $(CXXFLAGS)
RELEASE_ASFLAGS   =    $(ASFLAGS)
RELEASE_LDFLAGS   =    $(LDFLAGS) -Wl,--gc-sections -flto -s

DEBUG_FLAGS       =    $(FLAGS) -g -Og -DDEBUG
DEBUG_CFLAGS      =    $(CFLAGS)
DEBUG_CXXFLAGS    =    $(CXXFLAGS)
DEBUG_ASFLAGS     =    $(ASFLAGS) -g
DEBUG_LDFLAGS     =    $(LDFLAGS) -g -Wl,-Map,$(DEBUG)/$(TARGET).map

PREFIX            =
CC                =    $(PREFIX)gcc
CXX               =    $(PREFIX)g++
AS                =    $(PREFIX)as
LD                =    $(PREFIX)g++

# -----------------------------------------------

CFILES            =    $(shell find $(SOURCES) -name *.c)
CPPFILES          =    $(shell find $(SOURCES) -name *.cpp)
SFILES            =    $(shell find $(SOURCES) -name *.s -or -name *.S)
OFILES            =    $(CFILES:%=$(BUILD)/%.o) $(CPPFILES:%=$(BUILD)/%.o) $(SFILES:%=$(BUILD)/%.o)
DFILES            =    $(OFILES:.o=.d)

RELEASE_TARGET    =    $(if $(OUT:=), $(OUT)/$(TARGET).$(EXTENSION), .$(OUT)/$(TARGET).$(EXTENSION))
DEBUG_TARGET      =    $(if $(OUT:=), $(OUT)/$(TARGET)-debug.$(EXTENSION), .$(OUT)/$(TARGET)-debug.$(EXTENSION))

INCLUDE_FLAGS     =    $(addprefix -I,$(INCLUDES)) $(foreach dir,$(LIBS),-I$(dir)/include)
LIB_FLAGS         =    $(foreach dir,$(LIBS),-L$(dir)/lib)

# -----------------------------------------------

.SUFFIXES:

.PHONY: all libs release debug run clean mrproper

all: release debug

libs:
	@for dir in $(LIBS); do $(MAKE) --no-print-directory -C $$dir; done

release: libs $(RELEASE_TARGET)

debug: libs $(DEBUG_TARGET)

run: debug
	@echo "Running" $(DEBUG_TARGET)
	@$(DEBUG_TARGET)

$(RELEASE_TARGET): $(addprefix $(RELEASE),$(OFILES))
	@echo " LD  " $@
	@mkdir -p $(dir $@)
	@$(LD) $(ARCH) $(RELEASE_LDFLAGS) $(LIB_FLAGS) $^ -o $@ $(LINKS)
	@echo "Built" $(notdir $@)

$(DEBUG_TARGET): $(addprefix $(DEBUG),$(OFILES))
	@echo " LD  " $@
	@mkdir -p $(dir $@)
	@$(LD) $(ARCH) $(DEBUG_LDFLAGS) $(LIB_FLAGS) $^ -o $@ $(LINKS)
	@echo "Built" $(notdir $@)

$(RELEASE)/%.c.o: %.c
	@echo " CC  " $@
	@mkdir -p $(dir $@)
	@$(CC) -MMD -MP $(ARCH) $(RELEASE_FLAGS) $(RELEASE_CFLAGS) $(DEFINES) $(INCLUDE_FLAGS) -c $< -o $@

$(DEBUG)/%.c.o: %.c
	@echo " CC  " $@
	@mkdir -p $(dir $@)
	@$(CC) -MMD -MP $(ARCH) $(DEBUG_FLAGS) $(DEBUG_CFLAGS) $(DEFINES) $(INCLUDE_FLAGS) -c $< -o $@

$(RELEASE)/%.cpp.o: %.cpp
	@echo " CXX " $@
	@mkdir -p $(dir $@)
	@$(CXX) -MMD -MP $(ARCH) $(RELEASE_FLAGS) $(RELEASE_CXXFLAGS) $(DEFINES) $(INCLUDE_FLAGS) -c $< -o $@

$(DEBUG)/%.cpp.o: %.cpp
	@echo " CXX " $@
	@mkdir -p $(dir $@)
	@$(CXX) -MMD -MP $(ARCH) $(DEBUG_FLAGS) $(DEBUG_CXXFLAGS) $(DEFINES) $(INCLUDE_FLAGS) -c $< -o $@

$(RELEASE)/%.s.o: %.s %.S
	@echo " AS  " $@
	@mkdir -p $(dir $@)
	@$(AS) -MMD -MP -x assembler-with-cpp $(ARCH) $(RELEASE_FLAGS) $(RELEASE_ASFLAGS) $(INCLUDE_FLAGS) -c $< -o $@

$(DEBUG)/%.s.o: %.s %.S
	@echo " AS  " $@
	@mkdir -p $(dir $@)
	@$(AS) -MMD -MP -x assembler-with-cpp $(ARCH) $(DEBUG_FLAGS) $(DEBUG_ASFLAGS) $(INCLUDE_FLAGS) -c $< -o $@

clean:
	@echo Cleaning...
	@rm -rf $(DEBUG) $(RELEASE) $(RELEASE_TARGET) $(DEBUG_TARGET) $(OUT)

mrproper: clean
	@for dir in $(LIBS); do $(MAKE) clean --no-print-directory -C $$dir; done

-include $(addprefix $(RELEASE),$(DFILES)) $(addprefix $(DEBUG),$(DFILES))
