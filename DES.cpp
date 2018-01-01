/**********************************************************************************************************/
/*                                   Artificial Nerual Network Algorithm Develop Kit                                      */
/*                                                       CelestialTS Tech Inc.                                                            */
/*                                                    Copyright © 2015-2017                                                          */
/*                                       For more check : www.tianshicangxie.com                                          */
/**********************************************************************************************************/
#include "DES.h"

SymmtricKey::DES::DES()
{

}

FuncStat SymmtricKey::DES::Set_Key(string _str)
{
    this->MainKey = String2Key(_str);
    return OK;
}

void SymmtricKey::DES::Debug(void)
{
    Subkey_Generate();
    for (size_t i = 0; i < 16; i++)
    {
        std::cout << SubKeys[i] << std::endl;
    }
}

bitset<64> SymmtricKey::DES::Encode(bitset<64> _data)
{
    bitset<64> encode;
    bitset<64> currentcode;
    bitset<32> leftcode;
    bitset<32> rightcode;
    bitset<32> temp_leftcode;
    // IP
    currentcode = Initial_Permutation(_data);
    
    for (int i = 0; i < 32; i++)
        leftcode[i] = currentcode[i];
    for (int i = 32; i < 64; i++)
        rightcode[i - 32] = currentcode[i];
    //16 rounds
    for (int round = 0; round < 16; ++round)
    {
        temp_leftcode = rightcode;
        rightcode = leftcode ^ F(rightcode, SubKeys[round]);
        leftcode = temp_leftcode;
    }
    
    for (int i = 0; i < 32; i++)
        currentcode[i] = rightcode[i];
    for (int i = 32; i < 64; i++)
        currentcode[i] = leftcode[i - 32];
    // IP-1
    encode = Final_Permutation(currentcode);
    return encode;
}

bitset<64> SymmtricKey::DES::Decode(bitset<64> _data)
{
    bitset<64> decode;
    bitset<64> currentcode;
    bitset<32> leftcode;
    bitset<32> rightcode;
    bitset<32> temp_leftcode;
    // IP
    currentcode = Initial_Permutation(_data);

    for (int i = 0; i < 32; i++)
        leftcode[i] = currentcode[i];
    for (int i = 32; i < 64; i++)
        rightcode[i - 32] = currentcode[i];
    // 16 rounds
    for (int round = 0; round < 16; ++round)
    {
        temp_leftcode = rightcode;
        rightcode = leftcode ^ F(rightcode, SubKeys[15 - round]);
        leftcode = temp_leftcode;
    }

    for (int i = 0; i < 32; i++)
        currentcode[i] = rightcode[i];
    for (int i = 32; i < 64; i++)
        currentcode[i] = leftcode[i - 32];
    // IP-1  
    decode = Final_Permutation(currentcode);
    return decode;
}

bitset<64> SymmtricKey::DES::Initial_Permutation(bitset<64> _data)
{
    bitset<64> temp;
    for (int i = 0; i < 64; i++)
        temp[i] = _data[IP_Table[i] - 1];
    return temp;
}

bitset<64> SymmtricKey::DES::Final_Permutation(bitset<64> _data)
{
    bitset<64> temp;
    for (int i = 0; i < 64; i++)
        temp[i] = _data[FP_Table[i] - 1];
    return temp;
}

FuncStat SymmtricKey::DES::Subkey_Generate(void)
{
    bitset<56> realKey;
    for (unsigned i = 0; i < 56; i++)
        realKey[i] = MainKey[PC1_Table[i] - 1];

    bitset<28> leftpart, rightpart;
    bitset<56> compressedKey;

    // the left part(28-bit) of the real key(56-bit)
    for (int i = 0; i < 28; i++)
        leftpart[i] = realKey[i];
    // the right part(28-bit) of the real key(56-bit)
    for (int i = 0; i < 28; i++)
        rightpart[i] = realKey[i + 28];

    for (size_t round = 0; round < 16; round++)
    {
        // do the left shift stuff
        leftpart = LeftShift(leftpart, LS_Table[round]);
        rightpart = LeftShift(rightpart, LS_Table[round]);

        // combine left and right part
        for (size_t i = 0; i < 28; i++)
            compressedKey[i] = leftpart[i];
        for (size_t i = 0; i < 28; i++)
            compressedKey[i + 28] = rightpart[i];

        // get subkeys
        for (size_t i = 0; i < 48; i++)
            SubKeys[round][i] = compressedKey[PC2_Table[i] - 1];
    }
    return OK;
}

bitset<32> SymmtricKey::DES::F(bitset<32> _data, SubKey _subkey)
{
    bitset<48> extended_data;
    // extending 32-bit data 
    for (int i = 0; i < 48; i++)
        extended_data[i] = _data[E_Table[i] - 1];
    // XOR 
    extended_data ^= _subkey;
    // S boxing
    bitset<32> temp;
    unsigned index = 0;
    for (int i = 0; i < 48; i += 6)
    {
        unsigned row = extended_data[i] * 2 + extended_data[i + 5];
        unsigned col = extended_data[i + 1] * 8 + extended_data[i + 2] * 4 + extended_data[i + 3] * 2 + extended_data[i + 4];
        unsigned num = S_BOX[i / 6][row][col];
        bitset<4> binary(num);
        for (int i = 0; i < 4; i++)
            temp[index + i] = binary[i];
        index += 4;
    }
    // P 
    bitset<32> output;
    for (int i = 0; i < 32; i++)
        output[i] = temp[P_Table[i] - 1];
    return output;
}

Key SymmtricKey::DES::String2Key(string _str)
{
    string tempstr;
    for (size_t i = 0; i != _str.length(); ++i)
        tempstr += Hex2Bin(_str[i]);
    return Key (tempstr);
}

bitset<28> SymmtricKey::DES::LeftShift(bitset<28> _key, unsigned _index)
{
    bitset<28> temp = _key;
    for (int i = 0; i < 28; i++)
    {
        if (i + _index < 28)
            temp[i] = _key[i + _index];
        else
            temp[i] = _key[i + _index - 28];
    }
    return temp;
}

const string SymmtricKey::DES::Hex2Bin(char _c) const
{
    switch (toupper(_c))
    {
    case '0': return "0000";
    case '1': return "0001";
    case '2': return "0010";
    case '3': return "0011";
    case '4': return "0100";
    case '5': return "0101";
    case '6': return "0110";
    case '7': return "0111";
    case '8': return "1000";
    case '9': return "1001";
    case 'A': return "1010";
    case 'B': return "1011";
    case 'C': return "1100";
    case 'D': return "1101";
    case 'E': return "1110";
    case 'F': return "1111";
    default: return "0000";
    }
}