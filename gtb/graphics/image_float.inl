
/*

Copyright 2007 University of Utah


This file is part of Afront.

Afront is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published
by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Afront is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301, USA

*/


GTB_BEGIN_NAMESPACE

inline ImageFloat::ImageFloat(const Image& image) : 
    Image(image.width(), image.height(), GL_RGB, GL_FLOAT)
{
    //
    // copy image (format convert)
    //
    for (int y = 0; y < _height; ++y)
    {
        for (int x = 0; x < _width; ++x)
        {
            set_pixel_rgb(x, y, image(x, y));
        }
    }
}

inline ImageFloat::ImageFloat(GLsizei width, GLsizei height) :
    Image(width, height, GL_RGB, GL_FLOAT)
{
}

inline ColorRgb& ImageFloat::operator() (int x, int y)
{
    ColorRgb* ppixel = (ColorRgb*)_pixels;
    ppixel += x + y*_width;
    return *ppixel;
}

inline const ColorRgb& ImageFloat::operator() (int x, int y) const
{
    const ColorRgb* ppixel = (const ColorRgb*)_pixels;
    ppixel += x + y*_width;
    return *ppixel;
}


GTB_END_NAMESPACE
