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

default: $(PLUGINS)

.PHONY: $(PLUGINS)
$(PLUGINS):
	$(MAKE) -C $@ $(MAKECMDGOALS) CROSS=yes

dist:
	rm -rf plugins
	mkdir plugins
	for f in $(PLUGINS); do cp $$f/*.ldg plugins; done

clean: $(PLUGINS)
	rm -f *~ *.ldg
