#include "Pixel.h"

Pixel::Pixel()
{
    m_red = 0;
    m_green = 0;
    m_blue = 0;
}

Pixel::~Pixel()
{
    //dtor
}
void Pixel::setBlue(unsigned char val)
{
    m_blue = val;
}

unsigned char Pixel::getBlue()
{
    return m_blue;
}

void Pixel::setGreen(unsigned char val)
{
    m_green = val;
}

unsigned char Pixel::getGreen()
{
    return m_green;
}

void Pixel::setRed(unsigned char val)
{
    m_red = val;
}

unsigned char Pixel::getRed()
{
    return m_red;
}


void Pixel::setColor(unsigned char red, unsigned char green, unsigned char blue) {
    m_red = red;
    m_green = green;
    m_blue = blue;
}
