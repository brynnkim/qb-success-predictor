#include "knn.cpp"
// Include linear regression file here (Nirvan)

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std; 
#include <sstream>
#include <unordered_map>
#include <limits>
static vector<string> split_csv(const string& line) {
    vector<string> out;
    string cur;
    for (char c : line) {
        if (c == ',') { out.push_back(cur); cur.clear(); }
        else { cur.push_back(c); }
    }
    out.push_back(cur);
    return out;
}

static int to_int_or_zero(const string& s) {
    try {
        if (s.empty()) return 0;
        size_t idx = 0;
        long long v = stoll(s, &idx);
        if (idx != s.size()) return 0; 
        if (v > numeric_limits<int>::max()) return numeric_limits<int>::max();
        if (v < numeric_limits<int>::min()) return numeric_limits<int>::min();
        return static_cast<int>(v);
    } catch (...) {
        return 0;
    }
}

static const char* COL_C_TD   = "college_avg_touchdowns";
static const char* COL_C_YDS  = "college_avg_yards";
static const char* COL_C_INT  = "college_avg_interceptions";
static const char* COL_C_WINS = "college_avg_wins";

static const char* COL_N_TD   = "nfl_avg_touchdowns";
static const char* COL_N_YDS  = "nfl_avg_yards";
static const char* COL_N_INT  = "nfl_avg_interceptions";
static const char* COL_N_WINS = "nfl_avg_wins";

vector<Qb> loadQbCsv(const string& path) {
    vector<Qb> rows;
    ifstream fin(path);
    if (!fin) {
        cerr << "Error: cannot open " << path << "\n";
        return rows;
    }

    string header;
    if (!getline(fin, header)) {
        cerr << "Error: CSV is empty.\n";
        return rows;
    }
    auto heads = split_csv(header);
    unordered_map<string,int> col;
    for (int i = 0; i < (int)heads.size(); ++i) col[heads[i]] = i;

    auto get = [&](const vector<string>& v, const char* name) -> string {
        auto it = col.find(name);
        if (it == col.end()) return "";     
        int idx = it->second;
        if (idx < 0 || idx >= (int)v.size()) return "";
        return v[idx];
    };

    string line;
    while (getline(fin, line)) {
        if (line.empty()) continue;
        auto fields = split_csv(line);

        Qb q{};
        q.c_tds  = to_int_or_zero(get(fields, COL_C_TD));
        q.c_yds  = to_int_or_zero(get(fields, COL_C_YDS));
        q.c_ints = to_int_or_zero(get(fields, COL_C_INT));
        q.c_wins = to_int_or_zero(get(fields, COL_C_WINS));

        q.n_tds  = to_int_or_zero(get(fields, COL_N_TD));
        q.n_yds  = to_int_or_zero(get(fields, COL_N_YDS));
        q.n_ints = to_int_or_zero(get(fields, COL_N_INT));
        q.n_wins = to_int_or_zero(get(fields, COL_N_WINS));

        q.success = false;
        q.distance = 0.0;

        rows.push_back(q);
    }

    cout << "Loaded " << rows.size() << " rows from " << path << "\n";
    return rows;
}

int main () {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string path = "../footballdata/merged_stats_reduced.csv";
    cout << "Trying: " << path << "\n";

    vector<Qb> dataset = loadQbCsv(path);
    if (dataset.empty()) return 1;

    cout << "WELCOME TO QB SUCCESS PREDICTOR\n";
    cout << "======[MENU]======\n";
    cout << "1. Linear Regression Percentage\n";
    cout << "2. K-NN Percentage\n";

    string input;
    cin >> input;

    if (input == "1") {
        int user_td, user_yards, user_ints, user_wins;
        cout << "Enter your average touchdowns: ";     cin >> user_td;
        cout << "Enter your average yards: ";          cin >> user_yards;
        cout << "Enter your average interceptions: ";  cin >> user_ints;
        cout << "Enter your average wins: ";           cin >> user_wins;
        //linear regression
        cout << "[Linear Regression not implemented yet]\n";
    } 
    else if (input == "2") {
        int user_td, user_yards, user_ints, user_wins;
        cout << "Enter your average touchdowns: ";     cin >> user_td;
        cout << "Enter your average yards: ";          cin >> user_yards;
        cout << "Enter your average interceptions: ";  cin >> user_ints;
        cout << "Enter your average wins: ";           cin >> user_wins;

        vector<int> user = { user_td, user_yards, user_ints, user_wins };
        int k = 1000;
        double result = KNN(dataset, user, k);
        cout << "Estimated NFL success likelihood: " << result << "%\n";
    } 
    else {
        cout << "Invalid selection.\n";
    }

    return 0;
}