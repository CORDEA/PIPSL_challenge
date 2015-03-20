/*
 * encoding: utf-8
 *
 * Copyright [2015] [Yoshihiro Tanaka]
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Author: Yoshihiro Tanaka <contact@cordea.jp>
 * date  : 2015-03-13
 */

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <regex>
#include <algorithm>

using namespace std;

string readFile(const string& filename) {
    ifstream    file(filename);
    string      line, seq;

    if (file.is_open()) {
        while (getline(file, line)) if (line[0] != '>') seq += line;
        file.close();
    }
    return seq;
}

vector<int> checkBlocks(string& seq, const string& query, const int& block=50) {
    const   string      seq_bu      = seq;
    const   int         ned_hits    = query.length() / block;
            vector<int> index_arr;

    for (int i=0; i <= query.length() - block; i++) {
        int count = 0;

        while (true) {
            if (i > query.length()) {
                seq = seq_bu;
                break;
            }
            int index = seq.find(query.substr(i, block));

            if (index != string::npos and seq.length() > index + block + 1) {
                seq = seq.substr(index + block);
                index_arr.push_back(index);
                ++count;
            } else {
                seq = seq_bu;
                break;
            }
        }
        if (count != 0) i += block - 1;
    }

    cout << index_arr.size() << "/" << ned_hits << " blocks" << endl;
    return index_arr;
}

void drawhist(const int& seq_len, vector<int>& index_arr, const int range[]) {
    const   int block   = 10;
            int section = seq_len / block;
    
    for (int k=0; k <= 2; k++) {
        int                     start_p = 0;
        vector<int>::iterator   max;

        cout << endl;
        if (k == 1) {
            vector<int>::iterator   min = min_element(index_arr.begin(), index_arr.end());
                                    max = max_element(index_arr.begin(), index_arr.end());

            section = (*max - *min) / block;
            start_p = *min;
        } else if (k == 2) {
            if (range[0] + range[1] == 0) break;
            section = (range[1] - range[0]) / block;
            start_p = range[0];
        }

        for (int i=0; i < block; i++) {
            string  bar;
            int     end_p   = start_p - 1;
            if (i == block - 1) end_p = start_p;

            int     bot     = section * i       + start_p;
            int     top     = section * (i + 1) + end_p;
            if (i == block - 1) {
                if      (k == 1) top = *max;
                else if (k == 2) top = range[1];
            }

            for (int c : index_arr) if (bot <= c and c <= top) bar += "|";
            bar.resize(index_arr.size(), ' ');
            cout << bar << " :"
                << section * i + start_p << "-"
                << section * (i + 1) + end_p << endl;
        }
    }
}

int main(int argc, char* argv[]) {
    // 1: original FASTA
    // 2: query FASTA
    // 3: read blocks
    string      seq         = readFile(argv[1]);
    string      query       = readFile(argv[2]);
    int         block       = atoi(argv[3]);
    if (block > query.length()) {
        cout << block << " >> " << query.length() << endl;
        block = query.length();
    }

    vector<int> index_arr   = checkBlocks(seq, query, block);
    int         range[]     = {0, 0};
    if (argc > 5) for (int i=0; i < 2; i++) range[i] = atoi(argv[4+i]);
    if (index_arr.size() != 0) drawhist(seq.length(), index_arr, range);
    return 0;
}
