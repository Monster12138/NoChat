#include <iostream>
#include "json/json.h"
#include <string>

using namespace std;

int main()
{
    string name = "zhangsan";
    string passwd = "123456";

    Json::Value root;
    root["name"] = name;
    root["passwd"] = passwd;
    
    Json::StreamWriterBuilder w;
    Json::StreamWriter* writer = w.newStreamWriter();
    writer->write(root, cout);
    return 0;
}

