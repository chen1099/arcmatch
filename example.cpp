#include "./include/arcmatch.hpp"

int main() {
    graph::Graph Gq = graph::read("./graphs/query.txt");
    graph::Graph Gt = graph::read("./graphs/target.txt");

    std::vector<arcmatch::Match<graph::GraphInt>> m = arcmatch::match(Gq, Gt, 2);

    arcmatch::match_print(m, Gq, Gt);

    return 0;
}