#ifndef A10_H_PHUDVTKB
#define A10_H_PHUDVTKB

#include "Image.h"

using std::string;
using std::cout;
using std::cerr;
using std::endl;
using std::vector;

// Combine Image
Image combine(const Image &inputL, const Image &inputH, float sigma);

// Get image URLS
vector<string> imageUrls(string name, string size, int count);

// Downloads JPEG from converts to PNG
Image downloadPng(const string &url, int w = 0, int h = 0, bool average = false);

// Makes wxh image mosic from a search query
Image mosaic(int w, int h, int px, int py, string query, bool average = false);

// Makes row of mosaics
Image combineMosaics(int w, int h, int px, int py, vector<string> queries, bool average = false);

// combines and amalgamation of count number of photos.
Image amalgamate(int px, int py, string query, int count);

#endif /* end of include guard: A10_H_PHUDVTKB */
