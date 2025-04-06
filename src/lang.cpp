#include <boost/program_options.hpp>
#include <iostream>

namespace opt = boost::program_options;

using namespace std;

int main(int argc, char* argv[]) {
    opt::options_description desc("All options");
    
    string prm_name;
    
    desc.add_options()
        ("file", opt::value<string>(), "file to be executed.")
        ("help", "help msg");

    // parsing opts
    opt::variables_map opt_map;
    opt::store(opt::parse_command_line(argc, argv, desc), opt_map);
    opt::notify(opt_map);

    if (opt_map.count("file")) {
        //printf("bad!\n");
    }
    
    if (opt_map.count("help")) {
        std::cout << desc;
    }

    printf("end ok!\n");
}