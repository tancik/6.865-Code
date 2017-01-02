#include <iostream>
#include <curl/curl.h>
#include <Magick++.h>
#include "json/json.h"

#include "a10.h"
#include "filtering.h"

using namespace std;

string imageData;

std::size_t callback(const char* in, std::size_t size, std::size_t num, string* out)
{
    const std::size_t totalBytes(size * num);
    out->append(in, totalBytes);
    return totalBytes;
}

Image downloadPng(const string &url, int w, int h, bool average) {
  CURL *image;
  CURLcode imgresult;

  image = curl_easy_init();
  struct curl_slist *headers=NULL; // init to NULL is important
  curl_slist_append( headers, "Accept: image/jpeg");
  curl_slist_append( headers, "Content-type: image/jpeg");
  if( image ){
      int httpCode(0);
      std::unique_ptr<std::string> httpData(new std::string());
      curl_easy_setopt(image, CURLOPT_HTTPHEADER, headers);
      curl_easy_setopt(image, CURLOPT_URL, url.c_str());
      curl_easy_setopt(image, CURLOPT_WRITEFUNCTION, callback);
      curl_easy_setopt(image, CURLOPT_WRITEDATA, httpData.get());
      // Grab image
      imgresult = curl_easy_perform(image);
      curl_easy_getinfo(image, CURLINFO_RESPONSE_CODE, &httpCode);
      curl_easy_cleanup(image);

      if (httpCode == 200) {
        try {
          string imgData = *httpData.get();
          Magick::Image img;
          Magick::Blob blob(static_cast<const void *>(imgData.c_str()), imgData.size());
          img.read(blob);
          if (w == 0) {
            ostringstream dimen;
            dimen << w << "x" << h;
            img.resize(dimen.str());
          }
          img.magick("png");
          img.write("./Output/temp.png");
          Image png("./Output/temp.png");

          if (average) {
            Image average = Image(1,1,3);
            for (int y = 0; y < png.height(); y++) {
              for (int x = 0; x < png.width(); x++) {
                for (int c = 0; c < png.channels(); c++) {
                  average(0,0,c) += png(x,y,c);
                }
              }
            }
            average = average / float(png.width() * png.height());
            for (int y = 0; y < png.height(); y++) {
              for (int x = 0; x < png.width(); x++) {
                for (int c = 0; c < average.channels(); c++) {
                  png(x,y,c) = average(0,0,c);
                }
              }
            }
            return png;
          } else {
            return png;
          }
        } catch(...) {
            return Image(w,h,3);
        }
      } else {
        cout << "Couldn't GET image from " << url << " - exiting" << std::endl;
      }
  } else {
    std::cout << "didn't open" << std::endl;
  }
  return Image(0);
}



string getRequestUrl(string query) {
  ostringstream url;
  url << "https://api.flickr.com/services/rest/?method=flickr.photos.search&api_key=786fa69f8f34ed7a8279a02c58b43d84&sort=relevance&safe_search=true&text=" << query << "&format=json&nojsoncallback=1";
  return url.str();
}

vector<string> imageUrls(string query, string size, int count) {
  vector<string> strings;
  string url = getRequestUrl(query);
  CURL *curl;
  struct curl_slist *headers=NULL; // init to NULL is important
  curl_slist_append( headers, "Accept: application/json");
  curl_slist_append( headers, "Content-Type: application/json");
  curl_slist_append( headers, "charsets: utf-8");
  curl = curl_easy_init();

  if( curl ){
      curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
      curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
      curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
      curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);

      int httpCode(0);
      std::unique_ptr<std::string> httpData(new std::string());

      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, httpData.get());

      // Grab image
      curl_easy_perform(curl);
      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
      curl_easy_cleanup(curl);

      if (httpCode == 200) {
        cout << "\nGot successful response from " << url << std::endl;
        Json::Value jsonData;
        Json::Reader jsonReader;
        if (jsonReader.parse(*httpData.get(), jsonData)) {
          cout << "Successfully parsed JSON data" << std::endl;

          const Json::Value photo = jsonData["photos"]["photo"];
          for ( int index = 0; index < count; ++index ) {
            ostringstream photoUrl;
            photoUrl << "https://farm" << photo[index]["farm"] << ".staticflickr.com/" << photo[index]["server"].asString() << "/" << photo[index]["id"].asString() << "_" << photo[index]["secret"].asString() << "_" << size << ".jpg";
            strings.push_back(photoUrl.str());
          }
        } else {
          cout << "Could not parse HTTP data as JSON" << std::endl;
          cout << "HTTP data was:\n" << *httpData.get() << std::endl;
          return strings;
        }
      } else {
        cout << "Couldn't GET from " << url << " - exiting" << std::endl;
        return strings;
      }
  } else {
    std::cout << "didn't open" << std::endl;
    curl_easy_cleanup(curl);
  }
  return strings;
}

Image mosaic(int w, int h, int px, int py, string query, bool average) {
  Image newImage(w*px, h*py, 3);
  vector<string> urls = imageUrls(query, "s", w*h);

  for (int j = 0; j < h; j++) {
      for (int i = 0; i < w; i++){
        Image img = downloadPng(urls[i+j*w], px, py, average);
        for (int y = 0; y < py; y++) {
          for (int x = 0; x < px; x++) {
            // std::cout << "x=" << x+i*px << "y=" << y+j*py << std::endl;
            for (int c = 0; c < img.channels(); c++) {
              newImage(x+i*px, y+j*py, c) = img(x,y,c);
            }
          }
        }
      }
  }
  return newImage;
}

Image combineMosaics(int w, int h, int px, int py, vector<string> queries, bool average) {
  Image newImage(w*px*queries.size(), h*py, 3);
  for (int i = 0; i < queries.size(); i++) {
    Image img = mosaic(w, h, px, py, queries[i], average);
    for (int y = 0; y < img.height(); y++) {
      for (int x = 0; x < img.width(); x++) {
        for (int c = 0; c < img.channels(); c++) {
          newImage(x + i*img.width(), y, c) = img(x,y,c);
        }
      }
    }
  }
  return newImage;
}

Image amalgamate(int px, int py, string query, int count) {
  Image newImage(px,py,3);
  vector<string> urls = imageUrls(query, "z", count);
  for (int i = 0; i < urls.size(); i++){
    Image img = downloadPng(urls[i], px, py);
    for (int y = 0; y < newImage.height(); y++) {
      for (int x = 0; x < newImage.width(); x++) {
        for (int c = 0; c < img.channels(); c++) {
          newImage(x, y, c) += img.smartAccessor(x, y, c, true)/float(count);
        }
      }
    }
  }
  return newImage;
}
