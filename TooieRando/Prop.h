#pragma once
#include <string>
#include <vector>

class Prop
{
public:
    //From Decomp
    /* 0x0 */ int16_t position[3];
    /* 0x6 */ uint16_t unk6_7=0;// : 9; //Directly moved into unk74_7 in Actor
    /* 0x6 */ uint16_t unk6_1=0;// : 6; //unk4 << 0x19 >> 0x1A
    /* 0x6 */ uint16_t unk6_0=0;// : 1; //unk6 >> 0x7 << 0x7
    /* 0x8 */ uint16_t ItemID=0;
    /* 0x0A */ uint16_t unkA=0; //0xA
    /* 0x0C */ uint32_t FlagOrRotation=0;// : 9; //unkC >> 0x17
    /* 0x0C */ uint32_t unkC_0=0;// : 23; //0xC
    /* 0x10 */ uint16_t unk10=0; //0x10
    /* 0x12 */ uint16_t unk12=0; //0x12
    //Convert this Prop into a character vector
    std::vector<unsigned char> Prop::OutputData()
    {
        std::vector<unsigned char> Data{};
        Data.push_back(position[0]>>8);
        Data.push_back(position[0]&0xFF);
        Data.push_back(position[1] >> 8);
        Data.push_back(position[1] & 0xFF);
        Data.push_back(position[2] >> 8);
        Data.push_back(position[2] & 0xFF);
        uint16_t combined = unk6_7 << 0x7 | unk6_1 << 0x1 | unk6_0;
        Data.push_back(combined >> 8);
        Data.push_back(combined & 0xFF);
        Data.push_back(ItemID >> 8);
        Data.push_back(ItemID & 0xFF);
        Data.push_back(unkA >> 8);
        Data.push_back(unkA & 0xFF);
        uint32_t flagAndOther = FlagOrRotation << 23 | unkC_0;
        Data.push_back((flagAndOther >> 24) & 0xFF);
        Data.push_back((flagAndOther >> 16) & 0xFF);
        Data.push_back((flagAndOther >> 8) & 0xFF);
        Data.push_back(flagAndOther & 0xFF);
        Data.push_back(unk10 >> 8);
        Data.push_back(unk10 & 0xFF);
        Data.push_back(unk12 >> 8);
        Data.push_back(unk12 & 0xFF);
        return Data;
    }
    //Convert char vector to Prop Data
    void Prop::InputData(std::vector<unsigned char> data)
    {
        position[0] = data[0] << 8|data[1];
        position[1] = data[2] << 8 | data[3];
        position[2] = data[4] << 8 | data[5];
        unk6_7 = data[6] << 1 | data[7] >> 7;
        unk6_1 = (data[7] & 0b01111110) >>1;
        unk6_0 = (data[7] & 0b00000001);
        ItemID = data[8] << 8 | data[9];
        unkA = data[0xA] << 8 | data[0xB];
        FlagOrRotation = data[0xC] << 1 | data[0xD] >> 7;
        unkC_0 = (data[0xD] & 0b01111111)<<0x16| data[0xE]<<0x8| data[0xF];
        unk10 = data[0x10] << 8|data[0x11];
        unk12 = data[0x12] << 8 | data[0x13];
    }
};