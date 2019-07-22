#ifndef ENERGYPLUSPARSER_GENERATION_HPP
#define ENERGYPLUSPARSER_GENERATION_HPP

#include <string>
#include <vector>
#include <map>

const char kPathSeparator =
#ifdef _WIN32
        '\\';
#else
        '/';
#endif

#ifdef _WIN32
#define NL "\r\n"
#else
#define NL "\n"
#endif

using Individuals = std::vector<std::vector<std::string> >;
using Groups = std::map<std::string, std::vector<std::vector<std::string> > >;

struct Parameters {
    Individuals individuals;
    Groups groups;
};

struct Input {
    Input(std::string const &idf_path, std::string const &idf_name, std::string const &matrix_file,
          std::string const &param_file, std::string const &weather_file, std::string const &output_directory) :
            idf_path(idf_path),
            idf_name(idf_name),
            matrix_file(matrix_file),
            param_file(param_file),
            weather_file(weather_file),
            output_directory(output_directory){}

    std::string const idf_path;
    std::string const idf_name;
    std::string const matrix_file;
    std::string const param_file;
    std::string const weather_file;
    std::string const output_directory;
};

void make_dir(std::string const & folder_path);

std::string read_from_file(std::string const &input_file_name);

Parameters get_parameters(std::string const &param_file_name);

void generate_output_mapping(Input const & user_input);

void generate_input_output_file(Input const & user_input);

void strip_and_sort_idf(Input const & user_input);

void convert_and_strip_outputs(Input const & user_input);

#endif //ENERGYPLUSPARSER_GENERATION_HPP
