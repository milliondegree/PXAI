#include "CProvGraph.h"

using namespace cpg;

CProvGraph CProvGraph::ProvenanceQuery(const std::string& name) {
  ASSERT_EX(checkVertexExistByName(name), std::cout << name+" does not exist" << std::endl);
  vertex_t v = getVertexByName(name);
  
  // initialize CProvGraph of the queried vertex
  std::string new_save_path = save_path.substr(0, save_path.find_last_of("."));
  // std::cout << save_path << ' ' << new_save_path << std::endl;
  new_save_path += "-"+name+".dot";
  CProvGraph subProvG(new_save_path);

  // insert the source vertex to subProvG
  ASSERT_EX(g[v].vt==Derived, std::cout << g[v].name << " is a " << vertexTypeToString(g[v].vt) << " vertex" << std::endl);
  subProvG.addVariableVertex(g[v].vt, g[v].name, g[v].value);

  // std::cout << "start provenance query" << std::endl;
  DFSProvQuery(v, subProvG);
  return subProvG;
}

void CProvGraph::DFSProvQuery(vertex_t s, CProvGraph& subProvG) {
  vertex_t parent = subProvG.getVertexByName(g[s].name);
  if (g[s].vt==Input || g[s].vt==Parameter) {
    subProvG.updateVertexEDBs(parent);
    return;
  }
  // start iterating adjacent vertices
  adjacency_tier ai, ai_end;
  boost::tie(ai, ai_end) = boost::adjacent_vertices(s, g);
  vertex_t v_operator = *ai;
  vertex_t a_operator = subProvG.addOperatorVertex(g[v_operator].vt, g[v_operator].name, g[v_operator].params);
  subProvG.addProvEdge(parent, a_operator);
  for (boost::tie(ai, ai_end)=boost::adjacent_vertices(v_operator, g); ai!=ai_end; ai++) {
    vertex_t v = *ai;
    vertex_t child;
    if (subProvG.checkVertexExistByName(g[v].name)) {
      child = subProvG.getVertexByName(g[v].name);
      subProvG.addProvEdge(a_operator, child);
    }
    else {
      child = subProvG.addVariableVertex(g[v].vt, g[v].name, g[v].value);
      subProvG.addProvEdge(a_operator, child);
      DFSProvQuery(v, subProvG);
    }
  }
  subProvG.updateVertexEDBs(parent);
  return;
}

void CProvGraph::computeContribution(const std::string& name) {
  ASSERT_EX(checkVertexExistByName(name), std::cout << name+" does not exist" << std::endl);
  vertex_t v = getVertexByName(name);
  float previous_value = g[v].value;
  edge_iter ei_begin, ei_end, ei;
  boost::tie(ei_begin, ei_end) = boost::edges(g);
  std::vector<edge_t> edge_list;
  for (edge_iter ei=ei_begin; ei!=ei_end; ei++) edge_list.push_back(*ei);
  for (edge_t e : edge_list) {
    vertex_t v_source, v_target;
    v_source = boost::source(e, g);
    v_target = boost::target(e, g);
    if (g[v_source].vt!=Derived) {
      boost::remove_edge(v_source, v_target, g);
      float new_value = computeVariable(name);
      addProvEdge(v_source, v_target, previous_value-new_value);
    }
  }
}

void CProvGraph::computeContribution_v2(const std::string& name) {
  ASSERT_EX(checkVertexExistByName(name), std::cout << name+" does not exist" << std::endl);
  vertex_t v = getVertexByName(name);
  
  // insert derivative of the source vertex
  g[v].contribution = g[v].value;
  
  // std::cout << "start to compute derivative" << std::endl;
  std::unordered_set<std::string> visited;
  DFSComputeContribution(v, visited);
}

void CProvGraph::DFSComputeContribution(vertex_t s, std::unordered_set<std::string>& visited) {
  if (g[s].vt==Input || g[s].vt==Parameter) return;
  adjacency_tier ai, ai_end;
  boost::tie(ai, ai_end) = boost::adjacent_vertices(s, g);
  vertex_t v_operator = *ai;
  float c = g[s].contribution;
  switch (g[v_operator].vt) {
    case Sum: {
      adjacency_tier ai, ai_end;
      for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
        vertex_t v = *ai;
        edge_t e = boost::edge(s, v, g).first;
        g[e].contribution = g[v].value / g[s].value * c;
        visited.insert(edgeToString(e));
        bool all_visited = true;
        in_edge_iter ei, ei_end;
        for (boost::tie(ei, ei_end)=boost::in_edges(v, g); ei!=ei_end; ei++) {
          if (visited.find(edgeToString(*ei))==visited.end()) {
            all_visited = false;
            break;
          }
        }
        if (all_visited) {
          g[v].contribution = 0.0;
          for (boost::tie(ei, ei_end)=boost::in_edges(v, g); ei!=ei_end; ei++) 
            g[v].contribution += g[*ei].contribution;
          DFSComputeContribution(v, visited);
        }
      }
      break;
    }
    case Mul: {
      adjacency_tier ai, ai_end;
      for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
        vertex_t v = *ai;
        edge_t e = boost::edge(s, v, g).first;
        g[e].contribution = g[s].value / g[v].value ;
        visited.insert(edgeToString(e));
        bool all_visited = true;
        in_edge_iter ei, ei_end;
        for (boost::tie(ei, ei_end)=boost::in_edges(v, g); ei!=ei_end; ei++) {
          if (visited.find(edgeToString(*ei))==visited.end()) {
            all_visited = false;
            break;
          }
        }
        if (all_visited) {
          g[v].contribution = 0.0;
          for (boost::tie(ei, ei_end)=boost::in_edges(v, g); ei!=ei_end; ei++) 
            g[v].contribution += g[*ei].contribution;
          DFSComputeContribution(v, visited);
        }
      }
      break;
    }
    case Softmax: {
      
      break;
    }
    case Sigmoid: {
      adjacency_tier ai, ai_end;
      for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
        vertex_t v = *ai;
        edge_t e = boost::edge(s, v, g).first;
        g[e].contribution = c;
        visited.insert(edgeToString(e));
        bool all_visited = true;
        in_edge_iter ei, ei_end;
        for (boost::tie(ei, ei_end)=boost::in_edges(v, g); ei!=ei_end; ei++) {
          if (visited.find(edgeToString(*ei))==visited.end()) {
            all_visited = false;
            break;
          }
        }
        if (all_visited) {
          g[v].contribution = 0.0;
          for (boost::tie(ei, ei_end)=boost::in_edges(v, g); ei!=ei_end; ei++) 
            g[v].contribution += g[*ei].contribution;
          DFSComputeContribution(v, visited);
        }
      }
      break;
    }
    default: std::cout << "this is not an operator vertex\n"; exit(1);
  }
}

CProvGraph CProvGraph::ApproximateSubGraphQuery(std::string& name, float epsilon, float lambda) {
  ASSERT_EX(checkVertexExistByName(name), std::cout << name+" does not exist" << std::endl);
  vertex_t v = getVertexByName(name);
  ASSERT_EX(g[v].vt==Derived, std::cout << name << " is a " << vertexTypeToString(g[v].vt) << " vertex");
  float target = g[v].value;
  computeDerivative(name);
  std::unordered_map<std::string, float> target_derivatives;
  for (std::string edb : getVertexEBDsByName(name)) {
    target_derivatives[edb] = getVertexDerivativeByName(edb);
  }
  // std::cout << "target edb size: " << target_derivatives.size() << std::endl;
  
  // initialize CProvGraph of the the approximate query result
  std::string new_save_path = save_path.substr(0, save_path.find("."));
  new_save_path += "-approx.dot";

  std::priority_queue<ApproxSubGraphDiff, std::vector<ApproxSubGraphDiff>, ApproxSubGraphDiffComparison> pq;
  std::unordered_set<std::unordered_set<std::string>, EDBSetHash> visited;

  float min_diff = 1.0;
  float min_value_diff = 1.0;
  float min_derivative_diff = 1.0;
  std::unordered_set<std::string> min_set;
  std::unordered_set<std::string> includedEDBs;
  for (std::string EDB : g[v].EDBs) {
    std::unordered_set<std::string> includedEDBs;
    includedEDBs.insert(EDB);
    CProvGraph approxSubProvG(new_save_path);
    approxSubProvG.addVariableVertex(Derived, name, 0);
    DFSApproximateSubGraphQuery(v, approxSubProvG, includedEDBs);
    float value_diff = std::abs(target-approxSubProvG.getVertexValueByName(name));
    approxSubProvG.computeDerivative(name);
    // std::cout << "approx derivative finish" << std::endl;
    std::unordered_map<std::string, float> approx_derivatives;
    for (std::string edb : approxSubProvG.getVertexEBDsByName(name)) {
      approx_derivatives[edb] = approxSubProvG.getVertexDerivativeByName(edb);
    }
    // std::cout << "approx edb size: " << approx_derivatives.size() << std::endl;
    float derivative_diff = computeDerivativeDiff(target_derivatives, approx_derivatives);
    ApproxSubGraphDiff asgd(includedEDBs, value_diff, derivative_diff);
    if (asgd.getDiff()<min_diff) {
      min_set = asgd.EDBs;
      min_diff = asgd.getDiff();
      min_value_diff = asgd.value_diff;
      min_derivative_diff = asgd.derivative_diff;
    }
    if (min_value_diff<epsilon && min_derivative_diff<lambda) {
      return approxSubProvG;
    }
    pq.push(asgd);
    visited.insert(includedEDBs);
  }

  int iteration = 1;
  while (!pq.empty() && visited.size()<MAX_NUM_OF_SEARCH) {
    ApproxSubGraphDiff asgd = pq.top();
    pq.pop();
    if (asgd.getDiff()<min_diff) {
      min_set = asgd.EDBs;
      min_diff = asgd.getDiff();
      min_value_diff = asgd.value_diff;
      min_derivative_diff = asgd.derivative_diff;
    }
    if (min_value_diff<epsilon && min_derivative_diff<lambda) {
      break;
    }
    // search neighbors of asgd.EDBs
    std::unordered_set<std::string> includedEDBs = asgd.EDBs;
    for (std::string EDB : g[v].EDBs) {
      if (includedEDBs.find(EDB)==includedEDBs.end()) {
        includedEDBs.insert(EDB);
        if (visited.find(includedEDBs)!=visited.end()) continue;
        CProvGraph approxSubProvG(new_save_path);
        approxSubProvG.addVariableVertex(Derived, name, 0);
        DFSApproximateSubGraphQuery(v, approxSubProvG, includedEDBs);
        float value_diff = std::abs(target-approxSubProvG.getVertexValueByName(name));
        approxSubProvG.computeDerivative(name);
        std::unordered_map<std::string, float> approx_derivatives;
        for (std::string edb : approxSubProvG.getVertexEBDsByName(name)) {
          approx_derivatives[edb] = approxSubProvG.getVertexDerivativeByName(edb);
        }
        float derivative_diff = computeDerivativeDiff(target_derivatives, approx_derivatives);
        ApproxSubGraphDiff new_asgd(includedEDBs, value_diff, derivative_diff);
        pq.push(new_asgd);
        visited.insert(includedEDBs);
        includedEDBs.erase(EDB);
      }
    }
    iteration++;
  }

  // std::cout << "iteration: " << iteration << ", min value diff: " << min_value_diff << ", min derivative diff: " << min_derivative_diff << std::endl;
  CProvGraph approxSubProvG(new_save_path);
  approxSubProvG.addVariableVertex(Derived, name, 0);
  DFSApproximateSubGraphQuery(v, approxSubProvG, min_set);
  return approxSubProvG;
}

CProvGraph CProvGraph::ApproximateSubGraphQueryPrune(std::string& name, float epsilon, float lambda) {
  computeContribution(name);
  computeDerivative(name);

  vertex_t v = getVertexByName(name);
  float target = g[v].value;
  std::unordered_map<std::string, float> target_derivatives;
  for (std::string edb : getVertexEBDsByName(name)) {
    target_derivatives[edb] = getVertexDerivativeByName(edb);
  }

  edge_iter ei_begin, ei_end, ei;
  boost::tie(ei_begin, ei_end) = boost::edges(g);
  std::vector<edge_t> edge_list;
  for (edge_iter ei=ei_begin; ei!=ei_end; ei++) {
    vertex_t v_source = boost::source(*ei, g);
    vertex_t v_target = boost::target(*ei, g);
    if (g[v_source].vt!=Derived) {
      // std::cout << abs(g[*ei].contribution) << ' ' << std::abs(g[*ei].derivative) << std::endl;
      g[*ei].importance = ALPHA*std::abs(g[*ei].contribution)+BETA*std::abs(g[*ei].derivative);
      edge_list.push_back(*ei);
    }
  }
  edge_comparer ec(g);
  std::sort(edge_list.begin(), edge_list.end(), ec);
  int count = 0;
  // int step = edge_list.size()/15;
  int step = 1;
  std::cout << "number of edges: " << edge_list.size() << ", prune step: " << step << std::endl;
  CProvGraph ret = *this;
  for (edge_t e : edge_list) {
    vertex_t v_source = boost::source(e, g);
    vertex_t v_target = boost::target(e, g);
    boost::remove_edge(v_source, v_target, g);
    count += 1;
    if (count%step==0) {
      CProvGraph approxSubProvG = ProvenanceQuery(name);
      float approxiValue = approxSubProvG.computeVariable(name);
      float value_diff = std::abs(target-approxSubProvG.getVertexValueByName(name));
      approxSubProvG.computeDerivative(name);
      std::unordered_map<std::string, float> approx_derivatives;
      for (std::string edb : approxSubProvG.getVertexEBDsByName(name)) {
        approx_derivatives[edb] = approxSubProvG.getVertexDerivativeByName(edb);
      }
      float derivative_diff = computeDerivativeDiff(target_derivatives, approx_derivatives);
      // std::cout << "value diff: " << value_diff << ", derivative diff: " << derivative_diff << std::endl;
      if (value_diff>epsilon||derivative_diff>lambda) {
        std::cout << "value diff: " << value_diff << ", derivative diff: " << derivative_diff << std::endl;
        break;
      }
      ret = approxSubProvG;
    }
  }
  std::string new_save_path = save_path.substr(0, save_path.find_last_of("."));
  new_save_path += "-approx.dot";
  ret.setSavePath(new_save_path);
  std::cout << "pruned edges: " << count << std::endl;
  return ret;
}

float CProvGraph::DFSComputeVariable(vertex_t s, std::unordered_set<vertex_t>& visited) {
  // std::cout << "entering " << g[s].name << std::endl;
  if (visited.find(s)!=visited.end()) return g[s].value;
  if (g[s].vt==Input) {
    if (changedEDBs.find(g[s].name)!=changedEDBs.end()) {
      // std::cout << "find changed edb: " << g[s].name << ", previous value: " << g[s].value << ", changed value: " << changedEDBs[g[s].name] << std::endl;
      g[s].value = changedEDBs[g[s].name];
    }
    visited.insert(s);
    return g[s].value;
  }
  else if (g[s].vt==Parameter) {
    visited.insert(s);
    return g[s].value;
  }
  else if (!hasIntersection(changedEDBs, g[s].EDBs)) {
    visited.insert(s);
    return g[s].value;
  }
  // std::cout << "first time " << g[s].name << std::endl;
  // if the current vertex is a derived veriable that depends on changed inputs
  float ret;
  adjacency_tier ai, ai_end;
  boost::tie(ai, ai_end) = boost::adjacent_vertices(s, g);
  vertex_t v_operator = *ai;
  if (boost::out_degree(v_operator, g)==0) {
    g[s].value = 0;
    return 0;
  }
  switch (g[v_operator].vt) {
    case Sum: ret = DFSComputeSum(v_operator, visited); break;
    case Mul: ret = DFSComputeMul(v_operator, visited); break;
    case Div: ret = DFSComputeDiv(v_operator, visited); break;
    case Scale: ret = DFSComputeScale(v_operator, visited); break;
    case Softmax: ret = DFSComputeSoftmax(v_operator, visited); break;
    case Sigmoid: ret = DFSComputeSigmoid(v_operator, visited); break;
    default: std::cout << "this is not an operator vertex\n"; exit(1);
  }
  // std::cout << "finish " << g[s].name << std::endl;
  g[s].value = ret;
  visited.insert(s);
  return ret;
}

inline float CProvGraph::DFSComputeSum(vertex_t s, std::unordered_set<vertex_t>& visited) {
  float ret = 0;
  adjacency_tier ai, ai_end;
  boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g);
  for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
    vertex_t v = *ai;
    ret += DFSComputeVariable(v, visited);
  }
  return ret;
}

inline float CProvGraph::DFSComputeMul(vertex_t s, std::unordered_set<vertex_t>& visited) {
  float ret = 1;
  adjacency_tier ai, ai_end;
  for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
    vertex_t v = *ai;
    ret *= DFSComputeVariable(v, visited);
  }
  return ret;
}

inline float CProvGraph::DFSComputeDiv(vertex_t s, std::unordered_set<vertex_t>& visited) {
  float numerator=0, denominator=0;
  adjacency_tier ai, ai_end;
  for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
    vertex_t v = *ai;
    float tmp = DFSComputeVariable(v, visited);
    if (g[v].name==g[s].params["numerator_name"]) {
      numerator = tmp;
    }
    else {
      denominator = tmp;
    }
  }
  if (denominator==0) return 0;
  return numerator/denominator;
}

inline float CProvGraph::DFSComputeScale(vertex_t s, std::unordered_set<vertex_t>& visited) {
  float denominator = 0;
  float numerator = 0; 
  adjacency_tier ai, ai_end;
  for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
    vertex_t v = *ai;
    float tmp = DFSComputeVariable(v, visited);
    if (g[v].name==g[s].params["numerator_name"]) {
      numerator = tmp;
    }
    denominator += tmp;
  }
  if (denominator==0) return 0;
  return numerator/denominator;
}

inline float CProvGraph::DFSComputeSoftmax(vertex_t s, std::unordered_set<vertex_t>& visited) {
  float denominator = 0;
  float numerator = 0; 
  adjacency_tier ai, ai_end;
  for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
    vertex_t v = *ai;
    float tmp = DFSComputeVariable(v, visited);
    if (g[v].name==g[s].params["numerator_name"]) {
      numerator = std::exp(tmp);
    }
    denominator += std::exp(tmp);
  }
  if (denominator==0) return 0;
  return numerator/denominator;
}

inline float CProvGraph::DFSComputeSigmoid(vertex_t s, std::unordered_set<vertex_t>& visited) {
  float ret = 0;
  adjacency_tier ai, ai_end;
  for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
    vertex_t v = *ai;
    float value = DFSComputeVariable(v, visited);
    ret += 1/(1+std::exp(-value));
  }
  return ret;
} 

float CProvGraph::DFSComputeVariableNoEDB(vertex_t s, std::unordered_set<vertex_t>& visited) {
  if (visited.find(s)!=visited.end()) return g[s].value;
  if (g[s].vt==Input) {
    visited.insert(s);
    return g[s].value;
  }
  else if (g[s].vt==Parameter) {
    visited.insert(s);
    return g[s].value;
  }
  // if the current vertex is a derived veriable that depends on changed inputs
  float ret;
  adjacency_tier ai, ai_end;
  boost::tie(ai, ai_end) = boost::adjacent_vertices(s, g);
  vertex_t v_operator = *ai;
  if (boost::out_degree(v_operator, g)==0) {
    g[s].value = 0.0/0.0;
    return g[s].value;
  }
  int nan_cnt = 0;
  switch (g[v_operator].vt) {
    case Sum: {
      ret = 0;
      adjacency_tier ai, ai_end;
      for (boost::tie(ai, ai_end)=boost::adjacent_vertices(v_operator, g); ai!=ai_end; ai++) {
        vertex_t v = *ai;
        float value = DFSComputeVariableNoEDB(v, visited);
        if (!std::isnan(value)) ret += DFSComputeVariableNoEDB(v, visited);
        else nan_cnt++;
      }
      break;
    }
    case Mul: {
      ret = 1;
      adjacency_tier ai, ai_end;
      for (boost::tie(ai, ai_end)=boost::adjacent_vertices(v_operator, g); ai!=ai_end; ai++) {
        vertex_t v = *ai;
        float value = DFSComputeVariableNoEDB(v, visited);
        if (!std::isnan(value)) ret *= DFSComputeVariableNoEDB(v, visited);
        else nan_cnt++;
      }
      break;
    }
    case Div: {
      float numerator = 0, denominator = 1;
      adjacency_tier ai, ai_end;
      for (boost::tie(ai, ai_end)=boost::adjacent_vertices(v_operator, g); ai!=ai_end; ai++) {
        vertex_t v = *ai;
        if (g[v].name==g[v_operator].params["numerator_name"]) {
          numerator = DFSComputeVariableNoEDB(v, visited);
        }
        else {
          denominator = DFSComputeVariableNoEDB(v, visited);
        }
      }
      if (denominator==0) ret = 0;
      else ret = numerator/denominator;
      break;
    }
    case Scale: {
      float denominator = 0, numerator = 0; 
      adjacency_tier ai, ai_end;
      for (boost::tie(ai, ai_end)=boost::adjacent_vertices(v_operator, g); ai!=ai_end; ai++) {
        vertex_t v = *ai;
        float tmp = DFSComputeVariableNoEDB(v, visited);
        if (g[v].name==g[v_operator].params["numerator_name"]) {
          numerator = tmp;
        }
        denominator += tmp;
      }
      if (denominator==0) ret = 0;
      else ret = numerator/denominator;
      break;
    }
    case Softmax: {
      float denominator = 0, numerator = 0; 
      adjacency_tier ai, ai_end;
      for (boost::tie(ai, ai_end)=boost::adjacent_vertices(v_operator, g); ai!=ai_end; ai++) {
        vertex_t v = *ai;
        float tmp = DFSComputeVariableNoEDB(v, visited);
        if (!std::isnan(tmp)) {
          if (g[v].name==g[v_operator].params["numerator_name"]) {
            numerator = std::exp(tmp);
          }
          denominator += std::exp(tmp);
        }
        else nan_cnt++;
      }
      if (denominator==0) ret = 0;
      else ret = numerator/denominator;
      break;
    }
    case Sigmoid: {
      ret = 0;
      adjacency_tier ai, ai_end;
      for (boost::tie(ai, ai_end)=boost::adjacent_vertices(v_operator, g); ai!=ai_end; ai++) {
        vertex_t v = *ai;
        float value = DFSComputeVariableNoEDB(v, visited);
        if (!std::isnan(value)) ret += 1.0/(1+std::exp(-value));
        else nan_cnt++;
      }
      break;
    }
    default: std::cout << "this is not an operator vertex\n"; exit(1);
  }
  if (boost::out_degree(v_operator, g)==nan_cnt) ret = 0.0/0.0;
  g[s].value = ret;
  visited.insert(s);
  return ret;
}
