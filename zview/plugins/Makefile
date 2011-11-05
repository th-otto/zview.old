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
	rm -rf codecs
	mkdir codecs
	for f in $(PLUGINS); do cp $$f/*.ldg codecs; done

clean: $(PLUGINS)
	rm -f *~ *.ldg
