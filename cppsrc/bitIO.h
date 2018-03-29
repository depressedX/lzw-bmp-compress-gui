//
// Created by lph on 2018/3/13.
//

#ifndef LZW_BITIO_H
#define LZW_BITIO_H

#include <iostream>
#include <fstream>

using namespace std;

string display(unsigned char c) {
    string r;
    for (int i = 8; i > 0; --i) {
        int s = (c >> (i - 1)) & 1;
        r += s == 0 ? '0' : '1';
    }
    return r;
}

string display_m(unsigned char *c, int l) {
    string r;
    for (int i = 0; i < l; ++i) {
        r += display(c[i]) + " ";
    }
    return r;
}

class bitReader {
public:
    explicit bitReader(fstream &in);

    int readInt(int length);

    void close();

    bool eof();

//private:
    fstream *stream_client;
    unsigned char buffer = 0;
    unsigned int buffer_length = 0;

    void fill_buffer();
};

bitReader::bitReader(fstream &in) {
    this->stream_client = &in;
}

int bitReader::readInt(int length) {
    int res = 0;

    if (eof())
        return -1;


    int rest_length = length;
    while (rest_length > 0) {
        if (buffer_length > rest_length) {
//        缓冲区足够
            res = (res << rest_length) | (buffer >> (8 - rest_length));
            buffer = buffer << rest_length;
            buffer_length -= rest_length;
            rest_length = 0;
        } else {
//        缓冲区不够
            if (eof()) {
                return -1;
            }
            res = (res << buffer_length) | (buffer >> (8 - buffer_length));
            rest_length -= buffer_length;
            buffer_length = 0;;
            fill_buffer();
        }
    }
    return res;
}

void bitReader::fill_buffer() {
    int c = stream_client->get();
    if (c == -1 || stream_client->fail() || eof()) {
        return;
    }
    buffer_length = 8;
    buffer = (unsigned char) c;
}


void bitReader::close() {
    stream_client->close();
}

bool bitReader::eof() {
    return stream_client->eof();
}


class bitWritter {
public:
    explicit bitWritter(fstream &out);

    void writeInt(unsigned int v, int length);

    void close();


private:
    fstream *stream_client;
//    凑满一字节后输出
    unsigned char buffer = 0;
    unsigned int buffer_length = 0;

    void flush();
};

bitWritter::bitWritter(fstream &out) {
    stream_client = &out;

}

void bitWritter::writeInt(unsigned int v, int length) {
//    从高位输出
    for (int i = length; i >= 1; --i) {
        int sig = ((v >> (i - 1)) & 1);
        int ss;
        if (sig == 0) {
            ss = (1 << (7 - buffer_length)) ^ 0xff;
            buffer = (unsigned char) (buffer & ss);
        } else {
            ss = 1 << (7 - buffer_length);
            buffer = (unsigned char) (buffer | ss);
        }
        buffer_length++;
        if (buffer_length >= 8) flush();
    }
}

void bitWritter::flush() {
    if (buffer_length == 0)return;
    if (buffer_length > 8) {
        cerr << "find a bug" << endl;
    }

    if (buffer_length < 8) {
//        低位填充0
        buffer = static_cast<unsigned char>(buffer & (((1 << buffer_length) - 1) << (8 - buffer_length)));
    }
    stream_client->write((char *) &buffer, 1);
    buffer_length = 0;
}

void bitWritter::close() {
    flush();
    stream_client->close();
}


#endif //LZW_BITIO_H
