#include <iostream>
#include <boost/mpi.hpp>
#include <vector>
#include "Image.h"

namespace mpi=boost::mpi;

int main (int argc, char *argv[]) {

	std::cout<<"Inizializzazione dell'ambiente MPI"<<std::endl;
	mpi::environment env;
    mpi::communicator world;
    std::string path;
    Image *im;

    if(world.rank() == 0) {

        if (argc == 1){
            std::cerr<<"Errore nel numero di parametri utilizzare: NormalizeImg <path> [<min> <max>]"<<std::endl;
            return -1;
        }
        if ((argc > 2)&&(argc < 4)) {
            std::cout<<"Ricevuti in ingresso due parametri per la normalizzazione"<<atoi(argv[2])<<" "<<atoi(argv[3])<<std::endl;
            im = new Image(atoi(argv[2]),atoi(argv[3]),world);
        } else {
            std::cout<<"Nessun parametro opzionale utilizzo 0 255"<<std::endl;
            im = new Image(world);
        }
        path = argv[1];
        std::cout<<"Caricamento dell'immagine"<<path<<std::endl;
    } else {
        im = new Image(world);
    }
    im->loadImage(path);
    im->normalize();
    if (world.rank() == 0){
        unsigned found = path.find_last_of("/");
        path = path.substr(0,found)+"/normalize.ppm";
        std::cout<<"Salvataggio dell'immagine in: "<<path<<std::endl;
    }
    im->storeImage(path);
    im->testPrint();
    delete im;
}
