CC?=gcc
SRCDIR:=src/
ODIR:=obj/
CFILES:=$(wildcard $(SRCDIR)*.c)
_OFILES:=$(patsubst %.c,%.o,$(CFILES))
OFILES:= $(patsubst src/%,$(ODIR)%,$(_OFILES))

INC:= -I ../
LDFLAGS:=
CFLAGS:=-Wall -Werror -Wextra -std=gnu11 $(INC)
TARGET:=final

.PHONY: all clean debug

all: CFLAGS +=-O3 -march=native -funroll-loops
all: LDFLAGS +=-s
all: $(TARGET)

debug: CFLAGS +=-DDEBUG -g3
debug: LDFLAGS +=
debug: $(TARGET)

$(ODIR):
	@mkdir -p $(ODIR)
	 
$(ODIR)%.o: $(SRCDIR)%.c
	$(CC) $(CFLAGS) -c $< -o $@ 

$(TARGET): $(OFILES) | $(ODIR)
	$(CC) -o $@ $^ $(LDFLAGS)	

clean:
	@rm -rf $(ODIR)
	@rm $(TARGET)
