HTML=ui_debug/gui.html
HEADER=include/gui_html.h
GUARD=GUI_HTML_H

all: $(HEADER)

$(HEADER): $(HTML)
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

clean:
	rm -f $(HEADER)
