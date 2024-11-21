all: qSieve GaussBin

qSieve: qSieve.c
	gcc -o quadSieve qSieve.c -lgmp -g

GaussBin: GaussBin.c
	gcc GaussBin.c -o GaussBin

clean:
	rm quadSieve
	rm GaussBin
	rm binInput
	rm binOutput
