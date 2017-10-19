###############################################################################
#General makefile
# Usage:
#   $ make             Compile and link (or archive)
#   $ make clean     Clean the objectives and target.
###############################################################################


CROSS_COMPILE =
OPTIMIZE := -O2
#OPTIMIZE := -O0
WARNINGS := -Wall -Wno-unused -Wno-format
DEFS     := -DMYDEF=1 -UMYDEF2
EXTRA_CFLAGS := -std=c++11
EXTRA_LDFLAGS = ./lib/libcaffe.so.1.0.0

INC_DIR   = /usr/local/cuda/include /home/da.li/local/protobuf/include ./src/include ./src ./include
#should list every source code directory including subdir. ex:SRC_DIR   = src1 src2/a src2/b
SRC_DIR   =  ./src
OBJ_DIR   =  out/target
BIN_DIR   =  ./lib 
EXTRA_SRC =
EXCLUDE_FILES =

SUFFIX       = c cpp cc cxx
TARGET       := out/libisee_deepmar_attribute_recognition.so

#TARGET_TYPE  := ar
#TARGET_TYPE  := app
TARGET_TYPE  := so


#####################################################################################
#  Do not change any part of them unless you have understood this script very well  #
#  This is a kind remind.                                                           #
#####################################################################################

#FUNC#  Add a new line to the input stream.
define add_newline
$1

endef

#FUNC# set the variable `src-x' according to the input $1
define set_src_x
src-$1 = $(filter-out $4,$(foreach d,$2,$(wildcard $d/*.$1)) $(filter %.$1,$3))

endef

#FUNC# set the variable `obj-x' according to the input $1
define set_obj_x
obj-$1 = $(patsubst %.$1,$3%.o,$(notdir $2))

endef

#VAR# Get the uniform representation of the object directory path name
ifneq ($(OBJ_DIR),)
prefix_objdir  = $(shell echo $(OBJ_DIR)|sed 's:\(\./*\)*::')
prefix_objdir := $(filter-out /,$(prefix_objdir)/)
endif

GCC      := $(CROSS_COMPILE)gcc
G++      := $(CROSS_COMPILE)g++
SRC_DIR := $(sort . $(SRC_DIR))
inc_dir = $(foreach d,$(sort $(INC_DIR) $(SRC_DIR)),-I$d)

#--# Do smart deduction automatically
$(eval $(foreach i,$(SUFFIX),$(call set_src_x,$i,$(SRC_DIR),$(EXTRA_SRC),$(EXCLUDE_FILES))))
$(eval $(foreach i,$(SUFFIX),$(call set_obj_x,$i,$(src-$i),$(prefix_objdir))))
$(eval $(foreach f,$(EXTRA_SRC),$(call add_newline,vpath $(notdir $f) $(dir $f))))
$(eval $(foreach d,$(SRC_DIR),$(foreach i,$(SUFFIX),$(call add_newline,vpath %.$i $d))))

all_objs = $(foreach i,$(SUFFIX),$(obj-$i))
all_srcs = $(foreach i,$(SUFFIX),$(src-$i))

CFLAGS       = $(EXTRA_CFLAGS) $(WARNINGS) $(OPTIMIZE) $(DEFS)
TARGET_TYPE := $(strip $(TARGET_TYPE))

ifeq ($(filter $(TARGET_TYPE),so ar app),)
$(error Unexpected TARGET_TYPE `$(TARGET_TYPE)')
endif

ifeq ($(TARGET_TYPE),so)
 CFLAGS  += -fpic -shared
 LDFLAGS += -shared
endif

PHONY = all .mkdir clean

all: .mkdir $(TARGET)

define cmd_o
$$(obj-$1): $2%.o: %.$1  $(MAKEFILE_LIST)
	$(GCC) $(inc_dir) -Wp,-MT,$$@ -Wp,-MMD,$$@.d $(CFLAGS) -c -o $$@ $$<

endef
$(eval $(foreach i,$(SUFFIX),$(call cmd_o,$i,$(prefix_objdir))))

ifeq ($(TARGET_TYPE),ar)
$(TARGET): AR := $(CROSS_COMPILE)ar
$(TARGET): $(all_objs)
	rm -f $@
	$(AR) rcvs $@ $(all_objs)
else
$(TARGET): LD = $(if $(strip $(src-cpp) $(src-cc) $(src-cxx)),$(G++),$(GCC))
$(TARGET): $(all_objs)
	$(LD) $(LDFLAGS)  $(all_objs) $(EXTRA_LDFLAGS) -o $@  
endif

.mkdir:
	@if [ ! -d $(OBJ_DIR) ]; then mkdir -p $(OBJ_DIR); fi

clean:
	#rm -f $(prefix_objdir)*.o  $(prefix_objdir)*.o.d $(TARGET)
	rm -rf $(OBJ_DIR)  $(TARGET)	

-include $(patsubst %.o,%.o.d,$(all_objs))

.PHONY: $(PHONY)

install:
	cp $(TARGET) $(BIN_DIR)
