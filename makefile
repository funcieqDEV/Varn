CXX := clang++-17

# Pobierz cxxflags od llvm-config-18 i usuń -std=c++17
LLVM_CXXFLAGS := $(shell llvm-config-18 --cxxflags | sed 's/-std=c++[0-9][0-9]//g')
CXXFLAGS := -std=c++23 $(LLVM_CXXFLAGS)

LDFLAGS := $(shell llvm-config-18 --ldflags --system-libs --libs core mc support target)

SRC := $(shell find . -name '*.cpp')
OUT := bin/varnc

all: $(OUT)

$(OUT): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) $(LDFLAGS) -o $(OUT)

clean:
	rm -f $(OUT)

.PHONY: all clean
