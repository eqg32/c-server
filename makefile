build/server: build/main.o build/http.o build/list.o build/handlers.o build/child_process.o
	gcc build/main.o build/http.o build/child_process.o build/list.o build/handlers.o -o build/server -ltls	
build/main.o: src/main.c
	gcc -c src/main.c -o build/main.o
	
build/handlers.o: src/handlers.c include/handlers.h
	gcc -c src/handlers.c -Iinclude -o build/handlers.o

build/child_process.o: src/child_process.c include/child_process.h
	gcc -c src/child_process.c -Iinclude -o build/child_process.o

build/http.o: src/http.c include/http.h
	gcc -c src/http.c -Iinclude -o build/http.o

build/list.o: src/list.c include/list.h
	gcc -c src/list.c -Iinclude -o build/list.o

clean:
	rm build/*
