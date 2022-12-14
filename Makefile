TARGET = funkin
TYPE = ps-exe

SRCS = src/main.c \
       src/mutil.c \
       src/random.c \
       src/archive.c \
       src/font.c \
       src/trans.c \
       src/loadscr.c \
       src/menu.c \
       src/stage.c \
       src/psx/psx.c \
       src/psx/io.c \
       src/psx/gfx.c \
       src/psx/audio.c \
       src/psx/pad.c \
       src/psx/timer.c \
       src/stage/dummy.c \
       src/stage/week1.c \
       src/stage/vtown.c \
       src/stage/simp.c \
       src/stage/factory.c \
       src/stage/valley.c \
       src/animation.c \
       src/character.c \
       src/character/bf.c \
       src/character/speaker.c \
       src/character/dad.c \
       src/character/gf.c \
       src/character/bfv.c \
       src/character/vrage.c \
       src/character/cancer.c \
       src/character/bfz.c \
       src/character/hand.c \
       src/character/zord.c \
       src/character/homer.c \
       src/character/x.c \
       src/character/green.c \
       src/character/porky.c \
       src/character/ness.c \
       src/character/clucky.c \
       src/character/chuck.c \
       src/character/sneed.c \
       src/object.c \
       src/object/combo.c \
       src/object/splash.c \
       mips/common/crt0/crt0.s

CPPFLAGS += -Wall -Wextra -pedantic -mno-check-zero-division
LDFLAGS += -Wl,--start-group
# TODO: remove unused libraries
LDFLAGS += -lapi
#LDFLAGS += -lc
LDFLAGS += -lc2
#LDFLAGS += -lcard
LDFLAGS += -lcd
#LDFLAGS += -lcomb
LDFLAGS += -lds
LDFLAGS += -letc
LDFLAGS += -lgpu
#LDFLAGS += -lgs
LDFLAGS += -lgte
#LDFLAGS += -lgun
#LDFLAGS += -lhmd
#LDFLAGS += -lmath
#LDFLAGS += -lmcrd
#LDFLAGS += -lmcx
LDFLAGS += -lpad
LDFLAGS += -lpress
#LDFLAGS += -lsio
LDFLAGS += -lsnd
LDFLAGS += -lspu
#LDFLAGS += -ltap
LDFLAGS += -flto -Wl,--end-group

include mips/common.mk
