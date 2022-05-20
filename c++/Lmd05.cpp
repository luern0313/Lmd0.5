using namespace std;

#include "Lmd05.h"
#include "md5.hpp"
#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include <ctime>

const char char_set[65] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ#&";

Lmd05::Lmd05(int block_length) {
    // 8  16 24 32
    // 24 48 72 96
    assert(block_length % 24 == 0);
    assert(block_length >= 24);
    assert(block_length <= 96);

    this->block_length = block_length;

    init_magic_num();
}

void Lmd05::set_byte_array(const char* byte_array, int size) {
    this->byte_array = vector<unsigned char>(byte_array, byte_array + size);
    init_magic_num();
}

void Lmd05::init_magic_num() {
    this->rs[0] = 0xae3115a1;
    this->rs[1] = 0xca0ef573;
    this->rs[2] = 0x866b151a;
    this->rs[3] = 0x993da81e;
    this->cond = 0;
}

int Lmd05::bits_transform(int seed, int output_bits, int input_bits = 32) {
    int t_full = get_full_int(seed, input_bits);
    cond = (cond ^ t_full) + 1 & 3;

    if (cond == 0)
        return bits_transform_1(seed) & (1 << output_bits) - 1;
    else if (cond == 1)
        return bits_transform_2(seed) & (1 << output_bits) - 1;
    else if (cond == 2)
        return bits_transform_3(seed) & (1 << output_bits) - 1;
    else
        return bits_transform_4(seed) & (1 << output_bits) - 1;
}

int Lmd05::get_full_int(int integer, int length) {
    int f_int = 0;
    for (int i = 0; i < ceil(32 / length); i++) {
        f_int <<= length;
        f_int |= integer;
    }
    f_int &= 4294967295;
    return f_int;
}

int Lmd05::loop_shift_left(int x, int n, int length = 32) {
    if (length == 32)
        return ((x << n) | (x >> (length - n)));
    else
        return ((x << n) | (x >> (length - n))) & ((1 << length) - 1);
}

int Lmd05::compose_char(int int1, int int2) {
    int hex_num = int1 >> 4 & 3;
    hex_num <<= 2;
    hex_num ^= int1 & 15;
    hex_num <<= 2;
    hex_num ^= int1 >> 6 & 3;
    hex_num ^= int2 & 15;
    return hex_num;
}

string Lmd05::solve() {
    int block_length_half = int(block_length / 2);
    vector<int> vector_list;
    for (int i = 0; i < block_length_half; i++)
        vector_list.push_back(bits_transform(0, 8));
    int vector_offset = 0;

    int full_length = block_length - (byte_array.size() % block_length);
    int full = full_length;
    for (int i = 0; i < full_length; i++) {
        full = loop_shift_left(full, 1, 8);
        byte_array.push_back(full);
    }

    for (unsigned int offset_block = 0; offset_block < byte_array.size(); offset_block += block_length) {
        for (int i = 0; i < block_length; i++) {
            int index = (int(i / 2) + vector_offset) % block_length_half;
            int b = byte_array[offset_block + i];
            vector_list[index] ^= bits_transform((i << 8) | b, 8, 16);

            vector_offset += b;
            vector_offset %= block_length_half;
        }
    }

    string result_str_list;

    int random_result = rs[0] ^ rs[1] ^ rs[2] ^ rs[3];
    for (unsigned int i = 0; i < vector_list.size(); i++)
        vector_list[i] ^= random_result & 255;

    for (unsigned int i = 0; i < vector_list.size(); i += 3) {
        int index0 = (i + vector_offset) % block_length_half;
        int index1 = (index0 + 1) % block_length_half;
        int index2 = (index0 + 2) % block_length_half;
        result_str_list.insert(result_str_list.begin(), char_set[compose_char(vector_list[index0], vector_list[index1] >> 4)]);
        result_str_list.insert(result_str_list.begin(), char_set[compose_char(vector_list[index2], vector_list[index1])]);
    }

    return result_str_list;
}

int main()
{
    Lmd05 lmd05(24);
    string encrypt = "0123456789012345678901230123456789";
    lmd05.set_byte_array(encrypt.c_str(), encrypt.size());
    printf("%s\n", lmd05.solve().c_str());
    encrypt = "luern0313";
    lmd05.set_byte_array(encrypt.c_str(), encrypt.size());
    printf("%s\n", lmd05.solve().c_str());
}
