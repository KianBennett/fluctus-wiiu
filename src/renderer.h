#pragma once

#include <math.h>
#include <vector>
#include "draw.h"

extern const float DEG2RAD;
extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

struct Colour {
    char r, g, b, a;

    Colour(char r, char g, char b, char a) : r(r), g(g), b(b), a(a) {}
};

class Renderer {
public:
    Renderer(Colour colour) : colour(colour) {}
    virtual ~Renderer() {}
    virtual void render(float x, float y, float w, float h, float rot) = 0;
protected:
    Colour colour;
};

class QuadRenderer : public Renderer {
public:
    QuadRenderer(Colour colour) : Renderer(colour) {}

    void render(float x, float y, float w, float h, float rot) override {
        rot *= DEG2RAD;

        float x1 = -w / 2; // top left
        float y1 = -h / 2;
        float x2 =  w / 2; // top right
        float y2 = -h / 2;
        float x3 =  w / 2; // bottom right
        float y3 =  h / 2;
        float x4 = -w / 2; // bottom left
        float y4 =  h / 2;

        float x1rot = x1 * cos(rot) - y1 * sin(rot);
        float y1rot = x1 * sin(rot) + y1 * cos(rot);
        float x2rot = x2 * cos(rot) - y2 * sin(rot);
        float y2rot = x2 * sin(rot) + y2 * cos(rot);
        float x3rot = x3 * cos(rot) - y3 * sin(rot);
        float y3rot = x3 * sin(rot) + y3 * cos(rot);
        float x4rot = x4 * cos(rot) - y4 * sin(rot);
        float y4rot = x4 * sin(rot) + y4 * cos(rot);

        drawFillTri(x + x1rot, y + y1rot, x + x2rot, y + y2rot, x + x3rot, y + y3rot, colour.r, colour.g, colour.b, colour.a);
        drawFillTri(x + x3rot, y + y3rot, x + x4rot, y + y4rot, x + x1rot, y + y1rot, colour.r, colour.g, colour.b, colour.a);
    }
};

class TriRenderer : public Renderer {
public:
    TriRenderer(Colour colour) : Renderer(colour) {}

    void render(float x, float y, float w, float h, float rot) override {
        rot *= DEG2RAD;

        float x1 = -w / 2;  // left
        float y1 = -h / 2;
        float x2 =  0;       // top
        float y2 =  h / 2;
        float x3 =  w / 2;  // right
        float y3 = -h / 2;

        float x1rot = x1 * cos(rot) - y1 * sin(rot);
        float y1rot = x1 * sin(rot) + y1 * cos(rot);
        float x2rot = x2 * cos(rot) - y2 * sin(rot);
        float y2rot = x2 * sin(rot) + y2 * cos(rot);
        float x3rot = x3 * cos(rot) - y3 * sin(rot);
        float y3rot = x3 * sin(rot) + y3 * cos(rot);

        drawFillTri(x + x1rot, y + y1rot, x + x2rot, y + y2rot, x + x3rot, y + y3rot, colour.r, colour.g, colour.b, colour.a);
    }
};

class WaveRenderer : public Renderer {
public:
    float offset, amp, freq, height;

    WaveRenderer(Colour colour) : Renderer(colour) {}

    void updateValues(float offset, float amp, float freq, float height) {
        this->offset = offset;
        this->amp = amp;
        this->freq = freq;
        this->height = height;
    }

    void render(float x, float y, float w, float h, float rot) override {
        int segments = 12;
        float segmentWidth = SCREEN_WIDTH / (float) segments;

        for(int i = 0; i < segments; i++) {
            float segX = i * segmentWidth;
            float nextSegX = (i + 1) * segmentWidth;
            float segY = SCREEN_HEIGHT / 2.0f + amp * sin(freq * (segX + offset));
            float nextSegY = SCREEN_HEIGHT / 2.0f + amp * sin(freq * (nextSegX + offset));

            float vertices[12];

            // vertices[0] = segX;				    // top left
            // vertices[1] = segY - height / 2;
            // vertices[2] = segX + segmentWidth;	    // bottom right
            // vertices[3] = nextSegY + height / 2;
            // vertices[4] = segX;					// bottom left
            // vertices[5] = segY + height / 2;
            // vertices[6] = segX;					// top left
            // vertices[7] = segY - height / 2;
            // vertices[8] = segX + segmentWidth;	    // top right
            // vertices[9] = nextSegY - height / 2;
            // vertices[10] = segX + segmentWidth;    // bottom right
            // vertices[11] = nextSegY + height / 2;

            // drawTri(x + vertices[0], y + vertices[1], x + vertices[2], y + vertices[3], x + vertices[4], y + vertices[5], colour.r, colour.g, colour.b, colour.a);
            // drawTri(x + vertices[6], y + vertices[7], x + vertices[8], y + vertices[9], x + vertices[10], y + vertices[11], colour.r, colour.g, colour.b, colour.a);

            drawLine(segX, segY - height / 2, nextSegX, nextSegY - height / 2, colour.r, colour.g, colour.b, colour.a);
            drawLine(segX, segY - height / 2 - 1, nextSegX, nextSegY - height / 2 - 1, colour.r, colour.g, colour.b, colour.a);
            drawLine(segX, segY + height / 2, nextSegX, nextSegY + height / 2, colour.r, colour.g, colour.b, colour.a);
            drawLine(segX, segY + height / 2 + 2, nextSegX, nextSegY + height / 2 + 2, colour.r, colour.g, colour.b, colour.a);
        }
    }
};