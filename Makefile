CXX=g++
CXXFLAGS=-Wall -Wextra -pedantic -std=c++17 -O0 -g
LDFLAGS=$(CXXFLAGS)
OBJ=$(SRC:.cc=.o)

all:  test_bitio test_huffman encoder decoder

test_bitio: bitio.o test_bitio.o
	$(CXX) $(LDFLAGS) -o $@ $^

test_tree: test_tree.o tree.o
	$(CXX) $(LDFLAGS) -o $@ $^

test_huffman: huffman.o hforest.o htree.o test_huffman.o
	$(CXX) $(LDFLAGS) -o $@ $^

test_hforest: test_hforest.o hforest.o
	$(CXX) $(LDFLAGS) -o $@ $^

test_htree: test_htree.o htree.o
	$(CXX) $(LDFLAGS) -o $@ $^

encoder: encoder.o bitio.o huffman.o hforest.o htree.o
	$(CXX) $(LDFLAGS) -o $@ $^

decoder: decoder.o  bitio.o huffman.o hforest.o htree.o
	$(CXX) $(LDFLAGS) -o $@ $^

%.o: %.cc %.hh
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) -c -o $@ $<

clean:
	rm -rf *.o test_bitio encoder decoder test_huffman test_htree *.comp *.plaintext

test: all
	./test_huffman
	./test_bitio
