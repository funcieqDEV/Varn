CXX := clang++-17
CXXFLAGS := -std=c++23 $(shell llvm-config-18 --cxxflags)
LDFLAGS := $(shell llvm-config-18 --ldflags --system-libs --libs core mc support target)
SRC := $(shell find . -name '*.cpp')
OUT := bin/varnc

all: $(OUT)

$(OUT): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) $(LDFLAGS) -o $(OUT)

clean:
	rm -f $(OUT)

.PHONY: all clean
