# TODO: simplify

CXX=g++
LDFLAGS=-lpthread -lbladeRF -lboost_program_options -lfftw3 -lm

all: directories dist/atab

dist/atab: build/main.o build/utils.o build/sdr.o build/bladeRF.o build/iqrecord.o
	$(CXX) build/*.o $(LDFLAGS) -o $@

build/main.o: src/main.cpp
	$(CXX) -c $< -o $@

build/utils.o: src/utils.cpp
	$(CXX) -c $< -o $@

build/sdr.o: src/sdr/sdr.cpp
	$(CXX) -c $< -o $@

build/bladeRF.o: src/sdr/bladeRF.cpp
	$(CXX) -c $< -o $@

build/iqrecord.o: src/sdr/iqrecord.cpp
	$(CXX) -c $< -o $@

directories:
	mkdir -p dist build

clean:
	rm -rf build/
	rm -rf dist/