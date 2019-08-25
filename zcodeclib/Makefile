all::
	$(MAKE) -C src clean CPU="-m68000"
	$(MAKE) -C src CPU="-m68000"

	$(MAKE) -C src clean CPU="-m68020-60"
	$(MAKE) -C src CPU="-m68020-60"

	$(MAKE) -C src clean CPU="-mcpu=5475"
	$(MAKE) -C src CPU="-mcpu=5475"

clean::
	for CPU in -m68000 -m68020-60 -mcpu=5475; do $(MAKE) -C src CPU="$$CPU" $@; done
