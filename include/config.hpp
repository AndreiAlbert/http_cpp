#include <filesystem>

using namespace std::filesystem;
using std::string;

struct Options {
    int port;
    path root; 
    bool log;
    path directory_for_log;
};

Options load_config(const path& config_file_name);
