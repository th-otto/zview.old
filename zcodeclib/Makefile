all::
	$(MAKE) -C src clean CPU=""
	$(MAKE) -C src CPU=""

	$(MAKE) -C src clean CPU="-m68020-60"
	$(MAKE) -C src CPU="-m68020-60"

	$(MAKE) -C src clean CPU="-mcpu=5475"
	$(MAKE) -C src CPU="-mcpu=5475"

clean::
	for CPU in "" -m68020-60 -mcpu=5475; do $(MAKE) -C src CPU="$$CPU" $@; done
