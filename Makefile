# Makefile for Arco
# Lucy Nyuu 2025

CC = gcc
INCDIR = include
CFLAGS = -Wall -Wextra -g -I$(INCDIR) -lm -fPIC -shared -Ilv2
BUILDDIR = build
OBJDIR = $(BUILDDIR)/intermediate

SRCDIR = src
TARGET = $(OBJDIR)/arco.so
SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))

# Frontend
UI_CFLAGS = -Wall -Wextra -g -lm -fPIC -shared -Ilv2 -IX11
UI_SRCDIR = ui
UI_TARGET = $(OBJDIR)/arco_ui.so
UI_SRCS = $(wildcard $(UI_SRCDIR)/*.c)
UI_OBJS = $(patsubst $(UI_SRCDIR)/%.c,$(OBJDIR)/%.o,$(UI_SRCS))

# LV2 bundle directory
BUNDLE = $(BUILDDIR)/arco.lv2
DATA_DIR = lv2

all: $(UI_TARGET) $(BUNDLE) 

$(BUNDLE): $(TARGET)
	mkdir -p $(BUNDLE)
	cp $(TARGET) $(BUNDLE)/
	cp $(UI_TARGET) $(BUNDLE)/
	cp $(DATA_DIR)/*.ttl $(BUNDLE)/

# Link main
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Link UI
$(UI_TARGET): $(UI_OBJS)
	$(CC) $(UI_CFLAGS) -o $(UI_TARGET) $(UI_OBJS)

# Compile main
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile UI
$(OBJDIR)/%.o: $(UI_SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(UI_CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

# rm -f $(TARGET) $(OBJDIR)/*.o
clean:
	rm -f $(TARGET) $(UI_TARGET) $(OBJDIR)/*.o
	rm -rf $(BUNDLE)
