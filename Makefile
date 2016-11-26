##
#
##


CXX = g++
CXXFLAGS = -std=c++11 -g

BINARY1 = add_matrices
OBJECTS1 = $(BINARY1).o

BINARY2 = gen_matrix
OBJECTS2 = $(BINARY2).o

BINARY3 = find_string
OBJECTS3 = $(BINARY3).o
HEADERS3 = suffix_tree.hpp

SRC_DIR = src
INCL_DIR = include

BASE_INCL = -I $(INCL_DIR)


ALL_INCL = $(BASE_INCL)
ALL_FLAGS = $(CXXFLAGS)


default: $(BINARY3)
all: $(BINARY1) $(BINARY2) $(BINARY3)

$(BINARY1): $(OBJECTS1)
	$(CXX) -o $@ $^ $(ALL_FLAGS) $(ALL_LDFLAGS) $(ALL_LIBS)

$(BINARY2): $(OBJECTS2)
	$(CXX) -o $@ $^ $(ALL_FLAGS) $(ALL_LDFLAGS) $(ALL_LIBS)

$(BINARY3): $(OBJECTS3) $(HEADERS3)
	$(CXX) -o $@ $< $(ALL_FLAGS) $(ALL_LDFLAGS) $(ALL_LIBS)

%.o: $(SRC_DIR)/%.cpp
	$(CXX) -c $< -o $@ $(ALL_FLAGS) $(ALL_INCL)

clean:
	rm -f $(OBJECTS1) $(OBJECTS2) $(OBJECTS3) $(BINARY1) $(BINARY2) $(BINARY3)
