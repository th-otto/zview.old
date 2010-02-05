PLUGINS = \
	bmp \
	degas \
	eureka \
	gif \
	godpaint \
	img \
	jpg \
	neochrome \
	png \
	raw \
	tga \
	tiff

.PHONY: $(PLUGINS)
$(PLUGINS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

default: all

all: $(PLUGINS)

clean: $(PLUGINS)
	rm -f *~
