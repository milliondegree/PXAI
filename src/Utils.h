//============================================================================
// Name : Utils.h
// Author : David Nogueira
//============================================================================
#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <math.h>
#include <numeric>
#include <chrono>
#include <string>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <iterator>
#include <unordered_map>
#include <vector>
#include <cmath>
#include <functional>
#include <typeinfo>
#include <typeindex>
#include <cassert>

#include "CProvGraph.h"
#include "Chrono.h"
#ifdef _WIN32
#include <time.h>
#else
#include <sys/time.h>
#endif

namespace utils {
//Typical sigmoid function created from input x
//Returns the sigmoided value
inline double sigmoid(double x) {
  return 1 / (1 + exp(-x));
}

inline double inverse_sigmoid(double x) {
  return log(x / (1-x));
}

// Derivative of sigmoid function
inline double deriv_sigmoid(double x) {
  return sigmoid(x)*(1 - sigmoid(x));
};

//Compute hyperbolic tangent (tanh)
//Returns the hyperbolic tangent of x.
inline double hyperbolic_tan(double x) {
  return (tanh)(x);
}

// Derivative of hyperbolic tangent function
inline double deriv_hyperbolic_tan(double x) {
  return 1 - (std::pow)(hyperbolic_tan(x), 2);
};

inline double linear(double x) {
  return x;
}

// Derivative of linear function
inline double deriv_linear(double x) {
  return 1;
};

inline double vector_size(std::vector<double>& v) {
  double ret = 0;
  for (int i=0; i<v.size(); i++) ret += std::pow(v[i], 2);
  // return std::pow(ret, 0.5)/v.size();
  return std::pow(ret, 0.5);
}

struct ActivationFunctionsManager {
  bool GetActivationFunctionPair(const std::string & activation_name,
                                    std::pair<std::function<double(double)>,
                                    std::function<double(double)> > **pair) {
    auto iter = activation_functions_map.find(activation_name);
    if (iter != activation_functions_map.end())
      *pair = &(iter->second);
    else
      return false;
    return true;
  }

  static ActivationFunctionsManager & Singleton() {
    static ActivationFunctionsManager instance;
    return instance;
  }
private:
  void AddNewPair(std::string function_name,
                  std::function<double(double)> function,
                  std::function<double(double)> deriv_function) {
    activation_functions_map.insert(std::make_pair(function_name,
                                                   std::make_pair(function,
                                                                  deriv_function)));
  };

  ActivationFunctionsManager() {
    AddNewPair("sigmoid", sigmoid, deriv_sigmoid);
    AddNewPair("tanh", hyperbolic_tan, deriv_hyperbolic_tan);
    AddNewPair("linear", linear, deriv_linear);
  };

  std::unordered_map<std::string,
    std::pair<std::function<double(double)>, std::function<double(double)> > >
    activation_functions_map;
};

struct gen_rand {
  double factor;
  double offset;
public:
  gen_rand(double r = 2.0) : factor(r / RAND_MAX), offset(r / 2) {}
  double operator()() {
    return rand() * factor - offset;
  }
};

inline void Softmax(std::vector<double> *output) {
  size_t num_elements = output->size();
  std::vector<double> exp_output(num_elements);
  double exp_total = 0.0;
  for (size_t i = 0; i < num_elements; i++) {
    exp_output[i] = exp((*output)[i]);
    exp_total += exp_output[i];
  }
  for (size_t i = 0; i < num_elements; i++) {
    (*output)[i] = exp_output[i] / exp_total;
  }
}

inline void SoftmaxWithProv(std::vector<double> *output, std::vector<std::string>& input_names, cpg::CProvGraph& provG) {
  size_t num_elements = output->size();
  std::vector<double> exp_output(num_elements);
  double exp_total = 0.0;
  for (size_t i = 0; i < num_elements; i++) {
    exp_output[i] = exp((*output)[i]);
    exp_total += exp_output[i];
  }
  for (size_t i = 0; i < num_elements; i++) {
    std::string output_name = "softmax_"+std::to_string(i);
    (*output)[i] = exp_output[i] / exp_total;
    std::unordered_map<std::string, std::string> params = {{"numerator_pos", std::to_string(i)}};
    provG.addComputingSubgraph(output_name, (float)(*output)[i], cpg::Softmax, input_names, params);
  }
}

inline void  GetIdMaxElement(const std::vector<double> &output, size_t * class_id) {
  *class_id = std::distance(output.begin(),
                            std::max_element(output.begin(),
                                             output.end()));
}

inline float computeDerivativeDiff(std::unordered_map<std::string, float>& target_derivatives, std::unordered_map<std::string, float>& approx_derivatives) {
  float ret = 0;
  for (auto it : approx_derivatives) {
    std::string edb = it.first;
    ret += std::pow(std::abs(target_derivatives[edb]-it.second), 2);
  }
  return std::pow(ret, 0.5) / approx_derivatives.size();
}

inline float computeDerivativeDiff(std::vector<double>& target_derivatives, std::vector<double>& approx_derivatives) {
  float ret = 0;
  for (int i=0; i<approx_derivatives.size(); i++)
    ret += std::pow(std::abs(target_derivatives[i]-approx_derivatives[i]), 2);
  return std::pow(ret, 0.5) / approx_derivatives.size();
}

inline std::string vector_to_string(std::vector<double>& v) {
  std::string ret = "[";
  for (double d : v) {
    ret += std::to_string(d) + ", ";
  }
  ret += "]";
  return ret;
}

}
#endif // UTILS_H
