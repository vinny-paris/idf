#include "simulate.hpp"
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#ifdef _OPENMP
#include <omp.h>
#endif

int run_simulation(std::string const &executable, std::string const &arguments) {
//  pid_t parent = getpid();
    pid_t pid = fork();

    if (pid == -1) {
        // error, failed to fork()
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
    } else {
        std::cout << executable << " " << arguments << std::endl;
        return execl(executable.c_str(), arguments.c_str());
    }
    return -1;
}

void simulate(Input const & user_input,
              std::string const &output_directory,
              std::string const &energy_plus_directory) {
    std::string system_call(
            energy_plus_directory + "energyplus" + " -r -d " + output_directory + " -w " + user_input.weather_file + " " + user_input.idf_path);
    int return_value = system(system_call.c_str());

//            std::string arguments("-d " + output_dir + " -w " + weather_file + " " + user_input.idf_path);
//            int return_value = run_simulation( energy_plus, arguments );

    if (return_value != 0) {
        std::cout << "Bad simulation: " << user_input.idf_path << " " << user_input.weather_file << std::endl;
        exit(EXIT_FAILURE);
    }
}
