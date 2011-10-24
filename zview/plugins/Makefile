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
	tga \
	tiff

default: all

.PHONY: $(PLUGINS)
$(PLUGINS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

all: $(PLUGINS)

clean: $(PLUGINS)
	rm -f *~ *.ldg
