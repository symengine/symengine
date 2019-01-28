#ifndef COMMUTATIVE_MATCHER_H
#define COMMUTATIVE_MATCHER_H

#include "bipartite.h"
#include "utils.h"

#include <map>

class OperationMeta
{
public:
};

typedef BipartiteGraph<tuple<int, int>, tuple<int, int>, vector<Substitution>>
    Subgraph;
typedef map<tuple<int, int>, tuple<int, int>> Matching;

class CommutativeMatcher
{
public:
    map<RCP<const Basic>, tuple<int, set<int>>, lessBasic> subjects;
    BipartiteGraph<int, int, vector<Substitution>> bipartite;
    int max_optional_count;
    // map subject_pattern_ids;
    map<int, RCP<const Basic>> subjects_by_id;
    map<set<int>,
        tuple<int, multiset<int>, set<tuple<VariableWithCount, OperationMeta>>>>
        patterns;
    set<int> anonymous_patterns;

    CommutativeMatcher()
    {
    }

    virtual ~CommutativeMatcher()
    {
    }

    virtual generator<tuple<int, Substitution>>
    get_match_iter(RCP<const Basic> subject) = 0;

    int add_subject(RCP<const Basic> subject)
    {
        int subject_id;
        if (subjects.find(subject) == subjects.end()) {
            subject_id = subjects.size();
            set<int> pattern_set;
            // tuple<int, set<RCP<const Basic>>> elem =
            // make_tuple(subjects.size(), set<RCP<const Basic>>());
            subjects[subject] = make_tuple(subject_id, pattern_set);
            for (auto &p : get_match_iter(subject)) {
                int pattern_index = get<0>(p);
                Substitution substitution = get<1>(p);

                bipartite
                    .setdefault(make_tuple(subject_id, pattern_index),
                                vector<Substitution>())
                    .push_back(Substitution(substitution));
                pattern_set.insert(pattern_index);
            }
        } else {
            subject_id = get<0>(subjects[subject]);
        }
        return subject_id;
    }

    generator<tuple<int, Substitution>> match(RCP<const Basic> subject,
                                              Substitution substitution)
    {
        Deque subjects = {subject};
        return match(subjects, substitution);
    }

    // def match(self, subjects: Sequence[Expression], substitution:
    // Substitution) -> Iterator[Tuple[int, Substitution]]:
    generator<tuple<int, Substitution>> match(Deque subjects,
                                              Substitution substitution)
    {
        vector<tuple<int, Substitution>> result;

        multiset<int> subject_ids;
        multiset<int> pattern_ids;
        int subject_id;
        // int pattern_index;
        multiset<int> pattern_set;
        tuple<> pattern_vars;
        set<int> subject_pattern_ids;
        if (max_optional_count > 0) {
            subject_id = get<0>(this->subjects[None]);
            subject_pattern_ids = get<1>(this->subjects[None]);
            subject_ids.insert(subject_id);
            for (int i = 0; i < max_optional_count; ++i) {
                pattern_ids.insert(subject_pattern_ids.begin(),
                                   subject_pattern_ids.end());
            }
        }
        for (RCP<const Basic> &subject : subjects) {
            tuple<int, set<int>> p = this->subjects[subject];
            subject_id = get<0>(p);
            subject_pattern_ids = get<1>(p);
            subject_ids.insert(subject_id);
            pattern_ids.insert(subject_pattern_ids.begin(),
                               subject_pattern_ids.end());
        }
        for (const pair<set<int>,
                        tuple<int, multiset<int>,
                              set<tuple<VariableWithCount, OperationMeta>>>>
                 &p : patterns) {
            int pattern_index = get<0>(p.second);
            multiset<int> pattern_set = get<1>(p.second);
            set<tuple<VariableWithCount, OperationMeta>> pattern_vars
                = get<2>(p.second);
            if (pattern_set.size() > 0) {
                if (!includes(pattern_set.begin(), pattern_set.end(),
                              pattern_ids.begin(), pattern_ids.end())) {
                    continue;
                }
                vector<tuple<Substitution, multiset<int>>> bipartite_match_iter
                    = _match_with_bipartite(subject_ids, pattern_set,
                                            substitution);
                for (tuple<Substitution, multiset<int>> &p :
                     bipartite_match_iter) {
                    Substitution bipartite_substitution = get<0>(p);
                    multiset<int> matched_subjects = get<1>(p);
                    multiset<int> ids;
                    set_difference(subject_ids.begin(), subject_ids.end(),
                                   matched_subjects.begin(),
                                   matched_subjects.end(), inserter(ids, ids.begin()));
                    MultisetOfBasic remaining;
                    for (int id : ids) {
                        if (subjects_by_id[id] == None) {
                            continue;
                        }
                        remaining.insert(subjects_by_id[id]);
                    }
                    if (pattern_vars.size() > 0) {
                        vector<Substitution> sequence_var_iter
                            = _match_sequence_variables(remaining, pattern_vars,
                                                        bipartite_substitution);
                        for (Substitution &result_substitution :
                             sequence_var_iter) {
                            // YIELD:
                            result.push_back(
                                make_tuple(pattern_index, result_substitution));
                        }
                    } else if (remaining.size() == 0) {
                        result.push_back(
                            make_tuple(pattern_index, bipartite_substitution));
                    }
                }
            } else if (pattern_vars.size() > 0) {
                MultisetOfBasic multiset_arg(subjects.begin(), subjects.end());
                vector<Substitution> sequence_var_iter
                    = _match_sequence_variables(multiset_arg, pattern_vars,
                                                substitution);
                for (Substitution &variable_substitution : sequence_var_iter) {
                    // YIELD
                    result.push_back(
                        make_tuple(pattern_index, variable_substitution));
                }
            } else if (subjects.size() == 0) {
                // YIELD:
                result.push_back(make_tuple(pattern_index, substitution));
            }
        }
        return result;
    }

    /*
     * Variables for `_match_with_bipartite`:
     */

    // -> Iterator[Tuple[Substitution, MultisetOfInt]]:
    vector<tuple<Substitution, multiset<int>>>
    _match_with_bipartite(multiset<int> subject_ids, multiset<int> pattern_set,
                          Substitution substitution)
    {
        vector<tuple<Substitution, multiset<int>>> result;

        typedef vector<Substitution> TEdgeValue;

        Subgraph bipartite = _build_bipartite(subject_ids, pattern_set);
        for (const Matching &matching :
             enum_maximum_matchings_iter<tuple<int, int>, tuple<int, int>,
                                         vector<Substitution>>(bipartite)) {
            if (matching.size() < pattern_set.size()) {
                break;
            }
            if (!_is_canonical_matching(matching)) {
                continue;
            }
            auto loop = [&](list<vector<Substitution>> elab) {

            };
            vector<TEdgeValue> iterobjs;
            for (const pair<tuple<int, int>, tuple<int, int>> &p3 : matching) {
                iterobjs.push_back(
                    bipartite.__getitem__(make_tuple(p3.first, p3.second)));
            }
            for (vector<Substitution> &substs : itertools_product(iterobjs)) {
                Substitution bipartite_substitution
                    = substitution_union(substitution, substs);

                multiset<int> matched_subjects;
                for (const pair<tuple<int, int>, tuple<int, int>> &p3 :
                     matching) {
                    int elem = get<0>(p3.first);
                    int count = get<1>(p3.first);
                    for (int j = 0; j < count; j++) {
                        matched_subjects.insert(elem);
                    }
                }
                // YIELD:
                result.push_back(
                    make_tuple(bipartite_substitution, matched_subjects));
            }
        }
        return result;
    }

    vector<Substitution> _match_sequence_variables(
        MultisetOfBasic subjects,
        set<tuple<VariableWithCount, OperationMeta>> pattern_vars,
        Substitution substitution)
    {
        vector<Substitution> result;

        vector<VariableWithCount> only_counts;
        list<string> wrapped_vars;
        for (const tuple<VariableWithCount, OperationMeta> &p : pattern_vars) {
            only_counts.push_back(get<0>(p));
            wrapped_vars.push_back(get<0>(p).name->__str__());
        }
        for (Substitution &variable_substitution :
             commutative_sequence_variable_partition_iter(subjects,
                                                          only_counts)) {
            for (auto &var : wrapped_vars) {
                MultisetOfBasic operands = variable_substitution[var];

                //				for (const pair<RCP<const Basic>, int> &pp :
                //count_multiset(operands)) {
                //					RCP<const Basic> operands_var = pp.first;
                //					int operands_size = pp.second;
                if (operands.size() > 1) {
                    // variable_substitution[var] = associative(operands);
                    throw runtime_error("not implemented");
                } else {
                    variable_substitution[var] = operands;
                }
            }
            Substitution result_substitution
                = substitution_union(substitution, {variable_substitution});

            // YIELD:
            result.push_back(result_substitution);
        }
        return result;
    }

    bool _is_canonical_matching(Matching matching)
    {
        // anonymous_patterns = self.anonymous_patterns
        for (const pair<tuple<int, int>, tuple<int, int>> &pair1 : matching) {
            //.items():
            int s1 = get<0>(pair1.first);
            int n1 = get<1>(pair1.first);
            int p1 = get<0>(pair1.second);
            int m1 = get<1>(pair1.second);
            for (const pair<tuple<int, int>, tuple<int, int>> &pair2 :
                 matching) {
                int s2 = get<0>(pair2.first);
                int n2 = get<1>(pair2.first);
                int p2 = get<0>(pair2.second);
                int m2 = get<1>(pair2.second);

                if ((anonymous_patterns.find(p1) != anonymous_patterns.end())
                    && (anonymous_patterns.find(p2)
                        != anonymous_patterns.end())) {
                    if (n1 < n2 && m1 > m2) {
                        return false;
                    } else if (s1 == s2 && n1 < n2 && m1 > m2) {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    Subgraph _build_bipartite(multiset<int> subjects, multiset<int> patterns)
    {
        Subgraph bipartite;
        int n = 0;
        int m = 0;
        map<int, int> p_states;
        for (const pair<int, int> &p : count_multiset(subjects)) {
            int subject = p.first;
            int s_count = p.second;
            if (this->bipartite._graph_left.find(subject)
                != this->bipartite._graph_left.end()) {
                // for (const pair<int, set<int>> &subp :
                //     this->bipartite._graph_left) {
                bool any_patterns = false;
                for (int pattern : this->bipartite._graph_left[subject]) {
                    if (patterns.find(pattern) != patterns.end()) {
                        vector<Substitution> subst
                            = this->bipartite.__getitem__(
                                make_tuple(subject, pattern));
                        int p_count = count_multiset(patterns).at(pattern);
                        int p_start;
                        if (p_states.find(pattern) != p_states.end()) {
                            p_start = p_states[pattern];
                        } else {
                            p_start = p_states[pattern] = m;
                            m += p_count;
                        }
                        for (int i = n; i < n + s_count; i++) {
                            for (int j = p_start; j < p_start + p_count; j++) {
                                // (subject, i), (pattern, j)
                                bipartite.__setitem__(
                                    make_tuple(make_tuple(subject, i),
                                               make_tuple(pattern, j)),
                                    subst);
                            }
                        }
                    }
                }
                if (any_patterns) {
                    n += s_count;
                }
            }
        }

        return bipartite;
    }
};

#endif

/*
class CommutativeMatcher(object):
    __slots__ = (
        'patterns', 'subjects', 'subjects_by_id', 'automaton', 'bipartite',
'associative', 'max_optional_count', 'anonymous_patterns'
    )

    def __init__(self, associative: Optional[type]) -> None:
        self.patterns = {}
        self.subjects = {}
        self.subjects_by_id = {}
        self.automaton = ManyToOneMatcher()
        self.bipartite = BipartiteGraph()
        self.associative = associative
        self.max_optional_count = 0
        self.anonymous_patterns = set()

    int add_pattern(vector<RCP<const Basic>> operands, vector<int> constraints)
    {
        pattern_set, pattern_vars = self._extract_sequence_wildcards(operands,
constraints)
        sorted_vars = tuple(sorted(pattern_vars.values(), key=lambda v: (v[0][0]
or '', v[0][1], v[0][2], v[1])))
        sorted_subpatterns = tuple(sorted(pattern_set))
        pattern_key = sorted_subpatterns + sorted_vars
        if pattern_key not in self.patterns:
            inserted_id = len(self.patterns)
            self.patterns[pattern_key] = (inserted_id, pattern_set, sorted_vars)
        else:
            inserted_id = self.patterns[pattern_key][0]
        return inserted_id
    }


    def _extract_sequence_wildcards(self, operands: Iterable[Expression],
                                    constraints) -> Tuple[MultisetOfInt,
Dict[str, Tuple[VariableWithCount, bool]]]:
        pattern_set = Multiset()
        pattern_vars = dict()
        opt_count = 0
        for operand in op_iter(operands):
            if isinstance(operand, Wildcard) and operand.optional is not None:
                opt_count += 1
            if not self._is_sequence_wildcard(operand):
                actual_constraints = [c for c in constraints if
contains_variables_from_set(operand, c.variables)]
                pattern = Pattern(operand, *actual_constraints)
                index = None
                for i, (p, _, _) in enumerate(self.automaton.patterns):
                    if pattern == p:
                        index = i
                        break
                else:
                    vnames = set(e.variable_name for e in
preorder_iter(pattern.expression) if hasattr(e, 'variable_name') and
e.variable_name is not None)
                    renaming = {n: n for n in vnames}
                    index = self.automaton._internal_add(pattern, None,
renaming)
                    if is_anonymous(pattern.expression):
                        self.anonymous_patterns.add(index)
                pattern_set.add(index)
            else:
                varname = getattr(operand, 'variable_name', None)
                if varname is None:
                    if varname in pattern_vars:
                        (_, _, min_count, _), _ = pattern_vars[varname]
                    else:
                        min_count = 0
                    pattern_vars[varname] = (VariableWithCount(varname, 1,
operand.min_count + min_count, None), False)
                else:
                    if varname in pattern_vars:
                        (_, count, _, _), wrap = pattern_vars[varname]
                    else:
                        count = 0
                        wrap = operand.fixed_size and self.associative
                    pattern_vars[varname] = (
                        VariableWithCount(varname, count + 1, operand.min_count,
operand.optional), wrap
                    )
        if opt_count > self.max_optional_count:
            self.max_optional_count = opt_count
        return pattern_set, pattern_vars

    def _is_sequence_wildcard(self, expression: Expression) -> bool:
        if isinstance(expression, SymbolWildcard):
            return False
        if isinstance(expression, Wildcard):
            return not expression.fixed_size or self.associative
        return False


    def bipartite_as_graph(self) -> Graph:  # pragma: no cover
        """Returns a :class:`graphviz.Graph` representation of this bipartite
graph."""
        if Graph is None:
            raise ImportError('The graphviz package is required to draw the
graph.')
        graph = Graph()
        nodes_left = {}  # type: Dict[TLeft, str]
        nodes_right = {}  # type: Dict[TRight, str]
        node_id = 0
        for (left, right), value in self.bipartite._edges.items():
            if left not in nodes_left:
                name = 'node{:d}'.format(node_id)
                nodes_left[left] = name
                label = str(self.subjects_by_id[left])
                graph.node(name, label=label)
                node_id += 1
            if right not in nodes_right:
                name = 'node{:d}'.format(node_id)
                nodes_right[right] = name
                label = str(self.automaton.patterns[right][0])
                graph.node(name, label=label)
                node_id += 1
            edge_label = value is not True and str(value) or ''
            graph.edge(nodes_left[left], nodes_right[right], edge_label)
        return graph

    def concrete_bipartite_as_graph(self, subjects, patterns) -> Graph:  #
pragma: no cover
        """Returns a :class:`graphviz.Graph` representation of this bipartite
graph."""
        if Graph is None:
            raise ImportError('The graphviz package is required to draw the
graph.')
        bipartite = self._build_bipartite(subjects, patterns)
        graph = Graph()
        nodes_left = {}  # type: Dict[TLeft, str]
        nodes_right = {}  # type: Dict[TRight, str]
        node_id = 0
        for (left, right), value in bipartite._edges.items():
            if left not in nodes_left:
                subject_id, i = left
                name = 'node{:d}'.format(node_id)
                nodes_left[left] = name
                label = '{}, {}'.format(i, self.subjects_by_id[subject_id])
                graph.node(name, label=label)
                node_id += 1
            if right not in nodes_right:
                pattern, i = right
                name = 'node{:d}'.format(node_id)
                nodes_right[right] = name
                label = '{}, {}'.format(i, self.automaton.patterns[pattern][0])
                graph.node(name, label=label)
                node_id += 1
            edge_label = value is not True and str(value) or ''
            graph.edge(nodes_left[left], nodes_right[right], edge_label)
        return graph

*/
