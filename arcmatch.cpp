#include <iostream>
#include <string>
#include <vector>
#include "./include/arcmatch.hpp"

struct State {
    bool print_help = false;
    bool print_domains = false;
    std::string query_graph_path = "";
    std::string target_graph_path = "";
    int length_path = 2;
    std::string output_graph_path = "";
};

State global_state;

void parser(std::vector<std::string> args) {
    for (int i = 0; i < args.size(); i++) {
        std::string param = args[i];

        if (param == "--help" || param == "-h") {
            global_state.print_help = true;
            continue;
        }

        if (param == "--print" || param == "-p") {
            global_state.print_domains = true;
            continue;
        }

        if (i + 1 >= args.size()) continue;

        if (param == "--query" || param == "-q") {
            std::string next_param = args[i + 1];
            global_state.query_graph_path = next_param;
            continue;
        }

        if (param == "--target" || param == "-t") {
            std::string next_param = args[i + 1];
            global_state.target_graph_path = next_param;
            continue;
        }

        if (param == "--length-path" || param == "-lp") {
            std::string next_param = args[i + 1];

            try {
                int temp = std::stoi(next_param);

                if (temp > 0) {
                    global_state.length_path = temp;
                }
            } catch (std::exception e) {}

            continue;
        }

        if (param == "--output" || param == "-o") {
            std::string next_param = args[i + 1];
            global_state.output_graph_path = next_param;
            continue;
        }
    }
}

int main(int argc, char *argv[]) {
    std::vector<std::string> args(argv, argv + argc);

    parser(args);

    if (global_state.print_help) {
        std::cout << "Usage: " << args[0] << " [options]\n\n"
                  << "Required Arguments:\n"
                  << "  -q, --query <path>       Path to the query graph file.\n"
                  << "  -t, --target <path>      Path to the target graph file.\n\n"
                  << "Algorithm Options:\n"
                  << "  -lp, --length-path <n>   Set the path length for the arcmatch (Default: 2).\n"
                  << "  -p, --print              Print the arcmatch's internal data structures (Domain).\n\n"
                  << "General Options:\n"
                  << "  -h, --help               Show this help message and exit.\n"
                  << "  -o, --output <path>      Path to save the output graph(dot format).\n";
        return 0;
    }

    if (global_state.query_graph_path.empty() || global_state.target_graph_path.empty()) {
        std::cerr << "Error: Missing required arguments.\n"
                  << "You must provide both --query and --target.\n"
                  << "Run with --help for more information.\n";
        return -1;
    }

    try {
        graph::Graph Gq = graph::read(global_state.query_graph_path);
        graph::Graph Gt = graph::read(global_state.target_graph_path);
        int lp = global_state.length_path;
        bool is_print = global_state.print_domains;

        std::vector<arcmatch::Match<graph::GraphInt>> M = arcmatch::match(Gq, Gt, lp, is_print);

        arcmatch::match_print(M, Gq, Gt);

        if (!global_state.output_graph_path.empty()) {
            arcmatch::match_save_as_dot(M, Gt, global_state.output_graph_path);
        }
    } catch (std::ifstream::failure e) {
        std::cerr << "Error while opening/reading file: " << e.what();
        return -1;
    } catch (std::ofstream::failure e) {
        std::cerr << "Error while opening/writing file: " << e.what();
    } catch (std::exception e) {
        std::cerr << "Error: " << e.what();
        return -1;
    }

    return 0;
}