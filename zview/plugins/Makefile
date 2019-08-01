include ../../configvars

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

all: $(PLUGINS)

.PHONY: $(PLUGINS)
$(PLUGINS): libshare
	$(MAKE) -C $@ all

libshare::
	$(MAKE) -C $@ all

dist:
	rm -rf ../_dist/codecs
	mkdir ../_dist/codecs
	for f in $(PLUGINS); do cp $$f/*.ldg ../_dist/codecs; done
	$(STRIP) -s ../_dist/codecs/*.ldg

clean:
	rm -f */*.ldg */*.slb */*.o */*.a */*.lo
	$(MAKE) -C libshare clean
