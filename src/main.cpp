#include ".../src/knn.cpp"
// Include linear regression file here (Nirvan)

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

int main () {
    // Parse data here
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    ifstream fin("footballdata/merged_stats_reduced.csv");
    // Right now, my K-NN function assumes all data is stored in a vector of Qb instances (struct in knn.cpp)
    // but let me know if you want to change the format - Brynn

    // Menu draft
    cout << "WELCOME TO QB SUCCESS PREDICTOR" << endl;
    cout << "======[MENU]======" << endl;
    cout << "1. Linear Regression Percentage" << endl;
    cout << "2. K-NN Percentage" << endl;
    // Or something like that

    string input;
    int user_td, user_yards, user_ints;
    cin >> input;
    if (input == "1") {
        cout << "Enter your average touchdowns: " << endl;
        cin >> user_td;
        cout << "Enter your average yards: " << endl;
        cin >> user_yards;
        cout << "Enter your average yards: " << endl;
        cin >> user_ints;
        // Call linear regression function here (Nirvan)
    } else if (input == "2") {
        cout << "Enter your average touchdowns: " << endl;
        cin >> user_td;
        cout << "Enter your average yards: " << endl;
        cin >> user_yards;
        cout << "Enter your average yards: " << endl;
        cin >> user_ints;
        // dataset = vector of Qb objects, user = vector of user's input data
        // k = number of similar neighborhood you want to loom at (1,000 at default)
        double result = KNN(dataset, user, k);
        cout << "Estimated NFL success likelihood: " << result << endl;
    } else {
        // Handle when user input is something else?
    }


    return 0;
}