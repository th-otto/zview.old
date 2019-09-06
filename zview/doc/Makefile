HCP = hcp
UDO = udo

UDO_SOURCES = \
	zview.u \
	config.ui \
	credits.ui \
	faq.ui \
	file_formats.ui \
	glossary.ui \
	helppage.ui \
	history.ui \
	intro.ui \
	menu.ui \
	notes.ui \
	using.ui \
	$(empty)

all:: zview.hyp


zview.hyp: zview.stg
	$(HCP) -o $@ $<

zview.stg: $(UDO_SOURCES)
	$(UDO) --stg --no-warningslines -o $@ zview.u || { $(RM) $@; exit 1; }
