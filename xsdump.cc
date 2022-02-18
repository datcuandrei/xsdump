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

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>

#include <png++/png.hpp>
#include <png++/image.hpp>
#include <png++/rgb_pixel.hpp>
#include <png++/types.hpp>

#define VERSION_NUMBER 1.2

Display *dpy;
Screen *screen;
int scr;
XImage *img;

struct screenSize
{
    int width;
    int height;
};

void init_x();
void destroy_x();

int main(int argc, char *argv[])
{
    init_x();
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

    std::string output; // the destination of the file will be stored here
    if(argc >= 2)
    {
        // helpful commands for the users
        if(strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
        {
            fprintf(stdout,"Usage: xsdump OPTION\n\nOptions are:\n\n-o or --output [PATH]: outputs the image to PATH(include name of file & extension must be .png), otherwise is saved in HOME\n-h or --help: shows this help menu\n-v or --version: shows xsdump's version information");
        }else if(strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0)
        {
            fprintf(stdout,"xsdump %g\nA very simple screenshot tool for X11.\nCopyright (C) 2022 Andrei Datcu <https://datcuandrei.github.io>.\nLicensed under GPL v2: <https://github.com/datcuandrei/xsdump/blob/main/LICENSE>\nVisit <https://github.com/datcuandrei/xsdump> for more information.", VERSION_NUMBER);
        }else if(strcmp(argv[1], "-o") == 0 || strcmp(argv[1], "--output") == 0) // a way for the user to save the screen dump where he wants it.
        {
            output = argv[2];
            image.write(output);
            fprintf(stdout,"[SUCCESS] Screen dumped!");
        }    
    }else
    {
        // by default, it will be saved in the home directory of the current user.
        output = getenv("HOME");
        output.append("/xsdump-dump.png");
        image.write(output);
        fprintf(stdout,"[SUCCESS] Screen dumped!");
    }

    destroy_x();
    exit(EXIT_SUCCESS);
}

void init_x()
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
}

void destroy_x()
{
    XDestroyImage(img); // destroying the image connection
    XCloseDisplay(dpy); // closing the connection to the X server.
}
