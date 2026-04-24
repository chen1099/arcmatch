extern "C" {
    #include <sys/time.h>
}

#include "./include/arcmatch.hpp"

void timeval_subtract(struct timeval *res, const struct timeval *x, const struct timeval *y) {
    res->tv_sec = x->tv_sec - y->tv_sec;
    res->tv_usec = x->tv_usec - y->tv_usec;
    if (res->tv_usec < 0) {
        res->tv_sec--;
        res->tv_usec += 1000000;
    }
}

int main() {
    struct timeval start, end, diff;

    graph::Graph Gq = graph::read("./graphs/query.txt");
    graph::Graph Gt = graph::read("./graphs/target.txt");

    gettimeofday(&start, NULL);

    std::vector<arcmatch::Match<graph::GraphInt>> m = arcmatch::match(Gq, Gt, 2);

    gettimeofday(&end, NULL);

    timeval_subtract(&diff, &end, &start);

    printf("Time used: %ld.%06ld seconds\n", diff.tv_sec, diff.tv_usec);

    arcmatch::match_print(m, Gq, Gt);

    return 0;
}
