CXX=mpic++
IDIR= "include"
CXXFLAG= -c -std=c++11 -Wall -I$(IDIR) -fopenmp
LDFLAG= -lboost_mpi -lboost_serialization -fopenmp
SOURCE= src/Pixel.cpp src/Image.cpp main.cpp
OBJECT= $(SOURCE:.cpp=.o)
TARGHET= NormalizeImg
all: $(TARGHET)

$(TARGHET): $(OBJECT)
	$(CXX) $(OBJECT) -o $@ $(LDFLAG)

%.o: %.cpp
	$(CXX) $(CXXFLAG) $< -o $@
clean:
	rm -f $(OBJECT) $(TARGHET)
test:
	echo $(SOURCE)
	echo $(OBJECT)

