#include <fstream>
#include <iostream>
#include <map>
#include "util.h"
using namespace util;
std::map<std::string, std::string> Parse_SnapdragonProfiler_Vulkan(const std::string& path)
{
    std::vector<std::string> listings;
    int nameId = -1, clocksId = -1, shadersBusysId = -1, readTotalId = -1, writeTotalId = -1, parametersId = -1;

    int drawcall = 0;
    int index = 0;
    int clocks = 0;
    int shadersBusysCount = 0;
    float shadersBusys = 0.f, shadersBusysMin = -1.f, shadersBusysMax = 0.f;
    int readTotal = 0;
    int writeTotal = 0;

    std::map<std::string, std::string> result;

    std::ifstream csv_file(path);
    std::string line;
    int lineId = 0;
    while (std::getline(csv_file, line))
    {
        if (lineId == 0)
        {
            std::vector<std::string> keys = splitString(line, ',');
            for (int i = 0; i < keys.size(); i++)
            {
                listings.push_back(trim(keys[i]));
            }
            nameId = findItemIndex(listings, "Name");
            clocksId = findItemIndex(listings, "Clocks");
            shadersBusysId = findItemIndex(listings, "% Shaders Busy");
            readTotalId = findItemIndex(listings, "Read Total (Bytes)");
            writeTotalId = findItemIndex(listings, "Write Total (Bytes)");
            parametersId = findItemIndex(listings, "Parameters");

            if (nameId == -1)
            {
                std::cout << "ERROR: not find drawcall !" << std::endl;
                return result;
            }
        }
        else
        {
            std::vector<std::string> items = splitString(line, ',');
            if (nameId != -1)
            {
                std::string name = trim(items[nameId]);
                if (containsSubstring(name, "Draw"))
                {
                    drawcall++;
                    std::string value = trim(items[parametersId]);
                    std::vector<std::string> parameters = splitString(removeParentheses(value), ' ');
                    std::string index_count = parameters[6];
                    std::string instance_count = parameters[9];
                    index += std::stoi(index_count) * std::stoi(instance_count);
                }
            }
            if (clocksId != -1)
            {
                std::string value = trim(items[clocksId]);
                if (value != "")
                {
                    clocks += std::stoi(value);
                }
            }
            if (shadersBusysId != -1)
            {
                std::string value = trim(items[shadersBusysId]);
                if (value != "")
                {
                    float value_f = std::stof(value);
                    shadersBusys += value_f;
                    if (shadersBusysMin == -1.f)
                    {
                        shadersBusysMin = value_f;
                    }
                    else
                    {
                        shadersBusysMin = std::min(shadersBusysMin, value_f);
                    }
                    
                    shadersBusysMax = std::max(shadersBusysMax, value_f);
                    shadersBusysCount++;
                }
            }
            if (readTotalId != -1)
            {
                std::string value = trim(items[readTotalId]);
                if (value != "")
                {
                    int value_i = std::stoi(value);
                    readTotal += value_i;
                }
            }
            if (writeTotal != -1)
            {
                std::string value = trim(items[writeTotalId]);
                if (value != "")
                {
                    int value_i = std::stoi(value);
                    writeTotal += value_i;
                }
            }
            
        }
        lineId++;
    }

    if (nameId != -1)
    {
        result["drawcall"] = std::to_string(drawcall);
        result["index"] = std::to_string(index);
        result["face"] = std::to_string(index/3);
    }
    if (clocksId != -1)
    {
        result["GPU General - clocks"] = std::to_string(clocks);
    }
    if (shadersBusysId != -1)
    {
        result["GPU Shader Processing - % shaders busy (avg)"] = std::to_string(shadersBusys / shadersBusysCount);
        result["GPU Shader Processing - % shaders busy (min)"] = std::to_string(shadersBusysMin);
        result["GPU Shader Processing - % shaders busy (max)"] = std::to_string(shadersBusysMax);
    }
    if (readTotalId != -1)
    {
        result["GPU Memory Stats - read total (Bytes)"] = std::to_string(readTotal);
        result["GPU Memory Stats - read total (KB)"] = std::to_string(readTotal/1024.f);
        result["GPU Memory Stats - read total (MB)"] = std::to_string(readTotal/1024.f/1024.f);
        result["GPU Memory Stats - read total (GB)"] = std::to_string(readTotal/1024.f/1024.f/1024.f);
    }
    if (writeTotal != -1)
    {
        result["GPU Memory Stats - write total (Bytes)"] = std::to_string(writeTotal);
        result["GPU Memory Stats - write total (KB)"] = std::to_string(writeTotal/1024.f);
        result["GPU Memory Stats - write total (MB)"] = std::to_string(writeTotal/1024.f/1024.f);
        result["GPU Memory Stats - write total (GB)"] = std::to_string(writeTotal/1024.f/1024.f/1024.f);
    }
    return result;
}

int main(int argc, char** argv)
{
    if (argc != 5)
    {
        std::cout << "args : profiler + api + input_path + output_path" << std::endl;
        std::cout << "example : gparse snapdragon vulkan D:/ExportFromProfiler.csv D:/ParseResult.csv" << std::endl;
        return -1;
    }
    std::string profiler = argv[1];
    std::string api = argv[2];
    std::string ipath = argv[3];
    std::string opath = argv[4];

    bool check = true;
    if (getFileExtension(ipath) != "csv")
    {
        std::cout << "The input file needs to be a .csv file!" << std::endl;
        check = false;
    }
    if (getFileExtension(opath) != "csv")
    {
        std::cout << "The output file needs to be a .csv file!" << std::endl;
        check = false;
    }
    if (!fileExists(ipath))
    {
        std::cout << "The input file not exists!" << std::endl;
        check = false;
    }
    if (!check)
    {
        return 0;
    }
    
    if (profiler == "snapdragon")
    {
        if (api == "vulkan")
        {
            auto result = Parse_SnapdragonProfiler_Vulkan(ipath);
            createDirectoryIfNotExists(getDirectoryPath(opath));
            std::ofstream ofile(opath);
            for (auto pair : result)
            {
                ofile << pair.first << "," << pair.second << std::endl;
            }
            std::cout << "done" << std::endl;
        }
        else
        {
            std::cout << "not find api : " << api << std::endl;
        }
    }
    else
    {
        std::cout << "not find profiler : " << profiler << std::endl;
    }
    return 0;    
}