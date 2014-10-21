#include "Image.h"


/**
 * @brief Costruttore della classe che inizializza i valori della normalizzazione
 * @param nMin valore minimo della normalizzazione
 * @param nMax valore massimo della normalizzazione
 * @param world Comunicatore per le funzioni MPI
 */
Image::Image(unsigned char newMin, unsigned char newMax, mpi::communicator world): newMin(newMin), newMax(newMax), world(world) {};

/**
 * @brief Costruttore della classe che inizializza i valori della normalizzazione ai valori di default 0 e 255
 * @param world Comunicatore per le funzioni MPI
 */

Image::Image(mpi::communicator world) {

	world = world;
	newMin = 0;
	newMax = 255;
}

/**
  * @brief Metodo che carica la foto da file
  * @param path Stringa che contiene il percorso da quale caricare l'immagine
  */
void Image::loadImage(std::string path)
{
    std::string line;
    std::string slen, salt;
    int nline = 0;


    if(world.rank() == 0) {
        inputfile.open(path.c_str());
        if(inputfile.is_open())
        {
            while(getline(inputfile, line))
            {
                if((line.compare("P3") == 0)&&(nline == 0))
                {
                    std::cout<<"File conforme proseguo..."<<std::endl;
                    nline++;
                }
                else
                {
                    if(line.compare(0,1,"#") == 0)
                    {
                        std::cout<<"Riga di commento"<<std::endl;
                    }
                    else
                    {
                        if(nline == 1)
                        {
                            slen = line.substr(0,line.find_first_of(" "));
                            salt = line.substr(line.find_first_of(" ")+1);
                            larghezza = atoi(slen.c_str());
                            altezza = atoi(salt.c_str());
                            std::cout<<"Lunghezza "<<larghezza<<std::endl;
                            std::cout<<"Altezza "<<altezza<<std::endl;
                            nline++;
                        }
                        else
                        {
                            if (nline > 1)
                            {
                                fillMatrix();
                            }
                        }
                    }
                }
            }
        }
        else
        {
            std::cerr<<"Errore apertura file"<<std::endl;
        }
        inputfile.close();
	std::cout<<"Processo "<<world.rank()<<": Invio matrice,,,"<<std::endl;
	std::cout.flush();
        world.send(1, 0, immatrix);
    }else{
	std::cout<<"Processo: "<<world.rank()<<": In attesa di ricezione"<<std::endl;
        world.recv(0, 0, immatrix);
	std::cout<<"Processo "<<world.rank()<<": Ricezione avvenuta..."<<std::endl;
	std::cout.flush();
    }
}

/**
  * @brief Metodo che riempe la matrice di pixel con i valori dell'immagine
  */
void Image::fillMatrix()
{
    std::string red;
    std::string green;
    std::string blue;
    int i,j;

    for(i = 0; i < larghezza; i++)
    {
        std::vector<Pixel> temp;
        for (j = 0; j < altezza; j++)
        {
            getline(inputfile, red);
            getline(inputfile, green);
            getline(inputfile, blue);
            temp.push_back(Pixel((unsigned char) atoi(red.c_str()), (unsigned char) atoi(green.c_str()), (unsigned char) atoi(blue.c_str())));
        }
        immatrix.push_back(temp);
    }
}

/**
 * @brief Distruttore della classe
 */
Image::~Image()
{
}


/**
 * @brief Metodo che stampa la matrice per il test
 */
void Image::testPrint()
{
    int i,j;

    std::cout<<"Processo "<<world.rank()<<std::endl;
    for (i = 0; i < larghezza; i++)
    {
        for (j = 0; j < altezza; j++)
        {
            std::cout<<int(immatrix[i][j].getRed())<<" "<<int(immatrix[i][j].getGreen())<<" "<<int(immatrix[i][j].getBlue())<<"\t";
	    std::cout.flush();
        }
        std::cout<<std::endl;
    }
}

