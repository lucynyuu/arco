# Makefile for Arco
# Lucy Nyuu 2025

CC = clang
INCDIR = include
CFLAGS = -Wall -Wextra -I$(INCDIR) -lm -fPIC -shared -I./lv2-1.18.10/include
BUILDDIR = build
OBJDIR = $(BUILDDIR)/intermediate

SRCDIR = src
TARGET = $(OBJDIR)/arco.so
SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))

# Frontend
UI_INCDIR = ui/include
UI_CFLAGS = -Wall -Wextra -fPIC -shared -I$(UI_INCDIR) -I./lv2-1.18.10/include -ObjC -framework Cocoa -framework WebKit
UI_SRCDIR = ui/src
UI_TARGET = $(OBJDIR)/arco_ui.so
UI_SRCS = $(wildcard $(UI_SRCDIR)/*.m)
UI_OBJS = $(patsubst $(UI_SRCDIR)/%.c,$(OBJDIR)/%.o,$(UI_SRCS))

# LV2 bundle directory
BUNDLE = $(BUILDDIR)/arco.lv2
DATA_DIR = lv2

HTML=ui/ui_debug/gui.html
HTML_HEADER=ui/include/gui_html.h
GUARD=GUI_HTML_H

all: $(HTML_HEADER) $(UI_TARGET) $(BUNDLE) 

$(HTML_HEADER): $(HTML)
	{ \
	  echo "#ifndef $(GUARD)"; \
	  echo "#define $(GUARD)"; \
	  echo; \
	  printf "const char* const HTML_CONTENT = \""; \
	  tr -d '\n' < $(HTML) \
	    | sed -E 's/[[:space:]]+/ /g; \
	              s/ *([{}();:,=<>+\-\*/]) */\1/g; \
	              s/\\/\\\\/g; \
	              s/\"/\\\"/g' \
	    | tr -d '\n'; \
	  echo "\";"; \
	  echo; \
	  echo "#endif // $(GUARD)"; \
	} > $@

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
	rm -f $(HTML_HEADER)
