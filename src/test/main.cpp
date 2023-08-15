
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>
#include <math.h>
#include "../Load.h"
#include "../MLN.h"
#include "../Clique.h"
#include "../Optimizer.h"
using namespace std;



bool sortByValue(const pair<string, double>& p1, const pair<string, double>& p2) {
  return fabs(p1.second) > fabs(p2.second);
}



double calcMean(vector<double>& values) {
  double mean = 0;
  for (int i=0; i<values.size(); i++) {
    mean += values[i];
  }
  mean /= values.size();
  return mean;
}


double calcVar(vector<double>& values) {
  double mean = 0;
  for (int i=0; i<values.size(); i++) {
    mean += values[i];
  }
  mean /= values.size();
  double var = 0;
  for (int i=0; i<values.size(); i++) {
    var += pow((values[i]-mean), 2);
  }
  var /= values.size();
  return var;
}



void printLiterals(MLN mln) {
  // print out the observed literals and unknown literals after parsing
  unordered_set<string> obs = mln.getObsLiterals();
  unordered_set<string> queries = mln.getQueryLiterals();
  cout << "observed tuples: ";
  for (string s : obs) {
    cout << s << ' ';
  }
  cout << endl;
  cout << "unknown tuples: ";
  for (string s : queries) {
    cout << s << ' ';
  }
  cout << endl;
  cout << "size of observed tuples: " << obs.size() << endl;
  cout << "size of unobserved tuples: " << queries.size() << endl;
  cout << endl;
  for (Clique c : mln.getCliques()) {
    cout << c.toString() << endl;
  }
  cout << endl;
}


void printMLNStatistic(MLN& mln) {
  cout << "observed tuple number: " << mln.obs.size() << endl;
  cout << "unobserved tuple number: " << mln.queries.size() << endl;
  cout << "clique number: " << mln.cliques.size() << endl;
}




void probabilityQuery(MLN& mln, int round, string query_name, string mode, double delta) {
  clock_t t1 = clock();
  if (mode=="gibbs") {
    mln.gibbsSampling(round);
  }
  else if (mode=="lbp") {
    mln.loopyBeliefPropagation(query_name);
  }
  else if (mode=="mclbp") {
    mln.loopyBeliefPropagationMCS(query_name, round);
  }
  clock_t t2 = clock();
}



void influenceQuery(MLN& mln, string query, string infl, int round, double delta, string mode) {
  clock_t t1, t2;
  double infValue;
  t1 = clock();
  Grader grader;
  grader.computeGradient(mln, query, infl, round, delta, mode);
  infValue = mln.getInfluence(query)[infl];
  t2 = clock();
  // cout << "mode " << mode << ": influence compute time (" << mode << "): " << ((double)(t2-t1))/CLOCKS_PER_SEC << " seconds" << endl;
  // cout << "influence of " << infl << " on " << query << " is " << infValue << endl;
  // cout << endl;
}


void influenceQueryAll(MLN& mln, string query, int round, double delta, string mode) {
  Grader grader;
  // unordered_set<string> observedTuples = grader.getValidObservedTuples(mln, query);
  unordered_set<string> observedTuples = mln.getObsLiterals();
  clock_t t1 = clock();
  // cout << "mode " << mode << endl;
  vector<pair<string, double> > vp;
  for (string observed : observedTuples) {
    if (Parser::isRuleName(observed)) continue;
    grader.computeGradient(mln, query, observed, round, delta, mode);
    double influs = mln.getInfluence(query)[observed];
    vp.push_back(pair<string, double>({observed, influs}));
  }
  sort(vp.begin(), vp.end(), sortByValue);
}


Optimizer* initializeOptimizer(int optimizer_name, double update_rate) {
  Optimizer* ret;
  switch (optimizer_name) {
    case 1:
      ret = new GradientDescent(update_rate);
      break;
    case 2:
      ret = new Momentum(update_rate);
      break;
    case 3:
      ret = new Nesterov(update_rate);
      break;
    case 4:
      ret = new Adam(update_rate);
      break;
    case 5:
      ret = new LogicalOptimizer(update_rate);
      break;
    default:
      ret = new GradientDescent(update_rate);
      break;
  } 
  return ret;
}

Regulation initializeRegulation(string reg_name) {
  if (reg_name=="l1") {
    return l1;
  }
  else if (reg_name=="l2") {
    return l2;
  }
  return none;
}

bool chechHighest(MLN& mln, string target, vector<string>& query_names) {
  string highest = target;
  double highest_value = 0;
  for (string literal : query_names) {
    if (mln.prob[literal]>highest_value) {
      highest = literal;
      highest_value = mln.prob[literal];
    }
  }
  return highest==target;
}


int main(int argc, char* argv[]) {

  unordered_map<string, string> args;
  vector<string> argvs;
  for (int i=0; i<argc; i++) {
    argvs.push_back(string(argv[i]));
  }
  for (int i=1; i<argc; i+=2) {
    assert(i+1<argc);
    if (argvs[i]=="-q" || argvs[i]=="-query_name") {
      args["query_name"] = argvs[i+1];
    }
    else if (argvs[i]=="-t" || argvs[i]=="-target_name") {
      args["target_name"] = argvs[i+1];
    }
    else if (argvs[i]=="-o" || argvs[i]=="-observe_file") {
      args["observe_file"] = argvs[i+1];
    }
    else if (argvs[i]=="-p" || argvs[i]=="-provenance_file") {
      args["provenance_file"] = argvs[i+1];
    }
    else if (argvs[i]=="-s" || argvs[i]=="-save") {
      args["save"] = argvs[i+1];
    }
    else if (argvs[i]=="-i" || argvs[i]=="-influence_name") {
      args["influence_name"] = argvs[i+1];
    }
    else if (argvs[i]=="-r" || argvs[i]=="-round") {
      args["round"] = argvs[i+1];
    }
    else if (argvs[i]=="-a") {
      args["approx"] = argvs[i+1];
    }
    else if (argvs[i]=="-d" || argvs[i]=="-delta") {
      args["delta"] = argvs[i+1];
    }
    else if (argvs[i]=="-m" || argvs[i]=="-mode") {
      args["mode"] = argvs[i+1];
    }
    else if (argvs[i]=="-cn" || argvs[i]=="-clique_number") {
      args["clique_number"] = argvs[i+1];
    }
    else if (argvs[i]=="-im" || argvs[i]=="-influ_mode") {
      args["influ_mode"] = argvs[i+1];
    }
    else if (argvs[i]=="-op" || argvs[i]=="-optimize") {
      args["optimize"] = argvs[i+1];
    }
    else if (argvs[i]=="-update_rate") {
      args["update_rate"] = argvs[i+1];
    }
    else if (argvs[i]=="-regularization") {
      args["regularization"] = argvs[i+1];
    }
    else if (argvs[i]=="-optimizer") {
      args["optimizer"] = argvs[i+1];
    }
    else if (argvs[i]=="-l") {
      args["norm"] = argvs[i+1];
    }
    else if (argvs[i]=="-rule_name") {
      args["rule_name"] = argvs[i+1];
    }
  }

  if (args.find("provenance_file")==args.end() || args.find("observe_file")==args.end() || 
      (args.find("query_name")!=args.end() && args.find("mode")==args.end()) ) {
    cout << "missing input of provenance and observe" << endl;
    exit(0);
  }

  if (args.find("influ_mode")==args.end()) {
    args["influ_mode"] = args["mode"];
  }

  Load l(args["provenance_file"], args["observe_file"]);
  vector<string> prov = l.getProv();

  MLN mln(l);
  Parser parser = Parser(1);
  parser.parseProvenance(mln);
  // mln.merge();

  cout << "original MLN: " << endl;
  printMLNStatistic(mln);
  // cout << mln.toString() << endl;
  cout << endl;

  if (args.find("query_name")==args.end()) {
    vector<double> graph_time;
    vector<double> prob_time;
    vector<double> approx_time;
    vector<double> influ_time;
    vector<double> counter_time;
    vector<double> clique_number;
    int count = 0;
    for (string query_name : mln.queries) {
      clock_t t1 = clock();
      MLN mmln = mln.getMinimalMLN(query_name);
      if (mmln.cliques.size()<100) {
        // count++;
        continue;
      }
      graph_time.push_back(clock()-t1);
      
      t1 = clock();
      probabilityQuery(mmln, stoi(args["round"]), query_name, args["mode"], 0.1);
      double target = target = mmln.prob[query_name];
      prob_time.push_back(clock()-t1);

      t1 = clock();
      MLN amln = mmln.approximateSubGraph(query_name, target, stoi(args["round"]), stod(args["approx"]));
      // MLN amln = mmln.approximateSubGraph_v2(query_name, target, stoi(args["round"]), stod(args["approx"]), stoi(args["clique_number"]));
      approx_time.push_back(clock()-t1);
      if (amln.cliques.size()==30) {
        cout << query_name << ' ' << mmln.prob[query_name] << endl;
      }
      clique_number.push_back(amln.cliques.size());
      // cout << amln.toString() << endl;
      // MLN amln = mmln;

      t1 = clock();
      for (string literal : amln.obs) {
        if (Parser::isRuleName(literal) && stoi(args["rule_name"])>0) {
          influenceQuery(amln, query_name, literal, 10000, stod(args["delta"]), "gibbs");
        }
        else if (!Parser::isRuleName(literal)) {
          influenceQuery(amln, query_name, literal, stoi(args["round"]), 1.0, args["influ_mode"]);
        }
      }
      influ_time.push_back(clock()-t1);

      probabilityQuery(amln, stoi(args["round"]), query_name, args["mode"], 0.1);

      t1 = clock();
      vector<string> query_names ({query_name});
      for (string q : amln.queries) {
        if (q!=query_name && amln.prob[q]>amln.prob[query_name]) {
          query_names.push_back(q);
          break;
        } 
      }
      Optimizer* optimizer = initializeOptimizer(stoi(args["optimizer"]), stod(args["update_rate"]));
      Regulation regulation = initializeRegulation(args["norm"]);
      map<string, double> origin_probs = amln.prob;
      int round = 1;
      do {
        // compute influence
        for (int i=0; i<query_names.size(); i++) {
          string query_name = query_names[i];
          unordered_set<string> obs = amln.obs;
          for (string literal : obs) {
            if (Parser::isRuleName(literal) && stoi(args["rule_name"])>0) {
              influenceQuery(amln, query_name, literal, 10000, stod(args["delta"]), "gibbs");
            }
            else if (!Parser::isRuleName(literal)) {
              influenceQuery(amln, query_name, literal, stoi(args["round"]), 1.0, args["influ_mode"]);
            }
          }
        }
        // compute gradients of softmax loss
        map<string, double> gradients = amln.computeGradients(query_name, query_names, origin_probs, regulation, stod(args["regularization"]), stoi(args["rule_name"]));
        vector<pair<string, double> > gradient_vector;
        for (auto it : gradients) gradient_vector.push_back(it);
        sort(gradient_vector.begin(), gradient_vector.end(), sortByValue);
        // update parameters
        optimizer->update(amln, gradient_vector);
        amln.loopyBeliefPropagationMCS(query_name, stoi(args["round"]));
        // double loss = amln.computeCrossEntropyLoss(query_name, query_names, origin_probs, regulation, stod(args["regularization"]));
        // termination check
        if (
          chechHighest(amln, query_name, query_names) || 
          round>=10 
          ) {
          break;
        }
        round++;
      } while (true);
      counter_time.push_back(clock()-t1);

      count++;
      if (count>min((int)mln.prob.size(), 100)) break;
    }
    // for (int cn : clique_number) {
    //   cout << cn << ' ';
    //   cout << endl;
    // }
    cout << "avg graph time: " << calcMean(graph_time)/CLOCKS_PER_SEC << endl;
    cout << "avg prob time: " << calcMean(prob_time)/CLOCKS_PER_SEC << endl;
    cout << "avg approx time: " << calcMean(approx_time)/CLOCKS_PER_SEC << endl;
    cout << "avg clique number: " << calcMean(clique_number) << endl;
    cout << "avg influ time: " << calcMean(influ_time)/CLOCKS_PER_SEC << endl;
    cout << "avg counter time: " << calcMean(counter_time)/CLOCKS_PER_SEC << endl;
  }
  

  if (args.find("query_name")!=args.end() && args.find("mode")!=args.end()) {
    vector<string> query_names;
    boost::split(query_names, args["query_name"], boost::is_any_of(",|"));
    vector<MLN> MLNs;

    for (string query_name : query_names) {
      cout << "build MLN for " << query_name << endl;
      MLN mmln = mln.getMinimalMLN(query_name);
      cout << "minumum MLN: " << endl;
      printMLNStatistic(mmln);
      cout << mmln.toString() << endl;
      cout << endl;

      // probability inference
      double target;
      for (int i=0; i<1; i++) {
        clock_t t1 = clock();
        probabilityQuery(mmln, stoi(args["round"]), query_name, args["mode"], 0.1);
        clock_t t2 = clock();
        target = mmln.prob[query_name];
        cout << "probability query time: " << (t2-t1)*1.0/CLOCKS_PER_SEC << ", value of " << query_name << ": " << target << endl;
        cout << endl;
      }
      
      // approximate subgraph
      if (args.find("approx")!=args.end()) {
        clock_t t1 = clock();
        cout << "start approximate subgraph, target value: " << target << ", approx value: " << args["approx"] << ", rounds: " << args["round"] << endl;
        // MLN amln = mmln.approximateSubGraph_v3(query_name, target, stoi(args["round"]), stod(args["approx"]), stoi(args["clique_number"]));
        // MLN amln = mmln.approximateSubGraph_v2(query_name, target, stoi(args["round"]), stod(args["approx"]), stoi(args["clique_number"]));
        MLN amln = mmln.approximateSubGraph(query_name, target, stoi(args["round"]), stod(args["approx"]), stoi(args["clique_number"]));
        cout << "total time: " << (clock()-t1)*1.0/CLOCKS_PER_SEC << endl << endl;

        cout << "approxmimate subgraph: " << endl;
        // cout << amln.toString() << endl;
        printMLNStatistic(amln);
        cout << endl;

        probabilityQuery(amln, stoi(args["round"]), query_name, args["mode"], 0.1);
        cout << "final diff: " << fabs(amln.queryProb(query_name)-target) << endl << endl;

        mmln = amln;
      }
      MLNs.push_back(mmln);
    }
    exit(0);

    // graph combination
    MLN mmln = MLNs[0];
    for (int i=1; i<MLNs.size(); i++) {
      mmln = mmln+MLNs[i];
    }
    cout << "combined graph: " << endl;
    // cout << mmln.toString() << endl;
    printMLNStatistic(mmln);

    for (string query_name : query_names) {
      probabilityQuery(mmln, stoi(args["round"]), query_name, args["mode"], 0.1);
      cout << "probability of " << query_name << ": " << mmln.queryProb(query_name) << endl;
    }
    cout << endl;

    // influence inference
    clock_t t1 = clock();
    if (args.find("influence_name")!=args.end()) {
      for (int i=0; i<query_names.size(); i++) {
        string query_name = query_names[i];
        if (args["influence_name"]=="all") {
          unordered_set<string> obs = MLNs[i].obs;
          vector<pair<string, double> > vp;
          clock_t t1 = clock();
          for (string literal : obs) {
            if (Parser::isRuleName(literal) && stoi(args["rule_name"])>0) {
              influenceQuery(mmln, query_name, literal, 10000, stod(args["delta"]), "gibbs");
              vp.push_back(pair<string, double>({literal, mmln.getInfluence(query_name)[literal]}));
            }
            else if (!Parser::isRuleName(literal)) {
              // continue;
              influenceQuery(mmln, query_name, literal, stoi(args["round"]), 1.0, args["influ_mode"]);
              vp.push_back(pair<string, double>({literal, mmln.getInfluence(query_name)[literal]}));
            }
          }
          cout << "influence query time of " << query_name << ": " << (clock()-t1)*1.0/CLOCKS_PER_SEC << endl; 
          sort(vp.begin(), vp.end(), sortByValue);
          cout << "sorted influences:" << endl;
          for (auto it=vp.begin(); it<vp.end(); it++) {
            cout << "influence of " << it->first << " on " << query_name << " is " << it->second << endl;
          }
        }
        else {
          influenceQuery(mmln, query_name, args["influence_name"], stoi(args["round"]), stod(args["delta"]), args["influ_mode"]);
        }
      }
    }
    cout << "infl time " << (clock()-t1)*1.0/CLOCKS_PER_SEC << endl;
    cout << endl;

    // counterfactual inference
    t1 = clock();
    if (args.find("optimize")!=args.end()) {
      Optimizer* optimizer = initializeOptimizer(stoi(args["optimizer"]), stod(args["update_rate"]));

      if (query_names.size()>1) {
        int round = 1;
        Regulation regulation = initializeRegulation(args["norm"]);
        map<string, double> origin_probs = mmln.prob;
        double prev = 100;

        do {
          // compute influence
          for (int i=0; i<query_names.size(); i++) {
            string query_name = query_names[i];
            unordered_set<string> obs = MLNs[i].obs;
            for (string literal : obs) {
              if (Parser::isRuleName(literal) && stoi(args["rule_name"])>0) {
                influenceQuery(mmln, query_name, literal, stoi(args["round"]), stod(args["delta"]), args["influ_mode"]);
              }
              else if (!Parser::isRuleName(literal)) {
                influenceQuery(mmln, query_name, literal, stoi(args["round"]), 1.0, args["influ_mode"]);
              }
            }
          }
          // compute gradients of softmax loss
          map<string, double> gradients = mmln.computeGradients(args["optimize"], query_names, origin_probs, regulation, stod(args["regularization"]), stoi(args["rule_name"]));
          vector<pair<string, double> > gradient_vector;
          for (auto it : gradients) gradient_vector.push_back(it);
          sort(gradient_vector.begin(), gradient_vector.end(), sortByValue);
          // for (auto it : gradient_vector) {
          //   cout << "gradient of " << it.first << " is " << it.second << endl;
          // }
          // update parameters
          optimizer->update(mmln, gradient_vector);
          mmln.loopyBeliefPropagationMCS(args["optimize"], stoi(args["round"]));
          for (string query_name : query_names) {
            cout << "new probability of " << query_name << ": " << mmln.queryProb(query_name) << endl;
          }
          double loss = mmln.computeCrossEntropyLoss(args["optimize"], query_names, origin_probs, regulation, stod(args["regularization"]));
          cout << "round: " << round << ", loss of " << args["optimize"] << ": " << loss << endl;
          // termination check
          if (
            chechHighest(mmln, args["optimize"], query_names) || 
            round>=10 
            // (round>10 && loss>=prev)
            ) {
            break;
          }
          prev = loss;
          round++;
          cout << endl;
        } while (true);
        cout << endl;

        vector<pair<string, double> > diff;
        for (string literal : mmln.obs) {
          if (Parser::isRuleName(literal) && stoi(args["rule_name"])==0) continue;
          diff.push_back(pair<string, double>({literal, mmln.prob[literal]-origin_probs[literal]}));
          cout << "counterfactual " << literal << ": " << mmln.prob[literal] << endl;
        }
        cout << endl;

        cout << "diff of counterfactual: " << endl;
        sort(diff.begin(), diff.end(), sortByValue);
        for (auto it : diff) {
          cout << it.first << ": " << it.second << endl; 
        }
      }

    }
    cout << "optimization time: " << (clock()-t1)*1.0/CLOCKS_PER_SEC << "s" << endl;
    
  }
  else {
    cout << "missing query name or probability inference mode" << endl;
    exit(0);
  }

}