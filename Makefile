
build: bin/
	gcc src/parse.c -o bin/jsonparse

test: build
	bin/jsonparse test.json

bin/:
	mkdir bin