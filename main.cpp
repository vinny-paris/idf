#include <iostream>
#include "ezOptionParser.hpp"
#include "generation/generation.hpp"

#ifdef _OPENMP
#include <omp.h>
#endif

int main(int argc, const char *argv[]) {
    ez::ezOptionParser opt;

    opt.overview = "Run AutoBEM design of experiments tool";
    opt.syntax = "design_of_experiments [OPTIONS]";
    opt.example = "design_of_experiments -i in.idf -o outputs/ -m matrix.csv -p all_inputs.csv\n\n";
    opt.footer = "ORNL (C) 2019\n";

    opt.add(
            "", // Default.
            1, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "Input IDF file.", // Help description.
            "-i",     // Flag token.
            "--input",  // Flag token.
            "--idf" // Flag token.
    );

    opt.add(
            "generated_files/", // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "Output folder for completed files.", // Help description.
            "-o",     // Flag token.
            "--output"  // Flag token.
    );

    opt.add(
            "", // Default.
            1, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "Sensitivity matrix file.", // Help description.
            "-m",     // Flag token.
            "--matrix"  // Flag token.
    );

    opt.add(
            "", // Default.
            1, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "Input parameters building type.", // Help description.
            "-p",     // Flag token.
            "--params"  // Flag token.
    );

    opt.add(
			"2", // Default.
			false, // Required?
			1, // Number of args expected.
			0, // Delimiter if expecting multiple args.
			"Number of threads for OpenMP", // Help description.
			"-j"     // Flag token.
	);

    opt.add(
            "", // Default.
            0, // Required?
            0, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "Display usage instructions.", // Help description.
            "-h",     // Flag token.
            "-help",  // Flag token.
            "--help", // Flag token.
            "--usage" // Flag token.
    );

    opt.add(
            "", // Default.
            0, // Required?
            0, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "Print all inputs and categories for debugging.", // Help description.
            "--debug"     // Flag token.
    );

    opt.parse(argc, argv);

    if (opt.isSet("-h")) {
        std::string usage;
        opt.getUsage(usage);
        std::cout << usage;
        return 1;
    }

    if (opt.isSet("--debug")) {
        std::string pretty;
        opt.prettyPrint(pretty);
        std::cout << pretty;
    }

    std::string idf_path;
    if (opt.isSet("-i")) {
        opt.get("-i")->getString(idf_path);
    }

    std::string idf_name;
    auto const extension = idf_path.find( ".idf" );
    auto last_delimiter = idf_path.find_last_of( '/' );
    if ( last_delimiter != 0 )
        last_delimiter++;
    if (extension != std::string::npos) {
        idf_name = std::string( idf_path, last_delimiter, extension - last_delimiter );
    }

    std::string output_directory;
    opt.get("-o")->getString(output_directory);

    std::string matrix_path;
    if (opt.isSet("-m")) {
        opt.get("-m")->getString(matrix_path);
    }

    std::string params_path;
    if (opt.isSet("-p")) {
        opt.get("-p")->getString(params_path);
    }

	int number_of_threads = 2;
	if (opt.isSet("-j")) {
		opt.get("-j")->getInt(number_of_threads);
	}

    if (output_directory.back() != kPathSeparator) {
        output_directory.push_back(kPathSeparator);
    }

	make_dir(output_directory);

#ifdef _OPENMP
	omp_set_num_threads(number_of_threads);
#endif

    Input user_input(idf_path, idf_name, matrix_path, params_path, "", output_directory);

    generate_input_output_file(user_input);
    generate_output_mapping(user_input);

    return 0;
}
