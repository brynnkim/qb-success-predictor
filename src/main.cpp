#include "knn.cpp"
#include "Linear_regression.cpp"

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
        long long v = llround(stod(s));
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

static const char* COL_N_TD   = "nfl_avg_touchdowns";
static const char* COL_N_YDS  = "nfl_avg_yards";
static const char* COL_N_INT  = "nfl_avg_interceptions";

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

        q.n_tds  = to_int_or_zero(get(fields, COL_N_TD));
        q.n_yds  = to_int_or_zero(get(fields, COL_N_YDS));
        q.n_ints = to_int_or_zero(get(fields, COL_N_INT));

        q.success = false;
        q.distance = 0.0;

        rows.push_back(q);
    }

    cout << "Loaded " << rows.size() << " rows from " << path << "\n";
    return rows;
}

void display_menu() {
    string path = "../footballdata/merged_stats_reduced_with_synth.csv";
    cout << "Trying: " << path << "\n";

    vector<Qb> dataset = loadQbCsv(path);
    if (dataset.empty()) return;

    cout << "WELCOME TO QB SUCCESS PREDICTOR\n";
    cout << "============[MENU]============\n";
    cout << "1. Linear Regression Percentage\n";
    cout << "2. K-NN Percentage\n";
    cout << "3. Exit\n";
    string input;
    cin >> input;

    if (input == "1") {
        float user_td, user_yards, user_ints;
        cout << "Enter your average touchdowns: ";     cin >> user_td;
        cout << "Enter your average yards: ";          cin >> user_yards;
        cout << "Enter your average interceptions: ";  cin >> user_ints;
        cout << "\n";
        vector<float> user = { user_td, user_yards, user_ints };
        run(user);
        cout << "\n";
        display_menu();
    }
    else if (input == "2") {
        int user_td, user_yards, user_ints;
        cout << "Enter your average touchdowns: ";     cin >> user_td;
        cout << "Enter your average yards: ";          cin >> user_yards;
        cout << "Enter your average interceptions: ";  cin >> user_ints;
        cout << "\n";

        vector<int> user = { user_td, user_yards, user_ints };
        int k = 100;
        double result = KNN(dataset, user, k);
        cout.setf(ios::fixed);
        cout.precision(2);
        cout << "Estimated NFL success likelihood: " << result << "%\n";
        cout << "\n";
        display_menu();
    }
    else if (input == "3") {
        return;
    } else {
        cout << "Invalid selection.\n";
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    display_menu();
    return 0;
}