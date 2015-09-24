#include <iostream>
#include <string>

#include "Weather.h"
extern "C" {
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "HttpFetcher.h"
}

#define   TEST_HttpGet        1

using namespace std;

#if TEST_HttpGet
extern int http_fetch(const char *url, char **fileBuf);
extern void http_setTimeout(int seconds);

static std::string HttpGet00(std::string url)
{

    //const char *url = "http://m.weather.com.cn/atad/101010100.html";
    //char fileBuf[2048] = {};
    char *buf = (char *)malloc(2048 * sizeof(char));
    std::string content;

    http_setTimeout(5);
    http_fetch((const char *)url.c_str(), &buf);
    const char *strerr = http_strerror();

    http_perror(strerr);
    cout << strerr << " :::: " << endl;
    content = buf;
    cout << content << endl;

    return content;
}
#endif


int main ()
{
    Weather *ww = new Weather(CITYID_GET_URL);
    int errInfo = ww->GetWeatherInfo(WeatherInfoURL_ConturyCenter, AirQualityInfoURL_Xiaomi);
    cout << "main::" << endl;

    cout << ww->GetCity(0) << "  ";
    cout << ww->GetCity(1) << "  ";

    int time = 23;
    if ((time < 18) && (time > 7))
        cout << ww->GetImgNo(0) << "  ";
    else
        cout << ww->GetImgNo(1) << "  ";
    if (ww->GetImgNo(1) >= ImgNO_UnDef)
        cout << ImgNO_UnDef << "  ";

    cout << ww->GetTemp() << "  ";
    cout << ww->GetWeather() << "  ";

    cout << ww->GetAQI() << "  ";
    cout << ww->GetPM10() << "  ";
    cout << ww->GetPM25() << "  ";
    cout << ww->GetNO2() << "  ";
    cout << ww->GetSO2() << "  ";

    cout << endl;

    std::string errCode;
    if (errInfo & 1) { // get cityID error
        errCode = ERROR_GET_CITYID;
        cout << "error get city id..." << errCode << endl;
    } else {
        if (errInfo & (1 << 1)) { // get weather info error
            errCode = ERROR_GET_WEATHER;
            cout << "error get city id..." << errCode << endl;
        }
        if (errInfo & (1 << 2)) { // get aqi info error
            errCode = ERROR_GET_AQI;
            cout << "error get city id..." << errCode << endl;
        }
    }

#if TEST_HttpGet
    std::string content = HttpGet00(WeatherInfoBACKURL_ConturyCenter);
    cout << "main:" << content << endl;
#endif
    return 0;
}
