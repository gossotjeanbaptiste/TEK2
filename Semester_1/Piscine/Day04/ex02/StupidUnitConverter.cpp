/*
## EPITECH PROJECT, 2026
## Makefile
## File description:
## Compile the project z
*/

#include <iostream> 
#include <string>
#include <sstream>
#include <iomanip>

int main(void)
{
    std::string line;
    std::string scale;
    double temp;
    double result;

    while (std::getline(std::cin, line)) {
        std::stringstream ss(line);
        if (!(ss >> temp >> scale))
            continue;
        if (scale == "Celsius") {
            result = (temp * 9.0 / 5.0) + 32.0;
            scale = "Fahrenheit";
        } else if (scale == "Fahrenheit") {
            result = 5.0 / 9.0 * (temp - 32);
            scale = "Celsius";
        } else {
            continue;
        }
        std::cout << std::right << std::setw(16) << std::fixed 
        << std::setprecision(3) << result
        << std::setw(16) << scale << std::endl;
    }
    return 0;
}
