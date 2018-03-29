#include <iostream>
#include <fstream>
#include <map>
#include <sstream>
#include "bitIO.h"

using namespace std;

/*定义字典的初始大小*/
const unsigned DICT_SIZE = 256;
/*定义字典的起始编号*/
const unsigned CODE_START = 256;

/*定义压缩码的最大长度*/
const int MAX_CODE_LEN = 14;
/*定义压缩码的最小长度*/
const int MIN_CODE_LEN = 9;
/*定义清空标记*/
const int SIG_CLEAR = (1 << MAX_CODE_LEN) - 1;

string split_file_directory(const std::string &str) {
    std::size_t found = str.find_last_of("/\\");
    if (found == -1) {
        return "";
    }
    return str.substr(0, found);
}

string split_file_name(const std::string &str) {
    std::size_t found = str.find_last_of("/\\");
    string s1 = str.substr(found + 1);
    std::size_t dot = s1.find_last_of('.');
    if (dot != -1) {
        s1 = s1.substr(0, dot);
    }
    return s1;
}

string split_file_ext(const std::string &str) {
    std::size_t found = str.find_last_of("/\\");
    string s1 = str.substr(found + 1);
    std::size_t dot = s1.find_last_of('.');
    if (dot != -1) {
        s1 = s1.substr(dot);
    }
    return s1;
}

string translate(string c) {
    string res;
    for (int i = 0; i < c.length(); ++i) {
        res += (int) c[i] + " ";
    }
    return res;
}

/***
 * LZW压缩
 * @param in 已经打开的流
 * @param out 已经打开的流
 */
void encode_with_LZW(fstream &in, fstream &out) {
    bitWritter writter(out);
    map<string, unsigned> dict;
//    初始化字典默认项
    for (unsigned i = 0; i < DICT_SIZE; ++i) {
        dict[string(1, i)] = i;
    }

    unsigned int cur_code_len = MIN_CODE_LEN;
    unsigned int cur_code = CODE_START;

    bool debug_reset = false;

    int c = 0;
    string p;
    while (true) {
        c = in.get();
        if (in.eof()) {
            if (!p.empty()) {
                writter.writeInt(dict[p], cur_code_len);
            }
            break;
        }

        if (dict.find(p + (char) c) != dict.end()) {
//            找到了
            p += (char) c;
        } else {
//            没找到
            writter.writeInt(dict[p], cur_code_len);
            dict[p + (char) c] = cur_code++;

            if (cur_code == SIG_CLEAR) {
                debug_reset = true;
                p = (char) c;
                if (!p.empty()) {
                    writter.writeInt(dict[p], cur_code_len);
                    p.clear();
                }
//                重置标记
                writter.writeInt(SIG_CLEAR, MAX_CODE_LEN);

                dict.clear();
//                初始化字典默认项
                for (unsigned i = 0; i < DICT_SIZE; ++i) {
                    dict[string(1, i)] = i;
                }

                cur_code_len = MIN_CODE_LEN;
                cur_code = CODE_START;

                continue;

            }
            if (cur_code >= (1 << cur_code_len) - 1) {
//                需要增加编码长度

//                添加增加编码长度标记
                writter.writeInt((unsigned) (1 << cur_code_len) - 1, cur_code_len);
                cur_code_len++;
            }
            p = char(c);
        }
    }
    writter.close();
}


/***
 * LZW解压缩
 * @param in 已经打开的流
 * @param out 已经打开的流
 */
void decode_with_LZW(fstream &in, fstream &out) {
    bitReader reader(in);
    map<unsigned, string> dict;
//    初始化字典默认项
    for (unsigned i = 0; i < CODE_START; ++i) {
        dict[i] = string(1, i);
    }

    if (reader.eof()) {
        return;
    }
    unsigned int cur_code = CODE_START;
    unsigned int cur_code_length = MIN_CODE_LEN;

    int cW;
    int pW;


    cW = reader.readInt(cur_code_length);
    out.write(dict[cW].c_str(), 1);

    int debug_i = 0;
    while (!reader.eof()) {
        debug_i++;
        pW = cW;

        cW = reader.readInt(cur_code_length);
        if (cW == -1) break;
//        判断是否需要重置
        if (cW == SIG_CLEAR) {
            dict.clear();
            for (unsigned i = 0; i < CODE_START; ++i) {
                dict[i] = string(1, i);
            }
            cur_code = CODE_START;
            cur_code_length = MIN_CODE_LEN;

//            重新读入
            cW = reader.readInt(cur_code_length);
            if (cW == -1) break;
            out.write(dict[cW].c_str(), 1);

            continue;

        }

//        判断是否需要增加位数
        if (cW == (1 << cur_code_length) - 1) {
            cur_code_length++;

//            重新读入
            cW = reader.readInt(cur_code_length);
            if (cW == -1) break;
        }


        if (dict.find(cW) != dict.end()) {
            string tmp = dict[cW];
            out.write(tmp.c_str(), tmp.length());
            dict[cur_code++] = dict[pW] + dict[cW][0];
        } else {
            dict[cur_code++] = dict[pW] + dict[pW][0];
            string tmp = dict[pW] + dict[pW][0];
            out.write(tmp.c_str(), tmp.length());
        }
    }
}

int main(int argc, char *argv[]) {

//    参数错误
    if (argc <= 2) {
        cout << "please input file path" << endl;
        return 1;
    }

//    获取命令
    string command = argv[1];

//    文件完整路径
    string file_path = argv[2];
//    目录
    string file_directory = split_file_directory(file_path);
//    文件名
    string file_name = split_file_name(file_path);
//    拓展名
    string input_ext = split_file_ext(file_path);

    string output_path;


//    编辑输出路径
    if (argc >= 4)
        output_path = argv[3];
    else {
        if (command == "zip") {
            output_path = file_path + ".lzw";
        } else if (command == "unzip") {
            if (input_ext == ".lzw") {
                output_path = file_directory + file_name;
            } else {
                output_path = file_directory + file_name + ".raw";
            }
        } else {
            cout << "wrong command!" << endl;
        }
    }

//    打开输入文件流
    fstream input_stream(file_path, ios::in | ios::binary);
    if (!input_stream.is_open()) {
        cout << "can't open in file" << endl;
        return 1;
    }

//    打开输出文件流
    fstream output_stream(output_path, ios::out | ios::binary);
    if (!output_stream.is_open()) {
        cout << "can't open out file" << endl;
        return 1;
    }

    if (command == "zip") {
        encode_with_LZW(input_stream, output_stream);
    } else if (command == "unzip") {
        decode_with_LZW(input_stream, output_stream);
    }


    return 0;
}