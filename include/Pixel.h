#ifndef PIXEL_H
#define PIXEL_H

#include <boost/mpi.hpp>
#include <boost/serialization/access.hpp>


class Pixel
{
    private:
        unsigned char m_red;
        unsigned char m_green;
        unsigned char m_blue;
        friend class boost::serialization::access;

        template <class Archive>
        void serialize(Archive &ar, const unsigned int version) {
            ar & m_red;
            ar & m_green;
            ar & m_blue;
        }
    public:
        Pixel();
        Pixel(unsigned char red,unsigned char green,unsigned char blue) : m_red(red), m_green(green), m_blue(blue) {};
        virtual ~Pixel();
        unsigned char getRed();
        void setRed(unsigned char val);
        unsigned char getGreen();
        void setGreen(unsigned char val);
        unsigned char getBlue();
        void setBlue(unsigned char val);
        void setColor (unsigned char red,unsigned char green,unsigned char blue);
};

#endif // PIXEL_H
