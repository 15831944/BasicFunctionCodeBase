#ifndef __WEATHER_H__
#define __WEATHER_H__

#include <string>

#define CITYID_GET_URL      "http://61.4.185.48:81/g/"
#define WeatherInfoURL_ConturyCenter        "http://m.weather.com.cn/atad/{cityID}.html"
#define WeatherInfoBACKURL_ConturyCenter    "http://weather.51wnl.com/weatherinfo/GetMoreWeather?cityCode={cityID}&weatherType=0"
#define AirQualityInfoURL_Xiaomi        "http://weatherapi.market.xiaomi.com/wtr-v2/weather?cityId={cityID}"
#define AirQualityInfoBACKURL_Xiaomi    "http://weatherapi.market.xiaomi.com/wtr-v2/weather?cityId={cityID}"

#define Default_Http_Get_TimeOut    8
#define Default_CityID              "101010100"

#define ERROR_GET_CITYID    "ERROR_CITYID"
#define ERROR_GET_WEATHER   "ERROR_WEATHER"
#define ERROR_GET_AQI       "ERROR_AQI"


#ifdef __cplusplus

enum ImgNO_ {
    ImgNO_0 = 0,    // 0：晴
    ImgNO_1,    // 1：多云
    ImgNO_2,    // 2：阴
    ImgNO_3,    // 3：阵雨
    ImgNO_4,    // 4：雷阵雨
    ImgNO_5,    // 5：雷阵雨并伴有冰雹
    ImgNO_6,    // 6：雨夹雪
    ImgNO_7,    // 7：小雨
    ImgNO_8,    // 8：中雨
    ImgNO_9,    // 9：大雨
    ImgNO_10 = 10,   // 10：暴雨
    ImgNO_11,   // 11：大暴雨
    ImgNO_12,   // 12：特大暴雨
    ImgNO_13,   // 13：阵雪
    ImgNO_14,   // 14：小雪
    ImgNO_15,   // 15：中雪
    ImgNO_16,   // 16：大雪
    ImgNO_17,   // 17：暴雪
    ImgNO_18,   // 18：雾
    ImgNO_19,   // 19：冻雨
    ImgNO_20 = 20,   // 20：沙尘暴
    ImgNO_21,   // 21：小雨-中雨
    ImgNO_22,   // 22：中雨-大雨
    ImgNO_23,   // 23：大雨-暴雨
    ImgNO_24,   // 24：暴雨-大暴雨
    ImgNO_25,   // 25：大暴雨-特大暴雨
    ImgNO_26,   // 26：小雪-中雪
    ImgNO_27,   // 27：中雪-大雪
    ImgNO_28,   // 28：大雪-暴雪
    ImgNO_29,   // 29：浮尘
    ImgNO_30 = 30,   // 30：扬沙
    ImgNO_31,   // 31：强沙尘暴
    ImgNO_UnDef
};

/**
 *  WeatherForecast类
 *  获取天气预报信息；
 **/
class WeatherForecast
{
    public:
        WeatherForecast(std::string cityId);
        ~WeatherForecast();

        /**
         *  flag:
         *      0-city name in English ;
         *      1-city name in Chinese ;
         **/
        std::string GetCity(int flag);
        std::string SetCity(std::string c, int flag);

        /**
         *  flag vlaue:
         *      0-night image ;
         *      1-day image ;
         **/
        ImgNO_ GetImgNo(int flag);
        ImgNO_ SetImgNo(ImgNO_ i, int flag);

        std::string GetTemp()
        {
            return temp;
        }
        void SetTemp(std::string t)
        {
            temp = t;
        }

        std::string GetWeather()
        {
            return weather;
        }
        void SetWeather(std::string w)
        {
            weather = w;
        }

    protected:
        std::string GetURL()
        {
            return url;
        }
        void SetURL(std::string urlParten);
        std::string  GetInfo();

    private:
        std::string url;
        std::string urlBack;
        std::string cityCode;
        std::string city;
        std::string city_en;
        std::string temp;
        std::string weather;
        ImgNO_ dayImgNo;
        ImgNO_ nightImgNo;
};


/**
 *  AQIQuery类
 *  获取空气质量指数AQI；
 **/
class AQIQuery
{
    public:
        AQIQuery(std::string cityId);
        ~AQIQuery();

        int GetAQI()
        {
            return aqi;
        }
        void SetAQI(int a)
        {
            aqi = a;
        }

        int GetPM10()
        {
            return pm10;
        }
        void SetPM10(int p)
        {
            pm10 = p;
        }

        int GetPM25()
        {
            return pm25;
        }
        void SetPM25(int p)
        {
            pm25 = p;
        }

        int GetNO2()
        {
            return no2;
        }
        void SetNO2(int n)
        {
            no2 = n;
        }

        int GetSO2()
        {
            return so2;
        }
        void SetSO2(int s)
        {
            so2 = s;
        }

    protected:
        std::string GetURL()
        {
            return url;
        }
        void SetURL(std::string urlParten) ;
        std::string  GetInfo();

    private:
        std::string url;
        std::string urlBack;
        std::string cityCode;
        int aqi;
        int pm10;
        int pm25;
        int no2;
        int so2;
};


/**
 *  Weather类
 *  开放接口，封装天气预报和AQI接口；
 **/
class Weather : public WeatherForecast, public AQIQuery
{
    public:
        Weather(std::string cityCodeURL);
        ~Weather();
        int GetWeatherInfo(std::string weatherUrlParten, std::string aqiUrlParten);

    private:
        std::string cityIdUrl;
};


#endif // __cplusplus
#endif
