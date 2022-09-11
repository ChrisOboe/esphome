VPATH = smartplugs
YAMLS := $(notdir $(wildcard smartplugs/*.yaml))
DEFAULTS := $(wildcard defaults/*.yaml)

.PHONY: all
all:
	make $(patsubst %.yaml, build/.pioenvs/%/firmware.bin,$(YAMLS))

build/.pioenvs/%/firmware.bin: %.yaml $(DEFAULTS)
	esphome compile $<
