CC?=gcc
CP?=cp
SRCDIR:=src/
RODIR:=obj/
DODIR:=debugobj/
OFILES:=$(patsubst %.c,%.o,$(wildcard $(SRCDIR)*.c))
ROFILES:= $(patsubst $(SRCDIR)%,$(RODIR)%,$(OFILES))
DOFILES:= $(patsubst $(SRCDIR)%,$(DODIR)%,$(OFILES))

TARGET := libcontiguousmalloc.so
RTARGET := $(RODIR)$(TARGET)
DTARGET := $(DODIR)$(TARGET)
INC:= -I ../ -I inc/

LDFLAGS:= -fPIC -shared
RLDFLAGS:= $(LDFLAGS) -flto -s
DLDFLAGS:= $(LDFLAGS)

CFLAGS := -MMD -MP -Wall -Werror -Wextra -Wfatal-errors -std=gnu11 -fPIC $(INC)
RCFLAGS := $(CFLAGS) -O2 -flto
DCFLAGS := $(CFLAGS) -DDEBUG -g3

-include $(ROFILES:%.o=%.d)
-include $(DOFILES:%.o=%.d)

.DEFAULT_GOAL := all

.PHONY: all release clean debug

all: release debug

release: $(TARGET)

debug: $(DTARGET)

$(RODIR) $(DODIR):
	mkdir -p $@

$(RODIR)%.o: $(SRCDIR)%.c
	$(CC) $(RCFLAGS) -c $< -o $@

$(RTARGET): $(ROFILES) | $(RODIR)
	$(CC) -o $@ $^ $(RLDFLAGS)

$(TARGET) : $(RTARGET)
	$(CP) $^ $@

$(DODIR)%.o: $(SRCDIR)%.c
	$(CC) $(DCFLAGS) -c $< -o $@

$(DTARGET) : $(DOFILES) | $(DODIR)
	$(CC) -o $@ $^ $(DLDFLAGS)

clean:
	@rm -rf $(RODIR) $(DODIR)
