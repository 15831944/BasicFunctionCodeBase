#include <iostream>
#include <fstream>
#include <cassert>
#include <string>
#include <json/json.h>

#include "Log/LogC.h"
#include "logUtils.h"


void readJson();
void writeJson();

int main(int argc, char **argv)
{
    readJson();

    std::cout << "========================================================" << std::endl;
    writeJson();

    return 0;
}

void readJson()
{
    using namespace std;
    std::string strValue = "{\"name\":\"json\",\"array\":[{\"cpp\":\"jsoncpp\"},{\"java\":\"jsoninjava\"},{\"php\":\"support\"}]}";

    Json::Reader reader;
    Json::Value value;

    if (reader.parse(strValue, value)) {
        std::string out = value["name"].asString();
        std::cout << out << std::endl;
        const Json::Value arrayObj = value["array"];
        for (unsigned int i = 0; i < arrayObj.size(); i++) {
            if (!arrayObj[i].isMember("cpp"))
                continue;
            out = arrayObj[i]["cpp"].asString();
            std::cout << out;
            if (i != (arrayObj.size() - 1))
                std::cout << std::endl;
        }
    }

    return ;
}





/**
========================================================
{
   "array" : [
      {
         "cpp" : "jsoncpp",
         "java" : "jsoninjava",
         "php" : "support"
      }
   ],
   "name" : "json"
}
========================================================
 **/
void writeJson()
{
    using namespace std;

    Json::Value root;
    Json::Value arrayObj;
    Json::Value item;
    Json::Reader reader;
    Json::Value value;


    item["cpp"] = "jsoncpp";
    item["java"] = 12345;
    item["php"] = "support";
    arrayObj.append(item);

    root["name"] = "json";
    root["array"] = arrayObj;

    root.toStyledString();
    std::string strValue = root.toStyledString();
    std::cout << strValue << std::endl;

    std::cout << "=====================writeJson=====================" << std::endl;

    if (reader.parse(strValue, value)) {
        std::string out = value["name"].asString();
        std::cout << out << std::endl;
        const Json::Value arrayObj1 = value["array"];
        for (unsigned int i = 0; i < arrayObj1.size(); i++) {
            if (!arrayObj1[i].isMember("cpp"))
                continue;
            int aa = arrayObj1[i]["java"].asInt();
            std::cout << aa;
            if (i != (arrayObj1.size() - 1))
                std::cout << std::endl;
        }
    }
    std::cout << "" << std::endl;
    
    
    return ;
}




int ReadFromFile()
{
    using namespace std;
    ifstream ifs;
    ifs.open("testjson.json");
    assert(ifs.is_open());

    Json::Reader reader;
    Json::Value root;
    if (!reader.parse(ifs, root, false)) {
        return -1;
    }

    std::string name;
    int age;
    int size = root.size();
    for (int i = 0; i < size; ++i) {
        name = root[i]["name"].asString();
        age = root[i]["age"].asInt();

        std::cout << name << " " << age << std::endl;
    }

    return 0;
}



int WriteToFile()
{
    using namespace std;
    Json::Value root;
    Json::FastWriter writer;
    Json::Value person;

    person["name"] = "hello world";
    person["age"] = 100;
    root.append(person);

    std::string json_file = writer.write(root);


    ofstream ofs;
    ofs.open("test1.json");
    assert(ofs.is_open());
    ofs << json_file;

    return 0;
}










