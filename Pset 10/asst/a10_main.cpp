#include <iostream>

#include "a10.h"

using namespace std;

vector<string> colorsEnglish {"red","red,orange","orange","orange,yellow",
  "yellow","yellow,green","green","green,blue","blue","blue,purple","purple"};
vector<string> colorsSpanish {"rojo","rojo,naranja","naranja","naranja,amarillo",
  "amarillo","amarillo,verde","verde","verde,azul","azul","azul,morado","morado"};
vector<string> colorsFrench {"rouge","rouge,orange","orange","orange,jaune",
  "jaune","jaune,vert","vert","vert,bleu","bleu","bleu,mauve","mauve"};
vector<string> colorsItalian {"rosso","rosso,arancione","arancione","arancione,giallo",
  "giallo","giallo,verde","verde","verde,blu","blu","blu,porpora","porpora"};
vector<string> colorsGerman {"rot","rot,orange","orange","orange,gelb","gelb",
  "gelb,grün","grün","grün,blau","blau","blau,lila","lila"};

// Test downloading a photo from Flickr
void testDownloadImage() {
  vector<string> urls = imageUrls("yellow", "s", 1);
  Image img = downloadPng(urls[0]);
  img.write("./Output/testDownload.png");
};

// Test getting the average color of a Flickr photo
void testDownloadAverage() {
  vector<string> urls = imageUrls("yellow", "s", 1);
  Image img = downloadPng(urls[0], 50, 50, true);
  img.write("./Output/testDownloadAverage.png");
}

// Test createing a mosic of Flickr photos (single query)
void testMosaic() {
  Image img = mosaic(10, 10, 25, 25, "red,orange");
  img.write("./Output/testMosaic.png");
}

// Test createing a mosic of Flickr photos color averages (single query)
void testMosaicAverage() {
  Image img = mosaic(10, 10, 25, 25, "purple", true);
  img.write("./Output/testMosaicAverage.png");
}

//Test createing a engligh color hue mosic from photos
void testCombineMosaicsEnglish() {
  Image img = combineMosaics(5,10,25,25,colorsEnglish);
  img.write("./Output/testCombineMosaicsEnglish.png");
}

//Test createing a engligh color hue mosic from photo averages
void testCombineMosaicsEnglishAvg() {
  Image img = combineMosaics(5,10,25,25,colorsEnglish, true);
  img.write("./Output/testCombineMosaicsEnglishAvg.png");
}

//Test createing a engligh color hue mosic from photos
void testCombineMosaicsSpanish() {
  Image img = combineMosaics(5,10,25,25,colorsSpanish);
  img.write("./Output/testCombineMosaicsSpanish.png");
}

//Test createing a engligh color hue mosic from photo averages
void testCombineMosaicsSpanishAvg() {
  Image img = combineMosaics(5,10,25,25,colorsSpanish, true);
  img.write("./Output/testCombineMosaicsSpanishAvg.png");
}

//Test createing a french color hue mosic from photos
void testCombineMosaicsFrench() {
  Image img = combineMosaics(5,10,25,25,colorsFrench);
  img.write("./Output/testCombineMosaicsFrench.png");
}

//Test createing a french color hue mosic from photo averages
void testCombineMosaicsFrenchAvg() {
  Image img = combineMosaics(5,10,25,25,colorsFrench, true);
  img.write("./Output/testCombineMosaicsFrenchAvg.png");
}

//Test createing a italian color hue mosic from photos
void testCombineMosaicsItalian() {
  Image img = combineMosaics(5,10,25,25,colorsItalian);
  img.write("./Output/testCombineMosaicsItalian.png");
}

//Test createing a italian color hue mosic from photo averages
void testCombineMosaicsItalianAvg() {
  Image img = combineMosaics(5,10,25,25,colorsItalian, true);
  img.write("./Output/testCombineMosaicsItalianAvg.png");
}

//Test createing a german color hue mosic from photos
void testCombineMosaicsGerman() {
  Image img = combineMosaics(5,10,25,25,colorsGerman);
  img.write("./Output/testCombineMosaicsGerman.png");
}

//Test createing a german color hue mosic from photo averages
void testCombineMosaicsGermanAvg() {
  Image img = combineMosaics(5,10,25,25,colorsGerman, true);
  img.write("./Output/testCombineMosaicsGermanAvg.png");
}

// Create amalgamation from photos with query "mit"
void testAlagamateMIT() {
  Image img = amalgamate(600,400,"mit",50);
  img.write("./Output/testAlagamateMIT.png");
}

// Create amalgamation from photos with query "massachusetts,institute,technology"
void testAlagamateMITNoAbr() {
  Image img = amalgamate(600,400,"massachusetts,institute,technology",40);
  img.write("./Output/testAlagamateMITNoAbr.png");
}

// Create amalgamation from photos with query "mit,dome"
void testAlagamateDome() {
  Image img = amalgamate(600,400,"mit,dome",50);
  img.write("./Output/testAlagamateDome.png");
}

// Create amalgamation from photos with query "boston,day"
void testAlagamateBostonDay() {
  Image img = amalgamate(600,400,"boston,day",25);
  img.write("./Output/testAlagamateBostonDay.png");
}

// Create amalgamation from photos with query "boston,night"
void testAlagamateBostonNight() {
  Image img = amalgamate(600,400,"boston,night",50);
  img.write("./Output/testAlagamateBostonNight.png");
}

// Create amalgamation from photos with query "las,vegas,night"
void testAlagamateLasVegasNight() {
  Image img = amalgamate(600,400,"las,vegas,night",50);
  img.write("./Output/testAlagamateLasVegasNight.png");
}

// Create amalgamation from photos with query "vegas,day"
void testAlagamateLasVegasDay() {
  Image img = amalgamate(600,400,"vegas,day",50);
  img.write("./Output/testAlagamateLasVegasDay.png");
}

int main()
{
    testDownloadImage();
    testDownloadAverage();
    testMosaic();
    testMosaicAverage();
    testCombineMosaicsEnglish();
    testCombineMosaicsEnglishAvg();
    testCombineMosaicsFrench();
    testCombineMosaicsFrenchAvg();
    testCombineMosaicsSpanish();
    testCombineMosaicsSpanishAvg();
    testCombineMosaicsItalian();
    testCombineMosaicsItalianAvg();
    testCombineMosaicsGerman();
    testCombineMosaicsGermanAvg();
    testAlagamateMIT();
    testAlagamateMITNoAbr();
    testAlagamateDome();
    testAlagamateBostonDay();
    testAlagamateBostonNight();
    testAlagamateLasVegasNight();
    testAlagamateLasVegasDay();
    return EXIT_SUCCESS;
}
