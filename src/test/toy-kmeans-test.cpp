// Implementation of the KMeans Algorithm
// reference: http://mnemstudio.org/clustering-k-means-example-1.htm

#include "../KMeans.h"
#include <boost/algorithm/string.hpp>


const int number_classes = 3;
const int number_features = 2;
const int number_points = 6;
const char *toy_kmeans_dataset = "./data/toy-kmeans/toy-kmeans.data";
const std::string cprov_save_path = "./data/toy-kmeans/cprov/test.dot";
const std::array<std::string, number_classes> class_names =
{ "Class-1", "Class-2", "Class-3" };


void load_data(vector<Point>& points) {
  ifstream ofin(toy_kmeans_dataset);
  string line;

  int n = 0;
  while (getline(ofin, line)) {
    if (!line.empty()) {
      line.erase(0, line.find_first_not_of(" \t\r\n"));
      line.erase(line.find_last_not_of(" \t\r\n")+1);
      vector<string> split;
      boost::split(split, line, boost::is_any_of(","));
      vector<double> values;
      for (int i=0; i<number_features; i++) {
        values.push_back(stod(split[i]));
      }
      string class_name = split[number_features];
      Point p(n, values, class_name);
      points.push_back(p);
      n++;
    }
  }
}


int main(int argc, char *argv[])
{
  srand (time(NULL));

  int K, max_iterations;

  K = 4;
  max_iterations = 100;

  vector<Point> points;
  load_data(points);

  KMeans kmeans(K, number_points, number_features, max_iterations);
  KMeans kmeans_prov(K, number_points, number_features, max_iterations);

  clock_t t1, t2;
  t1 = clock();
  kmeans.run(points);
  t2 = clock();
  std::cout << "Without provenance: " << (t2-t1)*1.0/CLOCKS_PER_SEC << std::endl;

  t1 = clock(); 
  kmeans_prov.runWithProv(points);
  t2 = clock();
  std::cout << "With provenance: " << (t2-t1)*1.0/CLOCKS_PER_SEC << std::endl;

  kmeans_prov.provG.setSavePath(cprov_save_path);
  kmeans_prov.provG.saveGraph();

  return 0;
}