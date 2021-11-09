compile:
	@rm -f ./build/*

	@clang \
		-Ofast -Wall -Wextra --target=wasm32 --no-standard-libraries -Wno-unused-parameter -Wno-switch\
		-Wl,--no-entry -Wl,--export-dynamic \
		-o build/sim.wasm \
		$(shell find ./src/c -name '*.c')

	@uglifyjs \
		./src/js//ui/primitives/vec2.js \
		./src/js//ui/primitives/point.js \
		./src/js//ui/primitives/primitive.js \
		./src/js//ui/primitives/drawable.js \
		./src/js//ui/primitives/glyphs.js \
		./src/js//ui/eventhandler.js \
		./src/js//ui/elements/element.js \
		./src/js//ui/elements/textnode.js \
		./src/js//ui/elements/button.js \
		./src/js//tools/paint.js \
		./src/js//tools/erase.js \
		./src/js//tools/line.js \
		./src/js//tools/wind.js \
		./src/js//glue.js \
		./src/js//elements.js \
		-o ./build/glue.min.js --compress --mangle #--source-map url=glue.min.js.map

	@cp ./src/favicon.png ./src/logo.png ./src/index.html ./src/style.css ./build
