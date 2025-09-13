# Makefile for Arco
# Lucy Nyuu 2025

CC = gcc
INCDIR = include
CFLAGS = -Wall -Wextra -g -I$(INCDIR) -lm -fPIC -shared -Ilv2
TARGET = $(OBJDIR)/arco.so
SRCDIR = src
OBJDIR = $(BUILDDIR)/intermediate
BUILDDIR = build
SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))

# LV2 bundle directory
BUNDLE = $(BUILDDIR)/arco.lv2
DATA_DIR = lv2

all: $(BUNDLE)

$(BUNDLE): $(TARGET)
	mkdir -p $(BUNDLE)
	cp $(TARGET) $(BUNDLE)/
	cp $(DATA_DIR)/*.ttl $(BUNDLE)/

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -f $(TARGET) $(OBJDIR)/*.o
	rm -rf $(BUNDLE)
