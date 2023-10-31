#include "../MLN.h"

using namespace std;


int main(int argc, char* argv[]) {  

  vector<string> provenance_file_list = {"./data/hypertext-class/sample7/prov/sample71.txt",
                                        "./data/hypertext-class/sample7/prov/sample72.txt",
                                        "./data/hypertext-class/sample7/prov/sample73.txt",
                                        "./data/hypertext-class/sample7/prov/sample74.txt",
                                        "./data/hypertext-class/sample7/prov/sample75.txt",
                                        "./data/hypertext-class/sample7/prov/sample76.txt",
                                        "./data/hypertext-class/sample7/prov/sample77.txt",
                                        "./data/hypertext-class/sample7/prov/sample78.txt",
                                        "./data/hypertext-class/sample7/prov/sample79.txt",
                                        "./data/hypertext-class/sample7/prov/sample710.txt"
  };
  vector<string> observe_file_list = {"./data/hypertext-class/sample7/sample71.obs",
                                      "./data/hypertext-class/sample7/sample72.obs",
                                      "./data/hypertext-class/sample7/sample73.obs",
                                      "./data/hypertext-class/sample7/sample74.obs",
                                      "./data/hypertext-class/sample7/sample75.obs",
                                      "./data/hypertext-class/sample7/sample76.obs",
                                      "./data/hypertext-class/sample7/sample77.obs",
                                      "./data/hypertext-class/sample7/sample78.obs",
                                      "./data/hypertext-class/sample7/sample79.obs",
                                      "./data/hypertext-class/sample7/sample710.obs"
  };
  vector<double> lbp_time_list, lbp_prov_time_list, prov_query_time_list, lbp_coarse_prov_time_list, prov_query_coarse_time_list;

  for (int i=0; i<10; i++) {
    cout << "start sample " << i+1 << endl;
    Load l(provenance_file_list[i], observe_file_list[i]);
    vector<string> prov = l.getProv();

    MLN mln(l);
    Parser parser;
    parser.parseProvenance(mln);
    mln.merge();
    cout << "clique size: " << mln.cliques.size() << endl;

    clock_t t1, t2;
    t1 = clock();
    int round = mln.loopyBeliefPropagation();
    cout << "round " << round << endl;
    t2 = clock();
    lbp_time_list.push_back((t2-t1)*1.0/CLOCKS_PER_SEC);

    t1 = clock();
    round = mln.loopyBeliefPropagationWithProv();
    cout << "round " << round << endl;
    t2 = clock();
    lbp_prov_time_list.push_back((t2-t1)*1.0/CLOCKS_PER_SEC);

    double query_time_sum = 0;
    int count = 0;
    for (string query : mln.queries) {
      if (Parser::isVariable(query)) {
        string output_variable_name = query+"_iteration_"+to_string(round-1);
        t1 = clock();
        CProvGraph::CProvGraph query_of_output = mln.provG.ProvenanceQuery(output_variable_name);
        t2 = clock();
        count += 1;
        query_time_sum += (t2-t1)*1.0/CLOCKS_PER_SEC;
      }
    }
    prov_query_time_list.push_back(query_time_sum/count);
  }

  for (int i=0; i<10; i++) {
    cout << "start sample " << i+1 << endl;
    Load l(provenance_file_list[i], observe_file_list[i]);
    vector<string> prov = l.getProv();

    MLN mln(l);
    Parser parser;
    parser.parseProvenance(mln);
    mln.merge();
    cout << "clique size: " << mln.cliques.size() << endl;

    clock_t t1, t2;
    int round;
    t1 = clock();
    round = mln.loopyBeliefPropagationWithCoarseProv();
    cout << "round " << round << endl;
    t2 = clock();
    lbp_coarse_prov_time_list.push_back((t2-t1)*1.0/CLOCKS_PER_SEC);

    double query_coarse_time_sum = 0;
    int count = 0;
    for (string query : mln.queries) {
      if (Parser::isVariable(query)) {
        string output_variable_name = query+"_iteration_"+to_string(round-1);
        t1 = clock();
        CProvGraph::CProvGraph query_of_output = mln.provG.ProvenanceQuery(output_variable_name);
        t2 = clock();
        count += 1;
        query_coarse_time_sum += (t2-t1)*1.0/CLOCKS_PER_SEC;
      }
    }
    prov_query_coarse_time_list.push_back(query_coarse_time_sum/count);
  }

  for (double t : lbp_time_list) {
    cout << t << ", ";
  }
  cout << endl;
  for (double t : lbp_prov_time_list) {
    cout << t << ", ";
  }
  cout << endl;
  for (double t : prov_query_time_list) {
    cout << t << ", ";
  }
  cout << endl;
  for (double t : lbp_coarse_prov_time_list) {
    cout << t << ", ";
  }
  cout << endl;
  for (double t : prov_query_coarse_time_list) {
    cout << t << ", ";
  }
  cout << endl;

}