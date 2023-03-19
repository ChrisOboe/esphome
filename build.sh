#!/usr/bin/env bash

for i in */*.yaml; do
	NAME=$(yq '.substitutions.name' "$i")
	if [ "$NAME" == "null" ] || [ "$NAME" == "" ]; then
		continue
	fi
	set -e
	esphome compile "$i"
	mkdir -p builds
	cp build/$NAME/.pioenvs/$NAME/firmware.bin builds/$NAME.bin
	set +e
done
