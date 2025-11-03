<H1> College Quarterback Success Predictor </h1>
College Quarterback Success Predictor is a C++ application that estimates the success likelihood of a college quarterback in the NFL.

<h2> Installation </h2>
Clone our GitHub repository:

```bash
git clone https://github.com/brynnkim/qb-success-predictor
```
<h2> Usage </h2>
Select from the following menu:

```c++
WELCOME TO QB SUCCESS PREDICTOR
============[MENU]============
1. Linear Regression Percentage
2. K-NN Percentage
3. Exit
```
1. Linear Regression example:
```c++
1
Enter your average touchdowns: 32
Enter your average yards: 4500
Enter your average interceptions: 3

Reading data from ../footballdata/merged_stats_reduced_with_synth.csv...
Loaded 74202 data points.
Running gradient descent...
RMSE: 11.8855
Regression equation: y = 0.000634705x + 25.5054
Predicted NFL average touchdowns: 25.5257
Reading data from ../footballdata/merged_stats_reduced_with_synth.csv...
Loaded 74202 data points.
Running gradient descent...
RMSE: 1460.8
Regression equation: y = 0.209719x + 2287.52
Predicted NFL average yards: 3231.26
Reading data from ../footballdata/merged_stats_reduced_with_synth.csv...
Loaded 74202 data points.
Running gradient descent...
RMSE: 7.50334
Regression equation: y = 9.21329e-05x + 12.9784
Predicted NFL average interceptions: 12.9786
```

2. K-NN example:

```c++
2
Enter your average touchdowns: 32
Enter your average yards: 4500
Enter your average interceptions: 3

Estimated NFL success likelihood: 48.00%
```

<h2> Contributing </h2>
Pull requests are welcome. For major changes, please open an issue first
to discuss what you would like to change.

<h2> License </h2>

<details>
<summary>
Copyright (c) 2025 Brynn Kim, Sharelene Kho, Nirvan Shrestha </summary>
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
</details>
