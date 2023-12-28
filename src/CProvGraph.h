#ifndef _CPROVGRAPH_H
#define _CPROVGRAPH_H

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/graphviz.hpp>
#include <string>
#include <queue>
#include <unordered_set>
#include <unordered_map>


#ifndef NDEBUG
#define ASSERT_EX(condition, statement) \
    do { \
        if (!(condition)) { statement; assert(condition); } \
    } while (false)
#else
#define ASSERT_EX(condition, statement) ((void)0)
#endif

#define MAX_NUM_OF_SEARCH 100
#define ALPHA 0.5
#define BETA 0.05
#define PRUNE_STEP 10

namespace cpg {

enum VertexType {
                Input, Derived, Parameter,  // variables
                Sum, Mul, Div, Scale,  // basics
                Softmax, Sigmoid, InnerProduct, InnerProductAct, // MLP
                VectorMean, NearestCentroid, // K-Means
                NodeToClique, CliqueToNode, BeliefPropagation // PGM
                };

inline std::ostream& operator<<(std::ostream& out, const VertexType value){
  const char* s = 0;
  #define PROCESS_VAL(p) case(p): s = #p; break;
    switch(value) {
      PROCESS_VAL(Input)
      PROCESS_VAL(Derived)     
      PROCESS_VAL(Parameter)
      PROCESS_VAL(Sum)
      PROCESS_VAL(Mul)
      PROCESS_VAL(Div)
      PROCESS_VAL(Scale)
      PROCESS_VAL(Softmax)
      PROCESS_VAL(Sigmoid)
      PROCESS_VAL(InnerProduct)
      PROCESS_VAL(InnerProductAct)
      PROCESS_VAL(VectorMean)
      PROCESS_VAL(NearestCentroid)
      PROCESS_VAL(NodeToClique)
      PROCESS_VAL(CliqueToNode)
      PROCESS_VAL(BeliefPropagation)
    }
  #undef PROCESS_VAL
    return out << s;
}

inline std::string vertexTypeToString(VertexType vt) {
  std::string s;
  switch(vt) {
    case(Input): s = "input"; break;
    case(Derived): s = "derived"; break;
    case(Parameter): s = "param"; break;
    case(Sum): s = "sum"; break;
    case(Mul): s = "mul"; break;
    case(Div): s = "div"; break;
    case(Scale): s = "scale"; break;
    case(Softmax): s = "softmax"; break;
    case(Sigmoid): s = "sigmoid"; break;
    case(InnerProduct): s = "innerProduct"; break;
    case(InnerProductAct): s = "innerProductAct"; break;
    case(VectorMean): s = "vectorMean"; break;
    case(NearestCentroid): s = "nearestCentroid"; break;
    case(NodeToClique): s = "nodetoclique"; break;
    case(CliqueToNode): s = "cliquetonode"; break;
    case(BeliefPropagation): s = "beliefpropagation"; break;
  }
  return s;
}

struct ProvVertex {
  VertexType vt;
  std::string name;
  float value;
  float contribution;
  float derivative;
  std::unordered_map<std::string, std::string> params;
  std::vector<double> weights;
  std::unordered_set<std::string> EDBs;
};

struct ProvEdge{
  float contribution;
  float derivative;
  float importance;
};

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, ProvVertex, ProvEdge> Graph;
typedef boost::graph_traits<Graph>::vertex_descriptor vertex_t;
typedef boost::graph_traits<Graph>::edge_descriptor edge_t;
typedef boost::graph_traits<Graph>::vertex_iterator vertex_iter;
typedef boost::graph_traits<Graph>::edge_iterator edge_iter;
typedef boost::graph_traits<Graph>::in_edge_iterator in_edge_iter;
typedef boost::graph_traits<Graph>::out_edge_iterator out_edge_iter;
typedef boost::graph_traits<Graph>::adjacency_iterator adjacency_tier;

struct my_node_writer {
  my_node_writer(Graph& g_) : g (g_) {};
  template <class Vertex>
  void operator()(std::ostream& out, Vertex v) {
    if (g[v].vt==Input) 
      out << " [label=\"" << g[v].name << ": " << g[v].value << ", d: " << g[v].derivative << "\", shape=oval, color=dodgerblue]";
    else if (g[v].vt==Derived) 
      out << " [label=\"" << g[v].name << ": " << g[v].value << ", d: " << g[v].derivative << "\", shape=oval, color=limegreen]";
    else if (g[v].vt==Parameter) 
      out << " [label=\"" << g[v].name << ": " << g[v].value << ", d: " << g[v].derivative << "\", shape=oval, color=darkviolet]";
    else 
      out << " [label=\"" << g[v].name << "\", shape=box, color=tomato]";
  };
  Graph g;
};

struct my_edge_writer {
  my_edge_writer(Graph& g_) : g (g_) {};
  template <class Edge>
  void operator()(std::ostream& out, Edge e) {
    out << " [color=purple]" << std::endl;
    out << " [label=\"";
    vertex_t v_target = boost::target(e, g);
    // std::cout << g[v_target].vt << std::endl;
    if (g[v_target].vt==Derived || g[v_target].vt==Input || g[v_target].vt==Parameter) {
      // if (abs(g[e].contribution)>0.001) {
      //   out << "Con: " << g[e].contribution << ' ';
      // }
      // if (abs(g[e].derivative)>0.001) {
      //   out << "Dev: " << g[e].derivative;
      // }
      out << "Con: " << g[e].contribution << ' ';
      out << "Dev: " << g[e].derivative;
    }
    out << "\"]";
  };
  Graph g;
};

struct my_graph_writer {
  void operator()(std::ostream& out) const {
    out << "graph [bgcolor=lightgrey]";
    out << "node [shape=circle color=blue]";
    out << "edge [color=red]";
  }
};

struct edge_comparer {
  Graph* g;
  bool reverse = false;
  edge_comparer(Graph* c_g): g(c_g) {}
  edge_comparer(Graph* c_g, bool c_reverse): g(c_g), reverse(c_reverse) {}
  bool operator() (edge_t e1, edge_t e2) const {
    if (reverse) return (*g)[e1].importance > (*g)[e2].importance;
    return (*g)[e1].importance < (*g)[e2].importance;
  }
};

class ApproxSubGraphDiff {
public: 
  std::unordered_set<std::string> EDBs;
  float value_diff;
  float derivative_diff;
  ApproxSubGraphDiff() {}
  ApproxSubGraphDiff(const std::unordered_set<std::string>& s, float v_d, float d_d): EDBs(s), value_diff(v_d), derivative_diff(d_d) {}

  bool operator == (const ApproxSubGraphDiff& asgd2) const {
    return this->EDBs==asgd2.EDBs;
  }

  inline float getDiff() {
    return value_diff*ALPHA+derivative_diff*BETA;
  }
};

class ApproxSubGraphDiffComparison {
public: 
  ApproxSubGraphDiffComparison() {};
  bool operator () (const ApproxSubGraphDiff& sd1, const ApproxSubGraphDiff& sd2) {
    return sd1.value_diff*ALPHA+sd1.derivative_diff*BETA>sd2.value_diff*ALPHA+sd2.derivative_diff*BETA; 
  }
};

class EDBSetHash {
public:
  size_t operator () (const std::unordered_set<std::string>& EDBs) const {
    std::hash<std::string> hasher;
    size_t seed = EDBs.size();
    for (auto& i : EDBs) {
      seed ^= hasher(i) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
  }
};


class CProvGraph {
public:
  /* initializer*/
  CProvGraph() {
  }

  CProvGraph(const std::string& sp): save_path(sp) {}

  /* read functions */
  inline vertex_t getVertexByName(const std::string& name) {
    return vertex_set[name];
  }

  inline std::string edgeToString(const edge_t e) {
    return std::to_string(e.m_source)+"_"+std::to_string(e.m_target);
  }

  inline vertex_t checkVertexExistByName(const std::string& name) {
    return vertex_set.find(name)!=vertex_set.end();
  }

  inline std::unordered_set<std::string> getVertexEBDs(vertex_t v) {
    return g[v].EDBs;
  }

  inline float getVertexDerivativeByName(const std::string& name) {
    return g[getVertexByName(name)].derivative;
  }

  inline std::unordered_set<std::string> getVertexEBDsByName(const std::string& name) {
    return g[getVertexByName(name)].EDBs;
  }

  inline float getVertexValueByName(const std::string& name) {
    return g[getVertexByName(name)].value;
  }


  /* write functions */
  inline vertex_t addVariableVertex(const VertexType vt, const std::string& name, float value);

  inline vertex_t addOperatorVertex(const VertexType vt, const std::string& name);

  inline vertex_t addOperatorVertex(const VertexType vt, const std::string& name, const std::unordered_map<std::string, std::string>& params);

  inline vertex_t addOperatorVertex(const VertexType vt, const std::string& name, const std::vector<double>& weights);

  inline vertex_t addOperatorVertex(const VertexType vt, const std::string& name, const std::unordered_map<std::string, std::string>& params, const std::vector<double>& weights);

  void addComputingSubgraph(const std::string& output_name, const float value, VertexType vt, const std::vector<std::string>& input_names);

  void addComputingSubgraph(const std::string& output_name, const float value, VertexType vt, const std::vector<std::string>& input_names, const std::unordered_map<std::string, std::string>& params);

  void addComputingSubgraph(const std::string& output_name, const float value, VertexType vt, const std::vector<std::string>& input_names, const std::vector<double>& weights);

  void addComputingSubgraph(const std::string& output_name, const float value, VertexType vt, const std::vector<std::string>& input_names, const std::unordered_map<std::string, std::string>& params, const std::vector<double>& weights, int number_of_op);

  inline void addProvEdge(vertex_t v1, vertex_t v2, bool reverse) {
    if (!reverse) boost::add_edge(v1, v2, g);
    else boost::add_edge(v2, v2, g);
  }

  inline void addProvEdge(vertex_t v1, vertex_t v2) {
    boost::add_edge(v1, v2, g);
  }

  inline void addProvEdge(vertex_t v1, vertex_t v2, float contribution) {
    edge_t e;
    bool b;
    boost::tie(e, b) = boost::add_edge(v1, v2, g);
    g[e].contribution = contribution;
  }

  inline void setVertexValue(vertex_t v, float value) {
    g[v].value = value;
  }

  inline void setVertexEDBs(vertex_t v, std::unordered_set<std::string>& EDBs) {
    g[v].EDBs = EDBs;
  }

  inline void setChangedEDBs(std::unordered_map<std::string, float>& m) {
    changedEDBs = m;
  }

  



  /* provenance query functions */
public:
  CProvGraph ProvenanceQuery(const std::string& name);

  void DFSProvQuery(vertex_t s, CProvGraph& subProvG);



 /* derivative functions */
public:
  void computeDerivative(const std::string& name) {
    ASSERT_EX(checkVertexExistByName(name), std::cout << name+" does not exist" << std::endl);
    vertex_t v = getVertexByName(name);

    // insert derivative of the source vertex
    g[v].derivative = 1.0;
    
    // std::cout << "start to compute derivative" << std::endl;
    std::unordered_set<std::string> visited;
    DFSComputeDerivative(v, visited);
  }

private: 
  void DFSComputeDerivative(vertex_t s, std::unordered_set<std::string>& visited);

  void DFSComputeSumDerivative(vertex_t s, float d, std::unordered_set<std::string>& visited);

  void DFSComputeMulDerivative(vertex_t s, float d, std::unordered_set<std::string>& visited);

  void DFSComputeDivDerivative(vertex_t s, float d, std::unordered_set<std::string>& visited);

  void DFSComputeScaleDerivative(vertex_t s, float d, std::unordered_set<std::string>& visited);

  void DFSComputeSoftmaxDerivative(vertex_t s, float d, std::unordered_set<std::string>& visited);

  void DFSComputeSigmoidDerivative(vertex_t s, float d, std::unordered_set<std::string>& visited);

  void DFSComputeInnerProductDerivative(vertex_t s, float d, std::unordered_set<std::string>& visited);

  void DFSComputeInnerProductActDerivative(vertex_t s, float d, std::unordered_set<std::string>& visited);

  

 /* contribution functions */
public:
  void computeContribution(const std::string& name);

  void computeContribution_v2(const std::string& name);

private:
  void DFSComputeContribution(vertex_t s, std::unordered_set<std::string>& visited);

  void DFSComputeSoftmaxContribution(vertex_t s, float s_value, float c, std::unordered_set<std::string>& visited);

  void DFSComputeInnerProductContribution(vertex_t s, float s_value, float c, std::unordered_set<std::string>& visited);

  void DFSComputeSigmoidContribution(vertex_t s, float s_value, float c, std::unordered_set<std::string>& visited);



  /* provenance-enabled model inference */
public: 
  float computeVariableWithChangedEDBs(const std::string& name, const std::unordered_map<std::string, float>& EDBs);

  float computeVariable(const std::string& name);

private:
  inline bool hasIntersection(const std::unordered_set<std::string>& l_set, const std::unordered_set<std::string>& r_set);

  inline bool hasIntersection(const std::unordered_map<std::string, float>& l_map, const std::unordered_set<std::string>& r_set);

  float DFSComputeVariable(vertex_t s, std::unordered_set<vertex_t>& visited);

  float DFSComputeVariableNoEDB(vertex_t s, std::unordered_set<vertex_t>& visited);

  inline float DFSComputeSum(vertex_t s, std::unordered_set<vertex_t>& visited);

  inline float DFSComputeMul(vertex_t s, std::unordered_set<vertex_t>& visited);

  inline float DFSComputeDiv(vertex_t s, std::unordered_set<vertex_t>& visited);

  inline float DFSComputeScale(vertex_t s, std::unordered_set<vertex_t>& visited);

  inline float DFSComputeSoftmax(vertex_t s, std::unordered_set<vertex_t>& visited);

  inline float DFSComputeSigmoid(vertex_t s, std::unordered_set<vertex_t>& visited);

  inline float DFSComputeInnerProduct(vertex_t s, std::unordered_set<vertex_t>& visited);




  /* provenance-enabled approximate subgraph query */
public:
  CProvGraph ApproximateSubGraphQuery(std::string& name, float epsilon, float lambda);

  void updateVertexValue(vertex_t s) {
    if (g[s].vt==Input || g[s].vt==Parameter) return;
    adjacency_tier ai, ai_end;
    boost::tie(ai, ai_end) = boost::adjacent_vertices(s, g);
    vertex_t v_operator = *ai;
    switch (g[v_operator].vt) {
      case(Sum): g[s].value = computeSum(v_operator); break;
      case(Mul): g[s].value = computeMul(v_operator); break;
      case(Div): g[s].value = computeDiv(v_operator); break;
      case(Scale): g[s].value = computeScale(v_operator); break;
      default: std::cout << "this is not an operator vertex\n"; exit(1);
    }
  }

  void updateVertexEDBs(vertex_t s) {
    std::unordered_set<std::string> ret;
    if (g[s].vt==Input) {
      ret.insert(g[s].name);
      g[s].EDBs = ret;
      return;
    }
    else if (g[s].vt==Parameter) {
      g[s].EDBs = ret;
      return;
    }
    adjacency_tier ai, ai_end;
    boost::tie(ai, ai_end) = boost::adjacent_vertices(s, g);
    vertex_t v_operator = *ai;
    for (boost::tie(ai, ai_end)=boost::adjacent_vertices(v_operator, g); ai!=ai_end; ai++) {
      vertex_t v = *ai;
      for (std::string EDB : g[v].EDBs) {
        ret.insert(EDB);
      }
    }
    g[s].EDBs = ret;
    g[v_operator].EDBs = ret;
  }


private:
  void DFSApproximateSubGraphQuery(vertex_t s, CProvGraph& approxSubProvG, std::unordered_set<std::string>& includedEDBs) {
    vertex_t parent = approxSubProvG.getVertexByName(g[s].name);
    adjacency_tier ai, ai_end;
    boost::tie(ai, ai_end) = boost::adjacent_vertices(s, g);
    vertex_t v_operator = *ai;
    vertex_t a_operator = approxSubProvG.addOperatorVertex(g[v_operator].vt, g[v_operator].name, g[v_operator].params);
    approxSubProvG.addProvEdge(parent, a_operator);
    for (boost::tie(ai, ai_end)=boost::adjacent_vertices(v_operator, g); ai!=ai_end; ai++) {
      vertex_t v = *ai;
      vertex_t child;
      if (approxSubProvG.checkVertexExistByName(g[v].name)) {
        child = approxSubProvG.getVertexByName(g[v].name);
        approxSubProvG.addProvEdge(a_operator, child);
      } 
      else {
        if (g[v].vt==Input) {
          if (includedEDBs.find(g[v].name)!=includedEDBs.end()) {
            child = approxSubProvG.addVariableVertex(Input, g[v].name, g[v].value);
            approxSubProvG.addProvEdge(a_operator, child);
            approxSubProvG.updateVertexEDBs(child);
          }
        }
        else if (g[v].vt==Parameter) {
          child = approxSubProvG.addVariableVertex(Parameter, g[v].name, g[v].value);
          approxSubProvG.addProvEdge(a_operator, child);
        }
        else {
          if (hasIntersection(g[v].EDBs, includedEDBs)) {
            child = approxSubProvG.addVariableVertex(Derived, g[v].name, 0);
            DFSApproximateSubGraphQuery(v, approxSubProvG, includedEDBs);
            approxSubProvG.addProvEdge(a_operator, child);
          }
        }
      }
    }
    approxSubProvG.updateVertexEDBs(parent);
    approxSubProvG.updateVertexValue(parent);
    return;
  }

  float computeSum(vertex_t s) {
    float ret = 0;
    adjacency_tier ai, ai_end;
    for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
      vertex_t v = *ai;
      ret += g[v].value;
    }
    return ret;
  }

  float computeMul(vertex_t s) {
    float ret = 1;
    adjacency_tier ai, ai_end;
    for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
      vertex_t v = *ai;
      ret *= g[v].value;
    }
    return ret;
  }

  float computeDiv(vertex_t s) {
    float ret = 1;
    float numerator = 0;
    float denominator = 1;
    adjacency_tier ai, ai_end;
    boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g);
    for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
      vertex_t v = *ai;
      if (g[v].name==g[s].params["numerator_name"]) {
        numerator = g[v].value;
      }
      else {
        denominator = 1.0/g[v].value;
      }
    }
    ret = numerator*denominator;
    return ret;
  }

  float computeScale(vertex_t s) {
    float sum = 0;
    float numerator = 0; 
    adjacency_tier ai, ai_end;
    for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
      vertex_t v = *ai;
      if (g[v].name==g[s].params["numerator_name"]) {
        numerator = g[v].value;
      }
      sum += g[v].value;
    }
    return numerator/sum;
  }

  float computeDerivativeDiff(std::unordered_map<std::string, float>& target_derivatives, std::unordered_map<std::string, float>& approx_derivatives) {
    float ret = 0;
    for (auto it : approx_derivatives) {
      std::string edb = it.first;
      ret += std::pow(std::abs(target_derivatives[edb]-it.second), 2);
    }
    return std::pow(ret, 0.5) / approx_derivatives.size();
  }


 /* provenance prune */
public:
  CProvGraph ApproximateSubGraphQueryPrune(std::string& name, float epsilon, float lambda);
  



 /* print and save functions */
public:
  inline void printVertex(vertex_t v) {
    std::cout << g[v].vt << " vertex ";
    std::cout << g[v].name << ", value=" << g[v].value << std::endl;
    std::cout << "EDBs: ";
    for (auto EDB : g[v].EDBs) {
      std::cout << EDB << " ";
    }
    std::cout << std::endl;
  }

  inline void printGraph() {
    write_graphviz (std::cout, g, my_node_writer(g));
  }

  inline void setSavePath(const std::string& sp) {
    save_path = sp;
  }

  inline void saveGraph() {
    std::ofstream fout(save_path);
    write_graphviz (fout, g, my_node_writer(g), my_edge_writer(g));
    fout.close();
  }

  ~CProvGraph() {};




 /* class members */
public:
 std::vector<std::vector<double> > auxilary_data;

private:
  Graph g;
  std::string save_path = "/home/jz598/MLNInfer/data/CProv/raw/test.dot";
  std::unordered_map<std::string, vertex_t> vertex_set;
  std::unordered_map<std::string, float> changedEDBs;
};

};

#endif