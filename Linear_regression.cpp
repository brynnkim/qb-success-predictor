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
    float b = 0.0;
    float m = 0.0;

    for (int i = 0; i < num_iterations; i++) {
        RegressionResult result = backpropagation(m, b, inputs, actual_outputs);
        b = result.intercept;
        m = result.slope;
    }

    return {m, b};
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
void run() {
    // Example: predicting NFL touchdowns (column 4) from college touchdowns (column 1)
    // Column indices: 0=Name, 1=college_avg_touchdowns, 2=college_avg_yards, 3=college_avg_interceptions
    //                 4=nfl_avg_touchdowns, 5=nfl_avg_yards, 6=nfl_avg_interceptions

    string filename = "merged_stats_reduced.csv";
    int input_column = 1;  // college_avg_touchdowns
    int output_column = 4; // nfl_avg_touchdowns

    cout << "Reading data from " << filename << "..." << endl;
    auto [x, y] = get_data(filename, input_column, output_column);

    if (x.empty() || y.empty()) {
        cerr << "Error: No data loaded." << endl;
        return;
    }

    int data_points = x.size();
    cout << "Loaded " << data_points << " data points." << endl;
    cout << "Running gradient descent..." << endl;

    RegressionResult result = gradient_descent_runner(x, y, 10000);
    float slope = result.slope;
    float intercept = result.intercept;

    float mse = cost_function(slope, intercept, x, y);
    float rmse = sqrt(mse);

    print_results(slope, intercept, rmse);

    // Example prediction
    cout << "\nExample prediction:" << endl;
    float sample_input = 25.0;
    float prediction = slope * sample_input + intercept;
    cout << "If college_avg_touchdowns = " << sample_input
         << ", predicted nfl_avg_touchdowns = " << prediction << endl;
}

int main() {
    run();
    return 0;
}