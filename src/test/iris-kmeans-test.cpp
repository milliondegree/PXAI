// Implementation of the KMeans Algorithm
// reference: http://mnemstudio.org/clustering-k-means-example-1.htm

#include "../KMeans.h"
#include <boost/algorithm/string.hpp>


const int number_classes = 3;
const int number_features = 4;
const int number_points = 150;
const char *iris_dataset = "./data/iris/iris.data";
const std::string cprov_save_path = "data/iris-kmeans/cprov/test.dot";
const std::array<std::string, number_classes> class_names =
{ "Iris-setosa", "Iris-versicolor", "Iris-virginica" };


void load_data(vector<Point>& points) {
  ifstream ofin(iris_dataset);
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

  K = 3;
  max_iterations = 100;

  vector<Point> points;
  load_data(points);

  KMeans kmeans(K, number_points, number_features, max_iterations);

  clock_t t1, t2;
  t1 = clock();
  kmeans.run(points);
  t2 = clock();
  std::cout << "Without provenance: " << (t2-t1)*1.0/CLOCKS_PER_SEC << std::endl;

  // clock_t t1, t2;
  // t1 = clock();
  // kmeans.runWithProv(points);
  // t2 = clock();
  // std::cout << "With provenance: " << (t2-t1)*1.0/CLOCKS_PER_SEC << std::endl;

  return 0;
}