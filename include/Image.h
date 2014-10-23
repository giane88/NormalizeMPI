#ifndef IMAGE_H
#define IMAGE_H

#include <boost/mpi.hpp>
#include <iostream>
#include <fstream>
#include <omp.h>
#include <cstdlib>
#include <cstdio>
#include "Pixel.h"

namespace mpi=boost::mpi;

class Image
{
    public:
        Image (mpi::communicator world);
        Image (unsigned char newMin, unsigned char newMax, mpi::communicator world);
        void loadImage(std::string path);
        int storeImage(std::string path);
        void  normalize();
        void testPrint();
        virtual ~Image();
    private:
        std::ifstream inputfile;
        std::ofstream outputfile;
        std::vector<std::vector<Pixel>> immatrix;
        int altezza;
        int larghezza;
        unsigned char newMin;
        unsigned char newMax;
        unsigned char minRed, minGreen,minBlue;
        unsigned char maxRed, maxGreen,maxBlue;
        mpi::communicator world;

        void findMin();
        void findMax();
        void fillMatrix();
        void receiveMatrix();
        void printMatrix();
};
#endif
