#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>
#include <vector>
#include <iomanip>
#include <ctime>
#include <cctype>
using namespace std;

vector<string> split_csv(const string& line) {
    vector<string> out;
    string cur;
    for (char c : line) {
        if (c == ',') { out.push_back(cur); cur.clear(); }
        else if (c != '\r') { cur.push_back(c); }
    }
    out.push_back(cur);
    return out;
}

int random_int(int min, int max) {
    return min + rand() % (max - min + 1);
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    const string filename = "../footballdata/merged_stats_reduced_with_synth.csv";
    ifstream in(filename);
    vector<vector<string>> rows;
    string line;
    while (getline(in, line)) rows.push_back(split_csv(line));
    in.close();
    srand(static_cast<unsigned int>(time(nullptr)));

    for (size_t i = 1; i < rows.size(); ++i) {
        ostringstream ss;
        ss << fixed << setprecision(3) << (random_int(5, 45) + (rand() % 1000) / 1000.0);
        rows[i][4] = ss.str();
        ss.str(""); ss.clear();
        ss << fixed << setprecision(3) << (random_int(800, 5200) + (rand() % 1000) / 1000.0);
        rows[i][5] = ss.str();
        ss.str(""); ss.clear();
        ss << fixed << setprecision(3) << (random_int(0, 25) + (rand() % 1000) / 1000.0);
        rows[i][6] = ss.str();
    }

    ofstream out(filename, ios::trunc);
    for (const auto& row : rows) {
        for (size_t i = 0; i < 7; ++i) {
            if (i) out << ",";
            if (i < row.size()) out << row[i];
        }
        out << "\n";
    }
    out.close();
    return 0;
}
