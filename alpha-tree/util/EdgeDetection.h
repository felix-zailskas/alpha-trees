#ifndef EDGE_DETECTION_H
#define EDGE_DETECTION_H

#include "common.h"

double SimpleSalience(Pixel p, Pixel q);
double WeightedSalience(Pixel p, Pixel q);
double EdgeStrengthX(Pixel *img, int width, int height, int x, int y, SalienceFunction Salience);
double EdgeStrengthY(Pixel *img, int width, int height, int x, int y, SalienceFunction Salience);

#endif