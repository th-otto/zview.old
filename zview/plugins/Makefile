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
	signum \
	tga \
	tiff \
	webp \
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
	rm -rf ../_dist/oldcodecs
	mkdir ../_dist/oldcodecs
	for f in $(PLUGINS); do \
		cp -a $$f/*.slb ../_dist/codecs; \
		cp -a $$f/*.ldg ../_dist/oldcodecs; \
	done
	if test -d purec/$(SLB_SUBDIR); then mkdir ../_dist/oldcodecs/purec; cp -a purec/$(SLB_SUBDIR)/* ../_dist/oldcodecs/purec; fi

clean:
	rm -f */*.ldg */*.slb */*.o */*.a */*.lo
	$(MAKE) -C libshare clean
