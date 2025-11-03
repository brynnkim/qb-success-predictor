#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <iomanip>

using namespace std;

// Structure to hold the gradient descent result
struct RegressionResult {
    float slope;
    float intercept;
};

// Calculate the cost function (Mean Squared Error)
float cost_function(float m, float b, vector<float>& inputs, vector<float>& actual_outputs) {
    float error_percentage = 0.0;
    int N = inputs.size();

    if (N == 0) return 0.0f;

    for (int i = 0; i < N; i++) {
        float prediction = m * inputs[i] + b;
        float error = actual_outputs[i] - prediction;
        error_percentage += error * error;
    }

    error_percentage /= N;
    return error_percentage;
}

// Perform backpropagation and return updated parameters
RegressionResult backpropagation(float m, float b, vector<float>& inputs, vector<float>& actual_outputs) {
    float learning_rate = 0.001;
    float b_gradient = 0.0;
    float m_gradient = 0.0;
    int N = inputs.size();

    if (N == 0) return {m, b};

    for (int i = 0; i < N; i++) {
        float prediction = m * inputs[i] + b;
        float error = actual_outputs[i] - prediction;

        b_gradient += -(2.0 / N) * error;
        m_gradient += -(2.0 / N) * inputs[i] * error;
    }

    float new_b = b - (learning_rate * b_gradient);
    float new_m = m - (learning_rate * m_gradient);

    return {new_m, new_b};
}

// Run gradient descent
RegressionResult gradient_descent_runner(vector<float>& inputs, vector<float>& actual_outputs, int num_iterations) {
    float b = 0.0f;
    float m = 0.0f;

    for (float& n : inputs) n /= 1000.0f;

    for (int i = 0; i < num_iterations; i++) {
        RegressionResult result = backpropagation(m, b, inputs, actual_outputs);
        b = result.intercept;
        m = result.slope;
    }

    return {m / 1000.0f, b};
}

// Read CSV file and extract two columns by index
pair<vector<float>, vector<float>> get_data(string& file_name, int input_col_index, int output_col_index) {
    vector<float> x;
    vector<float> y;

    ifstream file(file_name);
    if (!file.is_open()) {
        cerr << "Error: Could not open file " << file_name << endl;
        return {x, y};
    }

    string line;
    bool first_line = true;

    while (getline(file, line)) {
        if (first_line) {
            first_line = false;
            continue; // Skip header
        }

        stringstream ss(line);
        string cell;
        vector<string> row;

        while (getline(ss, cell, ',')) {
            row.push_back(cell);
        }

        int row_size = row.size();
        int max_index = max(input_col_index, output_col_index);

        if (row_size > max_index) {
            try {
                float input_val = stof(row[input_col_index]);
                float output_val = stof(row[output_col_index]);
                x.push_back(input_val);
                y.push_back(output_val);
            } catch (const exception& e) {
                // Skip rows with invalid data
                continue;
            }
        }
    }

    file.close();
    return {x, y};
}

// Print the results
void print_results(float slope, float intercept, float rmse) {
    cout << "RMSE: " << rmse << endl;
    cout << "Regression equation: y = " << slope << "x + " << intercept << endl;
}

// Main function
void run(vector<float>& user) {
    // Example: predicting NFL touchdowns (column 4) from college touchdowns (column 1)
    // Column indices: 0=Name, 1=college_avg_touchdowns, 2=college_avg_yards, 3=college_avg_interceptions
    //                 4=nfl_avg_touchdowns, 5=nfl_avg_yards, 6=nfl_avg_interceptions

    string filename = "../footballdata/merged_stats_reduced_with_synth.csv";
    int input_column_td = 1;  // college_avg_touchdowns
    int output_column_td = 4; // nfl_avg_touchdowns
    cout << "Reading data from " << filename << "..." << endl;
    auto [x_td, y_td] = get_data(filename, input_column_td, output_column_td);
    if (x_td.empty() || y_td.empty()) {
        cerr << "Error: No data loaded." << endl;
        return;
    }
    cout << "Loaded " << x_td.size() << " data points." << endl;
    cout << "Running gradient descent..." << endl;
    RegressionResult result_td = gradient_descent_runner(x_td, y_td, 10000);
    float slope_td = result_td.slope;
    float intercept_td = result_td.intercept;
    float mse_td = cost_function(slope_td, intercept_td, x_td, y_td);
    float rmse_td = sqrt(mse_td);
    print_results(slope_td, intercept_td, rmse_td);
    float prediction_td = slope_td * user[0] + intercept_td;
    cout << "Predicted NFL average touchdowns: " << prediction_td << endl;

    int input_column_yd = 2;  // college_avg_yards
    int output_column_yd = 5; // nfl_avg_yards
    cout << "Reading data from " << filename << "..." << endl;
    auto [x_yd, y_yd] = get_data(filename, input_column_yd, output_column_yd);
    if (x_yd.empty() || y_yd.empty()) {
        cerr << "Error: No data loaded." << endl;
        return;
    }
    cout << "Loaded " << x_yd.size() << " data points." << endl;
    cout << "Running gradient descent..." << endl;
    RegressionResult result_yd = gradient_descent_runner(x_yd, y_yd, 10000);
    float slope_yd = result_yd.slope;
    float intercept_yd = result_yd.intercept;
    float mse_yd = cost_function(slope_yd, intercept_yd, x_yd, y_yd);
    float rmse_yd = sqrt(mse_yd);
    print_results(slope_yd, intercept_yd, rmse_yd);
    float prediction_yd = slope_yd * user[1] + intercept_yd;
    cout << "Predicted NFL average yards: " << prediction_yd << endl;

    int input_column_int = 3;  // college_avg_interceptions
    int output_column_int = 6; // nfl_avg_interceptions
    cout << "Reading data from " << filename << "..." << endl;
    auto [x_int, y_int] = get_data(filename, input_column_int, output_column_int);
    if (x_int.empty() || y_int.empty()) {
        cerr << "Error: No data loaded." << endl;
        return;
    }
    cout << "Loaded " << x_int.size() << " data points." << endl;
    cout << "Running gradient descent..." << endl;
    RegressionResult result_int = gradient_descent_runner(x_int, y_int, 10000);
    float slope_int = result_int.slope;
    float intercept_int = result_int.intercept;
    float mse_int = cost_function(slope_int, intercept_int, x_int, y_int);
    float rmse_int = sqrt(mse_int);
    print_results(slope_int, intercept_int, rmse_int);
    float prediction_int = slope_int * user[2] + intercept_int;
    cout << "Predicted NFL average interceptions: " << prediction_int << endl;
}