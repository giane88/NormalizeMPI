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
Image::Image(mpi::communicator world)
{

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

    if(world.rank() == 0)
    {
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
                            std::cout<<"Processo "<<world.rank()<<": Invio dimensioni..."<<std::endl;
                            broadcast(world, altezza,0);
                            broadcast(world, larghezza, 0);
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
        std::cout<<"Processo "<<world.rank()<<": Invio matrice..."<<std::endl;
        std::cout.flush();
        broadcast(world, immatrix, 0);
    }
    else
    {
        std::cout<<"Processo: "<<world.rank()<<": In attesa di ricezione"<<std::endl;
        broadcast(world, altezza,0);
        broadcast(world, larghezza, 0);
        broadcast(world, immatrix, 0);
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

/**
  * @brief Metodo che individua il valore minimo per ogni colore
  */
void Image::findMin()
{
    int i,j;

    unsigned char l_minRed = 255;
    unsigned char l_minGreen = 255;
    unsigned char l_minBlue = 255;

    std::cout<<"Processo "<<world.rank()<<": Eseguo il ciclo da "<<((world.rank()*larghezza)/world.size())<<" a "<<(((world.rank()+1)*larghezza)/world.size())<<std::endl;
    std::cout.flush();
    for (i = ((world.rank()*larghezza)/world.size()); i < (((world.rank()+1)*larghezza)/world.size()); i++)
    {
        for (j = 0; j < altezza; j++)
        {
            if (immatrix[i][j].getRed() < l_minRed) l_minRed = immatrix[i][j].getRed();
            if (immatrix[i][j].getGreen() < l_minGreen) l_minGreen = immatrix[i][j].getGreen();
            if (immatrix[i][j].getBlue() < l_minBlue) l_minBlue = immatrix[i][j].getBlue();
        }
    }

    if(world.rank() == 0)
    {
        reduce(world,l_minRed, minRed, mpi::minimum<unsigned char>(), 0);
        reduce(world,l_minGreen, minGreen, mpi::minimum<unsigned char>(), 0);
        reduce(world,l_minBlue, minBlue, mpi::minimum<unsigned char>(), 0);
        std::cout<<"Processo "<<world.rank()<<"I valori minimi per RGB sono: "<<int(minRed)<<" "<<int(minGreen)<<" "<<int(minBlue)<<std::endl;
    }
    else
    {
        reduce(world,l_minRed, mpi::minimum<unsigned char>(), 0);
        reduce(world,l_minGreen, mpi::minimum<unsigned char>(), 0);
        reduce(world,l_minBlue, mpi::minimum<unsigned char>(), 0);
    }
    broadcast(world,minRed,0);
    broadcast(world,minGreen,0);
    broadcast(world,minBlue,0);
}

/**
  * @brief Metodo che individua il valore massimo per ogni colore
  */
void Image::findMax()
{
    int i,j;

    unsigned char l_maxRed = 0;
    unsigned char l_maxBlue = 0;
    unsigned char l_maxGreen = 0;

    std::cout<<"Processo "<<world.rank()<<": Eseguo il ciclo da "<<((world.rank()*larghezza)/world.size())<<" a "<<(((world.rank()+1)*larghezza)/world.size())<<std::endl;
    std::cout.flush();
    for (i = ((world.rank()*larghezza)/world.size()); i < (((world.rank()+1)*larghezza)/world.size()); i++)
    {
        for (j = 0; j < larghezza; j++)
        {
            if (immatrix[i][j].getRed() > l_maxRed) l_maxRed = immatrix[i][j].getRed();
            if (immatrix[i][j].getGreen() > l_maxGreen) l_maxGreen = immatrix[i][j].getGreen();
            if (immatrix[i][j].getBlue() > l_maxBlue) l_maxBlue = immatrix[i][j].getBlue();
        }
    }

    if(world.rank() == 0)
    {
        reduce(world,l_maxRed, maxRed, mpi::maximum<unsigned char>(), 0);
        reduce(world,l_maxGreen, maxGreen, mpi::maximum<unsigned char>(), 0);
        reduce(world,l_maxBlue, maxBlue, mpi::maximum<unsigned char>(), 0);
    }
    else
    {
        reduce(world,l_maxRed, mpi::maximum<unsigned char>(), 0);
        reduce(world,l_maxGreen, mpi::maximum<unsigned char>(), 0);
        reduce(world,l_maxBlue, mpi::maximum<unsigned char>(), 0);
    }
    broadcast(world,maxRed,0);
    broadcast(world,maxGreen,0);
    broadcast(world,maxBlue,0);
    std::cout<<"Processo "<<world.rank()<<": I valori massimi per RGB sono: "<<int(maxRed)<<" "<<int(maxGreen)<<" "<<int(maxBlue)<<std::endl;
}

/**
 * @brief Metodo che implementa l'algoritmo di normalizzazione
 */
void Image::normalize()
{
    int i,j;

    findMin();
    findMax();

    for (i = ((world.rank()*larghezza)/world.size()); i < (((world.rank()+1)*larghezza)/world.size()); i++)
    {
        for (j = 0; j < altezza; j++)
        {
            if ((maxRed-minRed) != 0)
            {
                immatrix[i][j].setRed((((immatrix[i][j].getRed() - minRed) * (newMax - newMin))/(maxRed - minRed))+newMin);
            }
            if ((maxGreen-minGreen) != 0)
            {
                immatrix[i][j].setGreen((((immatrix[i][j].getGreen() - minGreen) * (newMax - newMin))/(maxGreen - minGreen))+newMin);
            }
            if ((maxBlue-minBlue) != 0)
            {
                immatrix[i][j].setBlue((((immatrix[i][j].getBlue() - minBlue) * (newMax - newMin))/(maxBlue - minBlue))+newMin);
            }
        }
    }
}

/**
  * @brief Metodo che scrive l'immagine normalizzata sul file
  * @param path Stringa che contiene il percorso nel quale salvare l'immagine
  */
int Image::storeImage(std::string path)
{
    receiveMatrix();

    if(world.rank() == 0)
    {
        outputfile.open(path.c_str());
        if(outputfile.is_open())
        {
            outputfile<<"P3"<<std::endl;
            outputfile<<"#File normalizzato"<<std::endl;
            outputfile<<larghezza<<" "<<altezza<<std::endl;
            outputfile<<int(newMax)<<std::endl;
            printMatrix();
        }
        outputfile.close();
    }

    return 0;
}

/**
 * @brief Metodo che sincronizza la matrice che rappresenta l'immagine
 */
void Image::receiveMatrix()
{
    int i,j;

    if (world.rank() != 0)
    {
        for (i = ((world.rank()*larghezza)/world.size()); i < (((world.rank()+1)*larghezza)/world.size()); i++)
        {
            world.send(0, i, immatrix[i]);
        }
    }
    else
    {
        for (j = 1; j < world.size(); j++)
        {
            for (i = ((j*larghezza)/world.size()); i < (((j+1)*larghezza)/world.size()); i++)
            {
                std::vector<Pixel> temp;
                world.recv(j, i, temp);
                immatrix[i].clear();
                immatrix[i]=temp;
            }
        }
    }
}

/**
 * @brief Metodo che scrive i valori dei pixel su file
 */
void Image::printMatrix()
{
    int i,j;

    for(i = 0; i < larghezza; i++)
    {
        for (j = 0; j < altezza; j++)
        {
            outputfile<<int(immatrix[i][j].getRed())<<std::endl;
            outputfile<<int(immatrix[i][j].getGreen())<<std::endl;
            outputfile<<int(immatrix[i][j].getBlue())<<std::endl;
        }
    }
}
