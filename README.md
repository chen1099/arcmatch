# Arcmatch
An subgraph isomorphism algorithm.

## Requirement
C++ compiler with C++11 and above standard version(keyword "auto" used).

## Usage
### Compile example(Linux or MacOS)
``` bash
g++ -std=c++11 example.cpp -o example
```
### Execute example(Linux or MacOS)
```bash
./example
```

### Compile CLI(linux or MacOS)
```bash
g++ -std=c++11 arcmatch.cpp -o arcmatch
```

### Execute CLI(Linux or MacOS)
```bash
./arcmatch --help
```
#### The output:
##### Usage: arcmatch [options]

##### Required Arguments:
- -q, --query <path>       Path to the query graph file.
- -t, --target <path>      Path to the target graph file.

##### Algorithm Options:
- -lp, --length-path <n>   Set the path length for the arcmatch (Default: 2).
- -p, --print              Print the arcmatch's internal data structures (Domain).

##### General Options:
- -h, --help               Show this help message and exit.
- -o, --output <path>      Path to save the output graph(dot format).

## Graph file format
- [n. vertex] = number
- [vertex label x] = string
- [n. vertex] = number
- [source x] = number
- [target x] = number
- [vertex label x] = string

```
[n. vertex]
[vertex label 1]
[vertex label 2]
...
[vertex label n]
[n. edge]
[source 1] [target 1] [edge label 1]
[source 2] [target 2] [edge label 2]
...
[source m] [target m] [edge label m]
```


### TODO:
- Change refine_domain() to recursive function (just an idea)
- Add directed graph read support (and update arcmatch for directed graph support)

## Ref.
- Bonnici, V., Grasso, R., Micale, G., Maria, A. D., Shasha, D., Pulvirenti, A., & Giugno, R. (2024). 
ArcMatch: high-performance subgraph matching for labelled graphs by exploiting edge domains. 
Data Mining and Knowledge Discovery, 38(6), 3868-3921.
https://doi.org/10.1007/s10618-024-01061-8