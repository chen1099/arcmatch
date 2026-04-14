#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>

namespace graph {
    using GraphInt = int;

    struct Node {
        public:
        GraphInt id;
        std::string label;

        Node(std::string _label = "", GraphInt _id = -1): label(_label), id(_id) {}
    };

    struct Edge {
        GraphInt id;
        std::string label;
        GraphInt source;
        GraphInt target;

        Edge(GraphInt _source, GraphInt _target, std::string _label = "", GraphInt _id = -1): source(_source), target(_target), label(_label), id(_id) {}
    };

    struct GraphIntHash {
        std::size_t operator() (GraphInt x) const {
            return x;
        }
    };

    struct Graph {
        protected:
        std::vector<Node> _V;
        std::vector<Edge> _E;
        std::vector<std::unordered_map<GraphInt, GraphInt>> _N;

        public:
        const bool is_bidirected;

        const std::vector<Node> &V() const { return _V; }
        const std::vector<Edge> &E() const { return _E; }
        const std::vector<std::unordered_map<GraphInt, GraphInt>> &N() const { return _N; }

        Graph(bool _is_bidirected = true): is_bidirected(_is_bidirected) {}

        GraphInt add_node(Node &node) {
            node.id = _V.size();

            _V.push_back(node);

            _N.emplace_back();

            return node.id;
        }

        GraphInt add_edge(Edge &edge) {
            if (edge.source < 0 || edge.source >= _V.size()) return -1;
            if (edge.target < 0 || edge.target >= _V.size()) return -1;

            edge.id = _E.size();

            _E.push_back(edge);

            _N[edge.source][edge.target] = edge.id;

            if (is_bidirected) {
                _N[edge.target][edge.source] = edge.id;
            }

            return edge.id;
        }

        GraphInt get_e(GraphInt source, GraphInt target) const {
            if (source < 0 || source >= _V.size()) return -1;
            if (target < 0 || target >= _V.size()) return -1;

            auto it = _N[source].find(target);

            if (it != _N[source].end()) {
                return it->second;
            }

            if (!is_bidirected) return -1;

            it = _N[target].find(source);
            
            if (it != _N[target].end()) {
                return it->second;
            }

            return -1;
        }

        void print() const {
            std::cout << "V: [" << std::endl;
            for (int i = 0; i < _V.size(); i++) {
                std::cout << "\"" << i << "\"" << (_V[i].label != "" ? ": " + _V[i].label : "") << std::endl;
            }
            std::cout << "]" << std::endl;

            std::cout << "E: [" << std::endl;
            for (int i = 0; i < _E.size(); i++) {
                std::cout << "(" << _E[i].source << ", " << _E[i].target << ")" << (_E[i].label != "" ? ": " + _E[i].label : "") << std::endl;
            }
            std::cout << "]" << std::endl;
        }
    };

    Graph read(std::string filename) {
        Graph g;
        std::string line;
        int n, count;

        std::ifstream file(filename);

        while (std::getline(file, line)) {
            std::istringstream iss(line);
            if (iss >> n) {
                break;
            }
        }

        count = 0;

        while (count < n && std::getline(file, line)) {
            std::istringstream iss(line);
            std::string label;

            iss >> label;

            Node n = Node(label);

            g.add_node(n);
                
            count++;
        }

        while (std::getline(file, line)) {
            std::istringstream iss(line);
            if (iss >> n) {
                break;
            }
        }

        count = 0;
        
        while (count < n && std::getline(file, line)) {
            std::istringstream iss(line);
            std::string label;
            int n1, n2;

            if (!(iss >> n1)) continue;
            if (!(iss >> n2)) continue;
            iss >> label;

            Edge e = Edge(n1, n2, label);

            g.add_edge(e);

            count++;
        }

        return g;
    }
}

#endif