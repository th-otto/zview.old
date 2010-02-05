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

.PHONY: $(PLUGINS)
$(PLUGINS):
	$(MAKE) -C $@ $(MAKECMDGOALS)
	cp $@/*.ldg .

all: $(PLUGINS)

clean: $(PLUGINS)
	rm -f *~ *.ldg

