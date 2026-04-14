# Arcmatch
An subgraph isomorphism algorithm.

## Requirement
C++ compiler with C++11 and above standard version(keyword "auto" used).

## Usage
### Compile (Linux or MacOS)
``` bash
g++ -std=c++11 example.cpp -o example
```
### Execute (Linux or MacOS)
```bash
./example
```

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
- Add CLI support
- Change refine_domain() to recursive function (just an idea)
- Add directed graph read support (and update arcmatch for directed graph support)

## Ref.
- Bonnici, V., Grasso, R., Micale, G., Maria, A. D., Shasha, D., Pulvirenti, A., & Giugno, R. (2024). 
ArcMatch: high-performance subgraph matching for labelled graphs by exploiting edge domains. 
Data Mining and Knowledge Discovery, 38(6), 3868-3921.
https://doi.org/10.1007/s10618-024-01061-8