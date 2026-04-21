#ifndef ARCMATCH_HPP
#define ARCMATCH_HPP

#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include "graph.hpp"

namespace arcmatch {
    template <typename T>
    using Domain = std::vector<std::unordered_set<T>>;

    template <typename T>
    using Match = std::unordered_map<T, T>;

    void vertex_domain_print(const graph::Graph &Gq, const graph::Graph &Gt, const Domain<graph::GraphInt> DV);
    void edge_domain_print(const graph::Graph &Gq, const graph::Graph &Gt, const Domain<graph::GraphInt> DE);

    Domain<graph::GraphInt> init_vertex_domain(const graph::Graph &Gq, const graph::Graph &Gt) {
        Domain<graph::GraphInt> d(Gq.V().size());

        for (graph::Node q: Gq.V()) {
            for (graph::Node t: Gt.V()) {
                if (q.label == t.label && Gq.N()[q.id].size() <= Gt.N()[t.id].size()) {
                    d[q.id].insert(t.id);
                }
            }
        }

        return d;
    }

    bool has_edge_with_label(const graph::Graph &G, const graph::Node &source, const graph::Node &target, const std::string &required_label) {
        graph::GraphInt e_idx = G.get_e(source.id, target.id);

        if (e_idx < 0) return false;

        if (G.E()[e_idx].label != required_label) return false;
        
        return true;
    }

    void arc_consistency(const graph::Graph &Gq, const graph::Graph &Gt, Domain<graph::GraphInt> &DV) {
        bool reduced = true;

        while (reduced) {
            reduced = false;

            for (graph::Edge eq: Gq.E()) {
                graph::GraphInt qi_idx = eq.source;
                graph::GraphInt qj_idx = eq.target;

                for (auto ti_idx = DV[qi_idx].begin(); ti_idx != DV[qi_idx].end();) {
                    bool found = false;

                    for (graph::GraphInt tj_idx: DV[qj_idx]) {
                        if (has_edge_with_label(Gt, Gt.V()[*ti_idx], Gt.V()[tj_idx], eq.label)) {
                            found = true;
                            break;
                        }
                    }

                    if (!found) {
                        ti_idx = DV[qi_idx].erase(ti_idx);
                        reduced = true;
                    } else {
                        ti_idx++;
                    }
                }

                for (auto ti_idx = DV[qj_idx].begin(); ti_idx != DV[qj_idx].end();) {
                    bool found = false;

                    for (graph::GraphInt tj: DV[qi_idx]) {
                        if (has_edge_with_label(Gt, Gt.V()[*ti_idx], Gt.V()[tj], eq.label)) {
                            found = true;
                            break;
                        }
                    }

                    if (!found) {
                        ti_idx = DV[qj_idx].erase(ti_idx);
                        reduced = true;
                    } else {
                        ti_idx++;
                    }
                }
            }
        }
    }

    Domain<graph::GraphInt> init_edge_domain(const graph::Graph Gq, const graph::Graph Gt, const Domain<graph::GraphInt> DV) {
        Domain<graph::GraphInt> d(Gq.E().size());

        for (graph::Edge eq: Gq.E()) {
            for (graph::GraphInt ti_idx: DV[eq.source]) {
                for (graph::GraphInt tj_idx: DV[eq.target]) {
                    graph::GraphInt et_idx = Gt.get_e(ti_idx, tj_idx);

                    if (et_idx < 0) continue;

                    graph::Edge et = Gt.E()[et_idx];

                    if (et.label != eq.label) continue;

                    d[eq.id].insert(et_idx);
                }
            }
        }

        return d;
    }

    void refine_domains(graph::GraphInt qi_idx, const graph::Graph &Gq, const graph::Graph &Gt, Domain<graph::GraphInt> &DV, Domain<graph::GraphInt> &DE) {
        for (auto n: Gq.N()[qi_idx]) {
            graph::GraphInt eq_idx = n.second;

            for (auto et_idx = DE[eq_idx].begin(); et_idx != DE[eq_idx].end();) {
                graph::Edge et = Gt.E()[*et_idx];

                if (DV[qi_idx].find(et.source) == DV[qi_idx].end() && DV[qi_idx].find(et.target) == DV[qi_idx].end()) {
                    et_idx = DE[eq_idx].erase(et_idx);
                } else {
                    et_idx++;
                }
            }
        }

        bool reduced = true;

        while (reduced) {
            reduced = false;

            for (graph::Edge eq: Gq.E()) {
                for (auto et_idx = DE[eq.id].begin(); et_idx != DE[eq.id].end();) {
                    graph::Edge et = Gt.E()[*et_idx];

                    if ((DV[eq.source].find(et.source) == DV[eq.source].end() || DV[eq.target].find(et.target) == DV[eq.target].end()) &&
                        (DV[eq.source].find(et.target) == DV[eq.source].end() || DV[eq.target].find(et.source) == DV[eq.target].end())) {
                        et_idx = DE[eq.id].erase(et_idx);
                        reduced = true;
                    } else {
                        et_idx++;
                    }
                }
            }

            for (graph::Edge eq: Gq.E()) {
                for (auto ti_idx = DV[eq.source].begin(); ti_idx != DV[eq.source].end();) {
                    bool found = false;

                    for (auto n: Gt.N()[*ti_idx]) {
                        graph::GraphInt et_idx = n.second;

                        if (DE[eq.id].find(et_idx) != DE[eq.id].end()) {
                            found = true;
                            break;
                        }
                    }

                    if (!found) {
                        ti_idx = DV[eq.source].erase(ti_idx);
                        reduced = true;
                    } else {
                        ti_idx++;
                    }
                }

                for (auto ti_idx = DV[eq.target].begin(); ti_idx != DV[eq.target].end();) {
                    bool found = false;

                    for (auto n: Gt.N()[*ti_idx]) {
                        graph::GraphInt et_idx = n.second;

                        if (DE[eq.id].find(et_idx) != DE[eq.id].end()) {
                            found = true;
                            break;
                        }
                    }

                    if (!found) {
                        ti_idx = DV[eq.target].erase(ti_idx);
                        reduced = true;
                    } else {
                        ti_idx++;
                    }
                }
            }
        }
    }

    bool verify_path_dfs(std::vector<graph::GraphInt> &q_path, std::vector<graph::GraphInt> &t_path, bool is_ring, const graph::Graph &Gq, const graph::Graph &Gt, Domain<graph::GraphInt> &DV, Domain<graph::GraphInt> &DE) {
        if (q_path.size() == t_path.size()) return true;

        graph::GraphInt qi_idx = q_path[t_path.size() - 1];
        graph::GraphInt qj_idx = q_path[t_path.size()];
        graph::GraphInt eq_idx = Gq.get_e(qi_idx, qj_idx);

        if (eq_idx < 0) return false;

        graph::GraphInt ti_idx = t_path[t_path.size() - 1];

        for (auto n: Gt.N()[ti_idx]) {
            graph::GraphInt et_idx = n.second;

            if (DE[eq_idx].find(et_idx) != DE[eq_idx].end()) {
                graph::Edge et = Gt.E()[et_idx];

                graph::GraphInt tj_idx = (et.source == ti_idx ? et.target : et.source);

                if (is_ring && (t_path.size() == q_path.size() - 1)) {
                    if (tj_idx == t_path.front()) {
                        return true;
                    }
                } else {
                    bool visited = false;

                    for (graph::GraphInt i: t_path) {
                        if (i == tj_idx) {
                            visited = true;
                            break;
                        }
                    }

                    if (!visited) {
                        t_path.push_back(tj_idx);

                        if (verify_path_dfs(q_path, t_path, is_ring, Gq, Gt, DV, DE)) {
                            t_path.pop_back();
                            return true;
                        }

                        t_path.pop_back();
                    }
                }
            }
        }

        return false;
    }

    void verify_path(std::vector<graph::GraphInt> &q_path, bool is_ring, const graph::Graph &Gq, const graph::Graph &Gt, Domain<graph::GraphInt> &DV, Domain<graph::GraphInt> &DE) {
        bool reduced = false;

        for (auto ti_idx = DV[q_path.front()].begin(); ti_idx != DV[q_path.front()].end();) {
            std::vector<graph::GraphInt> t_path = { *ti_idx };

            if (!verify_path_dfs(q_path, t_path, is_ring, Gq, Gt, DV, DE)) {
                ti_idx = DV[q_path.front()].erase(ti_idx);
                reduced = true;
            } else {
                ti_idx++;
            }
        }

        if (reduced) {
            refine_domains(q_path.front(), Gq, Gt, DV, DE);
        }
    }

    void path_reduction(std::vector<graph::GraphInt> &q_path, int lp, const graph::Graph &Gq, const graph::Graph &Gt, Domain<graph::GraphInt> &DV, Domain<graph::GraphInt> &DE) {
        if (q_path.empty()) return;
        if (lp <= 0) return;

        bool extended = false;

        graph::Node qi = Gq.V()[q_path.back()];

        for (auto n: Gq.N()[qi.id]) {
            graph::GraphInt eq_idx = n.second;

            graph::Edge eq = Gq.E()[eq_idx];

            graph::GraphInt qj_idx = (eq.source == qi.id ? eq.target : eq.source);
            graph::Node qj = Gq.V()[qj_idx];

            if (q_path.front() == qj.id) {
                q_path.push_back(qj.id);

                verify_path(q_path, true, Gq, Gt, DV, DE);

                q_path.pop_back();
            } else {
                bool visited = false;

                for (graph::GraphInt i: q_path) {
                    if (i == qj.id) {
                        visited = true;
                        break;
                    }
                }

                if (!visited) {
                    extended = true;

                    q_path.push_back(qj.id);

                    if (q_path.size() == lp) {
                        verify_path(q_path, false, Gq, Gt, DV, DE);
                    }

                    path_reduction(q_path, lp, Gq, Gt, DV, DE);

                    q_path.pop_back();
                }
            }
        }

        if (!extended) {
            verify_path(q_path, false, Gq, Gt, DV, DE);
        }
    }

    graph::GraphInt N1(graph::GraphInt qj_idx, const std::vector<graph::GraphInt> &partial_ordered, const graph::Graph &Gq) {
        graph::GraphInt res = 0;

        for (graph::GraphInt qi_idx: partial_ordered) {
            if (Gq.N()[qi_idx].find(qj_idx) != Gq.N()[qi_idx].end()) {
                res++;
            }
        }

        return res;
    }

    graph::GraphInt N2(graph::GraphInt qj_idx, const std::vector<graph::GraphInt> &partial_ordered, const graph::Graph &Gq) {
        std::unordered_set<graph::GraphInt> res;

        std::unordered_set<graph::GraphInt> po;

        for (graph::GraphInt i: partial_ordered) {
            po.insert(i);
        }

        for (graph::GraphInt qi_idx: partial_ordered) {
            for (auto n: Gq.N()[qi_idx]) {
                graph::GraphInt qin_idx = n.first;

                if (po.find(qin_idx) != po.end()) continue;

                if (Gq.N()[qin_idx].find(qj_idx) != Gq.N()[qin_idx].end()) {
                    res.insert(qin_idx);
                }
            }
        }

        return res.size();
    }

    graph::GraphInt N3(graph::GraphInt qj_idx, const std::vector<graph::GraphInt> &partial_ordered, const graph::Graph &Gq) {
        std::unordered_set<graph::GraphInt> res;

        std::unordered_set<graph::GraphInt> po;
        
        for (graph::GraphInt i: partial_ordered) {
            po.insert(i);
        }

        for (graph::GraphInt qi_idx: partial_ordered) {
            if (Gq.N()[qi_idx].find(qj_idx) != Gq.N()[qi_idx].end()) {
                res.insert(qi_idx);
            }
            
            for (auto n: Gq.N()[qi_idx]) {
                graph::GraphInt qin_idx = n.first;
                
                if (po.find(qin_idx) != po.end()) continue;

                if (Gq.N()[qin_idx].find(qj_idx) != Gq.N()[qin_idx].end()) {
                    res.insert(qin_idx);
                }
            }
        }

        return Gq.N()[qj_idx].size() - res.size();
    }

    graph::GraphInt N4(graph::GraphInt qi_idx, const graph::Graph &Gq) {
        return Gq.N()[qi_idx].size();
    }

    graph::GraphInt N5(graph::GraphInt qi_idx, const Domain<graph::GraphInt> &DV) {
        return DV[qi_idx].size();
    }

    graph::GraphInt get_first_v(graph::GraphInt qi_idx, graph::GraphInt qj_idx, const std::vector<graph::GraphInt> &partial_ordered, const graph::Graph &Gq, const Domain<graph::GraphInt> &DV) {
        if (partial_ordered.size() == 0) return -1;

        graph::GraphInt qi_value = N1(qi_idx, partial_ordered, Gq);
        graph::GraphInt qj_value = N1(qj_idx, partial_ordered, Gq);

        if (qi_value != qj_value) {
            if (qi_value > qj_value) {
                return qi_idx;
            }
            return qj_idx;
        }

        qi_value = N2(qi_idx, partial_ordered, Gq);
        qj_value = N2(qj_idx, partial_ordered, Gq);
        
        if (qi_value != qj_value) {
            if (qi_value > qj_value) {
                return qi_idx;
            }
            return qj_idx;
        }
        
        qi_value = N3(qi_idx, partial_ordered, Gq);
        qj_value = N3(qj_idx, partial_ordered, Gq);
        
        if (qi_value != qj_value) {
            if (qi_value > qj_value) {
                return qi_idx;
            }
            return qj_idx;
        }

        qi_value = N4(qi_idx, Gq);
        qj_value = N4(qj_idx, Gq);

        if (qi_value != qj_value) {
            if (qi_value > qj_value) {
                return qi_idx;
            }
            return qj_idx;
        }

        qi_value = N5(qi_idx, DV);
        qj_value = N5(qj_idx, DV);

        if (qi_value <= qj_value) {
            return qi_idx;
        }
        return qj_idx;
    }

    std::vector<graph::GraphInt> variable_ordering(const graph::Graph &Gq, const Domain<graph::GraphInt> &DV) {
        std::vector<graph::GraphInt> ordered;

        std::unordered_set<graph::GraphInt> singleton;
        std::unordered_set<graph::GraphInt> other;

        for (graph::Node qi: Gq.V()) {
            bool is_added = false;

            if (DV[qi.id].size() == 1) {
                is_added = true;
                singleton.insert(qi.id);
            }

            if (is_added) continue;

            other.insert(qi.id);
        }

        bool is_first = true;

        while (true) {
            graph::GraphInt first_v = -1;
            auto first_singleton_it = singleton.end();
            auto first_other_it = other.end();

            for (auto it = singleton.begin(); it != singleton.end();) {
                if (is_first) {
                    first_v = *it;
                    first_singleton_it = it;
                    it++;
                    continue;
                }

                if (first_v == -1) {
                    if (N1(*it, ordered, Gq) != 0) {
                        first_v = *it;
                        first_singleton_it = it;
                        it++;
                        continue;
                    }

                    it++;
                } else {
                    graph::GraphInt temp = get_first_v(first_v, *it, ordered, Gq, DV);

                    if (temp != first_v){
                        first_v = temp;
                        first_singleton_it = it;
                    }

                    it++;
                }
            }

            if (first_v != -1) {
                singleton.erase(first_singleton_it);
                ordered.push_back(first_v);
                is_first = false;
                continue;
            }

            for (auto it = other.begin(); it != other.end();) {
                if (is_first) {
                    first_v = *it;
                    first_other_it = it;
                    it++;
                    continue;
                }

                if (first_v == -1) {
                    if (N1(*it, ordered, Gq) != 0) {
                        first_v = *it;
                        first_other_it = it;
                        it++;
                        continue;
                    }

                    it++;
                } else {
                    graph::GraphInt temp = get_first_v(first_v, *it, ordered, Gq, DV);

                    if (temp != first_v){
                        first_v = temp;
                        first_other_it = it;
                    }

                    it++;
                }
            }

            if (first_v != -1) {
                other.erase(first_other_it);
                ordered.push_back(first_v);
                is_first = false;
            } else {
                break;
            }
        }

        return ordered;
    }

    void search_occurrences(const std::vector<graph::GraphInt> &theta, std::vector<graph::GraphInt> &theta2, Match<graph::GraphInt> &curr_match, std::vector<Match<graph::GraphInt>> &M, const graph::Graph &Gq, const graph::Graph &Gt, Domain<graph::GraphInt> &DV, Domain<graph::GraphInt> &DE) {
        if (theta.size() == theta2.size()) return;

        graph::GraphInt curr_idx = theta2.size();

        graph::GraphInt s_idx = -1;
        graph::GraphInt es_idx;
        graph::GraphInt s_domain_size;

        for (graph::GraphInt i = 0; i < curr_idx; i++) {
            graph::GraphInt qi_idx = theta[i];
            graph::GraphInt qj_idx = theta[curr_idx];

            graph::GraphInt eq_idx = Gq.get_e(qi_idx, qj_idx);

            if (eq_idx < 0) continue;

            if (s_idx == -1 || s_domain_size > DE[eq_idx].size()) {
                s_idx = qi_idx;
                es_idx = eq_idx;
                s_domain_size = DE[eq_idx].size();
            }
        }

        if (s_idx == -1) return;

        for (graph::GraphInt et_idx: DE[es_idx]) {
            graph::Edge et = Gt.E()[et_idx];

            graph::GraphInt ti_idx;
            graph::GraphInt tj_idx;
            
            if (curr_match[s_idx] == et.source) {
                ti_idx = et.source;
                tj_idx = et.target;
            } else if (curr_match[s_idx] == et.target) {
                ti_idx = et.target;
                tj_idx = et.source;
            } else {
                continue;
            }

            bool visited = false;

            for (graph::GraphInt i: theta2) {
                if (i == tj_idx) {
                    visited = true;
                }
            }

            if (visited) continue;

            bool feasible = true;

            for (graph::GraphInt j = 0; j < curr_idx; j++) {
                graph::GraphInt qi_idx = theta[curr_idx];
                graph::GraphInt qj_idx = theta[j];

                graph::GraphInt eq_idx = Gq.get_e(qi_idx, qj_idx);

                if (eq_idx < 0) continue;

                graph::GraphInt ti_idx = tj_idx;
                graph::GraphInt tj_idx = theta2[j];

                graph::GraphInt et_idx = Gt.get_e(ti_idx, tj_idx);

                if (et_idx < 0) continue;

                if (DE[eq_idx].find(et_idx) == DE[eq_idx].end()) {
                    feasible = false;
                    break;
                }
            }

            if (feasible) {
                theta2.push_back(tj_idx);
                
                curr_match[theta[curr_idx]] = theta2[curr_idx];

                if (theta.size() == theta2.size()) {
                    M.push_back(curr_match);
                }
                else {
                    search_occurrences(theta, theta2, curr_match, M, Gq, Gt, DV, DE);
                }

                curr_match.erase(theta[curr_idx]);

                theta2.pop_back();
            }
        }
    }

    std::vector<Match<graph::GraphInt>> match(const graph::Graph &Gq, const graph::Graph &Gt, int lp, bool print_domains = false) {
        Domain<graph::GraphInt> DV = init_vertex_domain(Gq, Gt);

        arc_consistency(Gq, Gt, DV);

        Domain<graph::GraphInt> DE = init_edge_domain(Gq, Gt, DV);

        for (graph::Node qi: Gq.V()) {
            std::vector<graph::GraphInt> q_path = { qi.id };
            path_reduction(q_path, lp, Gq, Gt, DV, DE);
        }

        std::vector<graph::GraphInt> theta = variable_ordering(Gq, DV);

        std::vector<Match<graph::GraphInt>> M;

        vertex_domain_print(Gq, Gt, DV);
        edge_domain_print(Gq, Gt, DE);

        for (graph::GraphInt ti_idx: DV[theta[0]]) {
            std::vector<graph::GraphInt> theta2 = { ti_idx };

            Match<graph::GraphInt> curr_match;
            curr_match[theta[0]] = ti_idx;

            search_occurrences(theta, theta2, curr_match, M, Gq, Gt, DV, DE);
        }

        return M;
    }

    void vertex_domain_print(const graph::Graph &Gq, const graph::Graph &Gt, const Domain<graph::GraphInt> DV) {
        std::cout << "Printing Domain Vertex:" << std::endl;

        for (graph::Node vq: Gq.V()) {
            std::cout << "Domain of " << vq.id << " " << vq.label << std::endl;
        
            for (graph::GraphInt vt_idx: DV[vq.id]) {
                graph::Node vt = Gt.V()[vt_idx];
        
                std::cout << "\t" << vt_idx << ", " << vt.label << std::endl;
            }
        }

        std::cout << "End Printing Domain Vertex." << std::endl;
    }

    void edge_domain_print(const graph::Graph &Gq, const graph::Graph &Gt, const Domain<graph::GraphInt> DE) {
        std::cout << "Printing Domain Edge:" << std::endl;

        for (graph::Edge eq: Gq.E()) {
            std::cout << "Domain of " << eq.id << ", " << eq.label << "; source: " << eq.source << "; target: " << eq.target << std::endl;

            for (graph::GraphInt et_idx: DE[eq.id]) {
                graph::Edge et = Gt.E()[et_idx];

                std::cout << "\t" << et_idx << ", " << et.label << "; source: " << et.source << "; target: " << et.target << std::endl;
            }
        }

        std::cout << "End Printing Domain Edge." << std::endl;
    }

    void match_print(const std::vector<Match<graph::GraphInt>> &M, const graph::Graph &Gq, const graph::Graph &Gt) {
        std::cout << "Printing Match Unordered: " << std::endl;

        for (graph::GraphInt i = 0; i < M.size(); i++) {
            std::cout << "Match id: " << i << std::endl;

            for (auto match: M[i]) {
                graph::Node qi = Gq.V()[match.first];
                graph::Node ti = Gt.V()[match.second];

                std::cout << "query:  \t" << qi.id << ", " << qi.label << "\t => \t"
                          << "target: \t" << ti.id << ", " << ti.label << std::endl;
            }
        }

        std::cout << "End Printing Match Unordered." << std::endl;
    }
}

#endif