/*
    xsdump - A very simple screenshot tool for X11. 
    Copyright (C) 2022 Andrei Datcu.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
#include <iostream>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>

#include <png++/png.hpp>
#include <png++/image.hpp>
#include <png++/rgb_pixel.hpp>
#include <png++/types.hpp>

#define VERSION_NUMBER 1.1

Display *dpy;
Screen *screen;
int scr;
XImage *img;

typedef struct screenSize
{
    int width;
    int height;
}res;

void help()
{
    std::cout << "Usage: xsdump OPTION" << std::endl;
    std::cout << std::endl;
    std::cout << "Options are: " << std::endl;
    std::cout << std::endl;
    std::cout << "-h or --help: shows this help menu" << std::endl;
    std::cout << "-v or --version: shows xsdump's version information" << std::endl;
}

int main(int argc, char *argv[])
{
    // connecting to xserver display
    dpy = XOpenDisplay(NULL);

    // in case there is no server
    if(dpy == NULL)
    {
        fprintf(stderr, "[ERROR] Can't connect to XServer display.\n");
        exit(EXIT_FAILURE);
    }

    // setting the default screen
    scr = DefaultScreen(dpy);
    screen = XDefaultScreenOfDisplay(dpy);
    

    screenSize res = {XWidthOfScreen(screen), XHeightOfScreen(screen)}; // getting the resolution of the screen

    img = XGetImage(dpy,screen->root,0,0,res.width,res.height,AllPlanes,ZPixmap); // grabbing a screen dump of the whole screen (all displays)
    
    // rgb masks
    u_long rm = 0xFF0000; // red mask
    u_long gm = 0xFF00;   // green mask
    u_long bm = 0xFF;     // blue mask

    /* 
        image matrix
        using png++ (wrapper for libpng)
        why use png++ when you can use libpng?
        because it is much easier and faster to get work done.
    
        we are getting each pixel at the appropiate x and y coords
        then that pixel is placed in the image matrix at those x and y coords
        with the according rgb values(encoded using bit shifting).
    */
    png::image<png::rgb_pixel> image(res.width, res.height);
    for(png::uint_32 x = 0; x < res.width; ++x)
    {
        for(png::uint_32 y = 0; y < res.height; ++y)
        {
            u_long pixel = XGetPixel(img, x, y);

            u_char red = (pixel & rm) >> 16;
            u_char green = (pixel & gm) >> 8;        
            u_char blue = pixel & bm;
        
            image[y][x] = png::rgb_pixel(red, green, blue);
        }
    }

    std::string output;
    if(argc >= 2)
    {
        if(strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
        {
            fprintf(stdout,"Usage: xsdump OPTION\n\nOptions are:\n\n-o or --output [PATH]: outputs the image to PATH(include name of file & extension must be .png), otherwise is saved in HOME\n-h or --help: shows this help menu\n-v or --version: shows xsdump's version information");
        }else if(strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0)
        {
            fprintf(stdout,"xsdump %g\nA very simple screenshot tool for X11.\nCopyright (C) 2022 Andrei Datcu.\nLicense GPLv2: <https://www.gnu.org/licenses/old-licenses/gpl-2.0.html>\nThis is free software: you are free to change and redistribute it.\nThere is NO WARRANTY, to the extent permitted by law.\n\nWritten by Andrei Datcu.", VERSION_NUMBER);
        }else if(strcmp(argv[1], "-o") == 0 || strcmp(argv[1], "--output") == 0)
        {
            output = argv[2];
            image.write(output);
            std::cout << "[SUCCESS] Screen dumped!";
        }    
    }else
    {
        output = getenv("HOME");
        output.append("/xsdump-dump.png");
        image.write(output);
        std::cout << "[SUCCESS] Screen dumped!";
    }

    // debugging gibberish
    /*std::cout << std::endl;
    std::cout << "[DEBUG]" << std::endl;
    std::cout << "RMask " << "GMask " << "BMask " << std::endl;
    std::cout << "----- " << "----- " << "----- " << std::endl;
    std::cout << rm << " " << gm << " " << bm << std::endl;
    std::cout << "Resolution: " << res.width << "x" << res.height << std::endl;
    std::cout << "Root Window ID: " << screen->root << std::endl;
    std::cout << "Display: " << dpy << std::endl;
    std::cout << std::endl;
    std::cout << "[SUCCESS] Screen dumped!";*/

    exit(EXIT_SUCCESS);
}
// end