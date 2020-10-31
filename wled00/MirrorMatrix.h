#ifndef MirrorMatrix_h
#define MirrorMatrix_h


// NeoPixelBusGfx example for a simple 32 x 8 pixel matrix.
// Scrolls 'Howdy' across the matrix in a portrait (vertical) orientation.

#include <NeoPixelBrightnessBusGfx.h>
#include <NeoPixelBrightnessBus.h>
#include <Fonts/Org_04.h>

#define M_PIN 4

#define WIDTH 32
#define HEIGHT 8

// See NeoPixelBus documentation for choosing the correct Feature and Method
// (https://github.com/Makuna/NeoPixelBus/wiki/NeoPixelBus-object)
NeoPixelBrightnessBusGfx<NeoGrbFeature, Neo800KbpsMethod> matrix(WIDTH, HEIGHT, M_PIN);

// See NeoPixelBus documentation for choosing the correct NeoTopology
// you may also use NeoTile or NeoMosaik 
// (https://github.com/Makuna/NeoPixelBus/wiki/Matrix-Panels-Support)
NeoTopology<ColumnMajorAlternating180Layout> topo(WIDTH, HEIGHT);

const uint16_t colors[] = {
  matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 0, 255) };

// use a remap function to remap based on the topology, tile or mosaik
// this function is passed as remap function to the matrix
uint16_t remap(uint16_t x, uint16_t y) {
  return topo.Map(x, y);
}

    void MatrixBegin()
    {
        matrix.Begin();

        // pass the remap function
        matrix.setRemapFunction(&remap);
        matrix.setTextWrap(false);
       // matrix.SetBrightness(40);
        matrix.setTextColor(colors[0]);
        matrix.setFont(&Org_04);
    }

    void Matrix_Print_Time(String time, bool show, bool dim)
    {
        int16_t x1, y1;
        uint16_t w, h;
        matrix.getTextBounds(time, 1, 5, &x1, &y1, &w, &h);

        if (w > 28)
        {
            matrix.setCursor(uint16_t((32 - (w)) / 2) - 3, 5);
        }
        else
        {
            matrix.setCursor(uint16_t((32 - w) / 2), 5);
        }

        matrix.fillScreen(0);
        matrix.print(time);

        if (show)
        {
            if (dim)
            {
                //matrix.setBrightness(10);
            }
            else
            {
               // matrix.setBrightness(255);
            }

            // FastLED.show();
        }
        else
        {
           // matrix.setBrightness(255);
        }
    }
#endif


    