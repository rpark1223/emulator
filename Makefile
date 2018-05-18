# 	  Makefile for COMP 40 Homework 6
#
#     Author: Robin Park, Sang Woo Kim
# 
# 	  Includes build rules for um.


############## Variables ###############

CC = gcc

IFLAGS = -I/comp/40/include -I/usr/sup/cii40/include/cii

CFLAGS = -g -std=gnu99 -Wall -Wextra -Werror -Wfatal-errors -pedantic $(IFLAGS)

LDFLAGS = -g -L/comp/40/lib64 -L/usr/sup/cii40/lib64

LDLIBS = -lnetpbm -lm -lrt -lbitpack -lcii40

INCLUDES = $(shell echo *.h)

############### Rules ###############

all: um

um: main.o um_emulator.o segments.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

clean:
	rm -f um *.o