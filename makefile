build/server: build/main.o build/http.o \
	build/list.o build/child_process.o build/config.o
	gcc build/main.o build/http.o \
	build/child_process.o build/list.o build/config.o \
	-o build/server -lssl -lcrypto

build/main.o: src/main.c
	gcc -c src/main.c -o build/main.o

build/child_process.o: src/child_process.c include/child_process.h
	gcc -c src/child_process.c -Iinclude -o build/child_process.o

build/http.o: src/http.c include/http.h
	gcc -c src/http.c -Iinclude -o build/http.o

build/config.o: src/config.c include/config.h
	gcc -c src/config.c -Iinclude -o build/config.o

build/list.o: src/list.c include/list.h
	gcc -c src/list.c -Iinclude -o build/list.o

certificate:
	openssl genpkey -algorithm RSA -out private/privkey.pem -pkeyopt rsa_keygen_bits:2048
	openssl req -new -key private/privkey.pem -out private/request.csr
	openssl x509 -req -days 365 -in private/request.csr -signkey private/privkey.pem -out private/certificate.pem
	
clean:
	rm build/*
