top_srcdir=../..
include $(top_srcdir)/configvars

PLUGINS = \
	bmp \
	degas \
	doodle \
	eureka \
	gif \
	godpaint \
	img \
	jpg \
	neochrome \
	png \
	tga \
	tiff \
	$(empty)

all:: $(PLUGINS)

.PHONY: $(PLUGINS)
$(PLUGINS): libshare
	$(MAKE) -C $@ all

libshare::
	$(MAKE) -C $@ all

dist:
	rm -rf ../_dist/codecs
	mkdir ../_dist/codecs
	for f in $(PLUGINS); do cp -a $$f/*.slb ../_dist/codecs; done

clean:
	rm -f */*.ldg */*.slb */*.o */*.a */*.lo
	$(MAKE) -C libshare clean
