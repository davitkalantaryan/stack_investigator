
targetName=unittest

mkfile_path		=  $(abspath $(lastword $(MAKEFILE_LIST)))
mkfile_dir		=  $(shell dirname $(mkfile_path))
# line below is not needed, just here as a example
repoRootPath	:= $(shell curDir=`pwd` && cd $(mkfile_dir)/../../.. && pwd && cd ${curDir})

firstTarget: all

include $(mkfile_dir)/../../common/common_mkfl/unix.common.Makefile

GTEST_SRC_DIR=$(repoRootPath)/src/tests/googletest
COMMON_SRC_DIR=$(repoRootPath)/src/core

GTEST_SRCS	= $(shell find $(GTEST_SRC_DIR) -name "*.cpp")
COMMON_SRCS	= $(shell find $(COMMON_SRC_DIR) -name "*.cpp")

CPPFLAGS += -I$(repoRootPath)/contrib/googletest/googletest/include
# c=+ 11 is needed for google test
CPPFLAGS += -std=c++11

LIBS += $(repoRootPath)/sys/$(lsbCode)/$(Configuration)/lib/libgtest_main$(nameExtension).a
LIBS += $(repoRootPath)/sys/$(lsbCode)/$(Configuration)/lib/libgtest$(nameExtension).a
LIBS += -pthread

all: $(repoRootPath)/sys/$(lsbCode)/$(Configuration)/test/$(targetName)

$(repoRootPath)/sys/$(lsbCode)/$(Configuration)/test/$(targetName): \
			$(GTEST_SRCS:%=$(repoRootPath)/sys/$(lsbCode)/$(Configuration)/.objects/$(targetName)/%.o)	\
			$(COMMON_SRCS:%=$(repoRootPath)/sys/$(lsbCode)/$(Configuration)/.objects/$(targetName)/%.o)
	@mkdir -p $(@D)
	@$(LINK) $^ $(LIBS) $(LFLAGS) -o $@

.PHONY: clean
clean:
	@rm -rf $(repoRootPath)/sys/$(lsbCode)/$(Configuration)/.objects/$(targetName)
	@rm -f  $(repoRootPath)/sys/$(lsbCode)/$(Configuration)/test/$(targetName)
	@echo "  " cleaning of googletest complete !!!
