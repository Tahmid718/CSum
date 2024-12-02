#ifndef CALCULATE_H
#define CALCULATE_H

#include <vector>

#include <wx/wx.h>

class Hash
{
    public:
        void CalculateHash();
        void hfsCalculation(std::vector<std::string> PlainStrings);
        std::vector<unsigned char> Symmetric();
};

#endif