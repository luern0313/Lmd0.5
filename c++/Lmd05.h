#ifndef LMD05_H
#define LMD05_H

using namespace std;

#include <vector>
#include <string>

#define bits_transform_1(seed) rs[0] = ((rs[1] & rs[2]) | ((~rs[1]) & rs[3])) ^ rs[0] ^ seed
#define bits_transform_2(seed) rs[1] = (rs[2] | rs[0] & ~rs[3]) ^ rs[1] ^ seed
#define bits_transform_3(seed) rs[2] = rs[0] ^ rs[1] ^ rs[2] ^ rs[3] ^ seed
#define bits_transform_4(seed) rs[3] = (rs[1] ^ ~rs[3] ^ (~rs[0] | rs[2])) ^ seed

class Lmd05
{
public:
    Lmd05(int block_length);
    void set_byte_array(const char* byte_array, int size);
    string solve();

private:
    int rs[4];
    int cond;
    vector<unsigned char> byte_array;
    int block_length;

    void init_magic_num();
    int bits_transform(int seed, int output_bits, int input_bits);
    static int get_full_int(int integer, int length);
    static int loop_shift_left(int x, int n, int length);
    static int compose_char(int int1, int int2);
};

#endif
