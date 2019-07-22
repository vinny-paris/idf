#include "generation.hpp"
#include "../util/utils.hpp"
#include "../IDF/idf_parser.hpp"
#include <iostream>
#include <fstream>
#include <cassert>
#include <algorithm>
#include <sys/stat.h>

#ifdef _OPENMP
#include <omp.h>
#endif

#ifdef _WIN32
#define NL "\r\n"
#else
#define NL "\n"
#endif

void make_dir(std::string const & folder_path) {
#if defined(_WIN32)
    _mkdir(folder_path.c_str());
#else
    mkdir(folder_path.c_str(), 0777);
#endif
}

std::string read_from_file(std::string const &input_file_name) {
    std::ifstream idf_stream(input_file_name, std::ios::in | std::ios::ate);
    if (!idf_stream.is_open()) {
        std::cout << "Could not open file: " << input_file_name << std::endl;
    }
    std::ifstream::pos_type size = idf_stream.tellg();
    char *memblock = new char[(int) size + 1];
    idf_stream.seekg(0, std::ios::beg);
    idf_stream.read(memblock, size);
    memblock[size] = '\0';
    idf_stream.close();
    std::string input_file = memblock;
    delete[] memblock;
    return input_file;
}

void output_to_csv(std::string const &output_file_name, std::vector<std::vector<std::string> > output,
                   char delimiter = ',') {
    std::ofstream output_file(output_file_name);
    if (!output_file.is_open()) {
        std::cout << "Could not open file: " << output_file_name << std::endl;
    }
    for (auto const &row : output) {
        size_t const size = row.size();
        assert(size > 0);
        for (size_t i = 0; i < size - 1; ++i) {
            output_file << row[i] + delimiter;
        }
        output_file << row[size - 1] + NL;
    }
}

Parameters get_parameters(std::string const &param_file_name) {
    std::ifstream params(param_file_name);
    if (!params.is_open()) {
        std::cout << "Could not open file: " << param_file_name << std::endl;
    }
    Parameters parameters;
    std::string line;
    std::getline(params, line);
    while (std::getline(params, line)) {
        auto const param = splitString(line, ',');
        if (!param[8].empty()) {
            parameters.groups[param[8]].push_back(param);
        } else {
            parameters.individuals.push_back(param);
        }
    }
    return parameters;
}

void generate_output_mapping(Input const & user_input) {
    auto const params = get_parameters(user_input.param_file);
    std::vector<std::vector<std::string> > matrix_output;

    auto const full_idf = read_from_file(user_input.idf_path);

    IdfParser idfParser;
    auto parsed_idf = idfParser.decode(full_idf);

    int counter = 0;
    int column = 0;
    int single_leading = 0;
    char excel_col[4] = "";

    for (auto const group : params.groups) {
        column = counter % 26;
        single_leading = counter / 26;
        bool added_value = false;
        for (auto const &param : group.second) {
            auto idf_object = std::find_if(
                    parsed_idf.begin(), parsed_idf.end(),
                    [&param](std::vector<std::string> const &item) {
                        return item[0] == param[0] && (param[1].empty() || item[1] == param[1]);
                    }
            );
            if (idf_object == parsed_idf.end()) continue;
            if (!added_value) {
//                if (param[0] == "Schedule:Day:Interval") {
//                    std::string upper_name(param[1]);
//                    std::for_each(upper_name.begin(), upper_name.end(), ::toupper);
//                    if (upper_name.find("CLGSETP") != std::string::npos ||
//                        upper_name.find("HTGSETP") != std::string::npos) {
//                    }
//                }

//                if (single_leading > 1) {
//                    excel_col[0] = 'A' + single_leading - 1;
//                    excel_col[1] = 'A' + single_leading - 2;
//                    excel_col[2] = 'A' + column;
//                    excel_col[3] = '\0';
//                } else if (single_leading > 0) {
                if (single_leading > 0) {
                    excel_col[0] = 'A' + single_leading - 1;
                    excel_col[1] = 'A' + column;
                    excel_col[2] = '\0';
                    excel_col[3] = '\0';
                } else {
                    excel_col[0] = 'A' + column;
                    excel_col[1] = '\0';
                    excel_col[2] = '\0';
                    excel_col[3] = '\0';
                }

                matrix_output.emplace_back(
                        std::vector<std::string>{
                                excel_col,
                                param[0] + " " + param[1] + " " + param[2],
                                param[3]
                        }
                );
                added_value = true;
                counter++;
            }
        }
    }

    for (auto const param : params.individuals) {
        column = counter % 26;
        single_leading = counter / 26;
        auto idf_object = std::find_if(
                parsed_idf.begin(), parsed_idf.end(),
                [&param](std::vector<std::string> const &item) {
                    return item[0] == param[0] && (param[1].empty() || item[1] == param[1]);
                }
        );
        if (idf_object == parsed_idf.end()) continue;

//        if (single_leading > 1) {
//            excel_col[0] = 'A' + single_leading - 1;
//            excel_col[1] = 'A' + single_leading - 2;
//            excel_col[2] = 'A' + column;
//            excel_col[3] = '\0';
//        } else if (single_leading > 0) {
        if (single_leading > 0) {
            excel_col[0] = 'A' + single_leading - 1;
            excel_col[1] = 'A' + column;
            excel_col[2] = '\0';
            excel_col[3] = '\0';
        } else {
            excel_col[0] = 'A' + column;
            excel_col[1] = '\0';
            excel_col[2] = '\0';
            excel_col[3] = '\0';
        }

        matrix_output.emplace_back(
                std::vector<std::string>{
                        excel_col,
                        param[0] + " " + param[1] + " " + param[2],
                        param[3]
                }
        );
        counter++;
    }

    std::string output_file(
            user_input.output_directory
            + "mapped_" + user_input.idf_name + '_' + "output_matrix.csv"
    );

    output_to_csv(output_file, matrix_output);
}

void generate_input_output_file(Input const & user_input) {
    std::ifstream matrix(user_input.matrix_file);
    if (!matrix.is_open()) {
        std::cout << "Could not open file: " << user_input.matrix_file << std::endl;
    }
    std::vector<std::vector<int> > sensitivity_matrix;

    auto const params = get_parameters(user_input.param_file);

    std::string line;
//    std::getline(matrix, line); // python version doesn't have a header
    while (std::getline(matrix, line)) {
        sensitivity_matrix.emplace_back(splitStringToInt(line, ','));
    }
    matrix.close();

    std::vector<std::vector<std::string> > matrix_output;

    auto const full_idf = read_from_file(user_input.idf_path);
    size_t matrix_count = 0;

    IdfParser idfParser;
    auto parsed_idf = idfParser.decode(full_idf);

    size_t max_rows = sensitivity_matrix.size();

    for (auto const &row : sensitivity_matrix) {
        std::vector<std::string> output_values;
        int counter = 0; // from python this starts at 0, but previously it was 1

        for (auto const group : params.groups) {
            auto const matrix_value = row[counter];

            bool added_value = false;

            for (auto const &param : group.second) {
                auto idf_object = std::find_if(
                        parsed_idf.begin(), parsed_idf.end(),
                        [&param](std::vector<std::string> const &item) {
                            return item[0] == param[0] && (param[1].empty() || item[1] == param[1]);
                        }
                );
                if (idf_object == parsed_idf.end()) continue;

                std::string val;
                if (matrix_value == 1) {
                    val = param[5];
                } else if (matrix_value == -1) {
                    val = param[4];
                }
                // auto val = matrix_value == 1 ? param[5] : param[4];
                idf_object->at(std::stoul(param[2]) + 1) = val;
                if (!added_value) {
                    if (param[0] == "Schedule:Day:Interval") {
                        std::string upper_name(param[1]);
                        std::for_each(upper_name.begin(), upper_name.end(), ::toupper);
                        if (upper_name.find("CLGSETP") != std::string::npos ||
                            upper_name.find("HTGSETP") != std::string::npos) {
                            std::string val;
                            if (matrix_value == 1) {
                                val = "8";
                            } else if (matrix_value == -1) {
                                val = "3";
                            }
                            // val = matrix_value == 1 ? "8" : "3";
                        }
                    }
                    output_values.push_back(val);
                    added_value = true;
                }
            }
            counter++;
        }

        for (auto const param : params.individuals) {
            auto const matrix_value = row[counter];

            auto idf_object = std::find_if(
                    parsed_idf.begin(), parsed_idf.end(),
                    [&param](std::vector<std::string> const &item) {
                        return item[0] == param[0] && (param[1].empty() || item[1] == param[1]);
                    }
            );
            if (idf_object == parsed_idf.end()) continue;
            std::string val;
            if (matrix_value == 1) {
                val = param[5];
            } else if (matrix_value == -1) {
                val = param[4];
            }
            // auto const val = matrix_value == 1 ? param[5] : param[4];
            idf_object->at(std::stoul(param[2]) + 1) = val;
            output_values.push_back(val);
            counter++;
        }

        auto const new_idf = idfParser.encode(parsed_idf);

        // make_dir(user_input.output_directory + kPathSeparator);

        std::ofstream output_idf_file(
                user_input.output_directory
            + kPathSeparator +
            user_input.idf_name + '_' +
            std::to_string(matrix_count) + ".idf"
        );
        if ( ! output_idf_file.is_open() ) {
          std::cout << "Could not open file: "
                    << user_input.output_directory + kPathSeparator +
                            user_input.idf_name + '_' + std::to_string(matrix_count) + ".idf"
                    << std::endl;
        }
        output_idf_file << new_idf;
        output_idf_file.close();

        matrix_output.push_back(output_values);

        matrix_count++;
    }

    std::string output_file(
        user_input.output_directory
        + kPathSeparator +
        user_input.idf_name + '_' +
        "output_matrix.csv"
    );
    output_to_csv(output_file, matrix_output);
    matrix_output.clear();
}

void strip_and_sort_idf(Input const & user_input) {
    auto const full_idf = read_from_file(user_input.idf_path);
    IdfParser idfParser;
    auto parsed_idf = idfParser.decode(full_idf);

    std::sort(
            parsed_idf.begin(),
            parsed_idf.end(),
            [](const std::vector<std::string> &a, const std::vector<std::string> &b) -> bool {
                if (a.size() < 2) return false;
                if (a[0] > b[0]) return true;
                if (a[0] < b[0]) return false;
                if (a.size() < 3) return false;
                return a[1] > b[1];
            }
    );

    auto const new_idf = idfParser.encode(parsed_idf);

    std::ofstream output_file(user_input.output_directory + "stripped_" + user_input.idf_name + ".idf");
    if (!output_file.is_open()) {
        std::cout << "Could not open file: "
                  << user_input.output_directory + "stripped_" + user_input.idf_name + ".idf"
                  << std::endl;
    }
    output_file << new_idf;
    output_file.close();
}

void convert_and_strip_outputs(Input const & user_input) {
    auto const full_idf = read_from_file(user_input.idf_path);
    IdfParser idfParser;
    auto parsed_idf = idfParser.decode(full_idf);

    for (auto it = parsed_idf.begin(); it != parsed_idf.end();) {
        if (it->at(0).find("Output:") != std::string::npos ||
            it->at(0).find("OutputControl:") != std::string::npos) {
            it = parsed_idf.erase(it);
        } else {
            ++it;
        }
    }
    parsed_idf.push_back({"Output:Meter", "Electricity:Facility", "Monthly"});
    parsed_idf.push_back({"Output:Meter", "Gas:Facility", "Monthly"});

    auto const new_idf = idfParser.encode(parsed_idf);

    std::ofstream output_file(user_input.output_directory + "converted_" + user_input.idf_name + ".idf");
    if (!output_file.is_open()) {
        std::cout << "Could not open file: " << user_input.output_directory + "converted_" + user_input.idf_name + ".idf"
                  << std::endl;
    }
    output_file << new_idf;
    output_file.close();
}
