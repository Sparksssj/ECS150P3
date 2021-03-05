# Target library
lib	:= libfs.a
objc	:= fs.o disk.o
inter	:= fs.d disk.d
CC	:= gcc
RCS	:= ar rcs
CFLAGS	:= -Wall -Wextra -Werror -MMD

# CFLAGS += -g

ifneq ($(V), 1)
Q = @
endif

all: $(lib)

deps	:= $(patsubset %.o,%.d,$(objc))
-include $(deps)

$(lib):	$(objc)
	@echo "RCS $@"
	$(Q)$(RCS) $@ $^

%.o: %.c
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@echo "clean"
	$(Q)rm -f $(lib) $(objc) $(deps) $(inter)