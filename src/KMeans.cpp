#include "KMeans.h"
#include "CKMeansProvGraph.h"

void KMeans::run(vector<Point> & points)
{
  if(K > total_points)
    return;

  vector<int> prohibited_indexes;

  // choose K distinct values for the centers of the clusters
  // for(int i = 0; i < K; i++)
  // {
  //   int index_point = i * (total_points / K);
  //   prohibited_indexes.push_back(index_point);
  //   points[index_point].setCluster(i);
  //   Cluster cluster(i, points[index_point]);
  //   clusters.push_back(cluster);
  // }

  initializeCentroidsPlusPlus(points, false);

  int iter = 1;

  while(true)
  {
    bool done = true;

    // associates each point to the nearest center
    for(int i = 0; i < total_points; i++)
    {
      int id_old_cluster = points[i].getCluster();
      int id_nearest_center = getIDNearestCenter(points[i]);

      if(id_old_cluster != id_nearest_center)
      {
        if(id_old_cluster != -1)
          clusters[id_old_cluster].removePoint(points[i].getID());

        points[i].setCluster(id_nearest_center);
        clusters[id_nearest_center].addPoint(points[i]);
        done = false;
      }
    }

    // recalculating the center of each cluster
    for(int i = 0; i < K; i++)
    {
      for(int j = 0; j < total_values; j++)
      {
        int total_points_cluster = clusters[i].getTotalPoints();
        double sum = 0.0;

        if(total_points_cluster > 0)
        {
          for(int p = 0; p < total_points_cluster; p++)
            sum += clusters[i].getPoint(p).getValue(j);
          clusters[i].setCentralValue(j, sum / total_points_cluster);
        }
      }
    }

    if(done == true || iter >= max_iterations)
    {
      std::cout << "Break in iteration " << iter << "\n\n";
      std::cout << "Break in iteration " << iter << "\n\n";
      break;
    }

    iter++;
  }

  // shows elements of clusters
  for(int i = 0; i < K; i++)
  {
  //   int total_points_cluster =  clusters[i].getTotalPoints();

  //   std::cout << "Cluster " << clusters[i].getID() + 1 << endl;
  //   for(int j = 0; j < total_points_cluster; j++)
  //   {
  //     std::cout << "Point " << clusters[i].getPoint(j).getID() + 1 << ": ";
  //     for(int p = 0; p < total_values; p++)
  //       std::cout << clusters[i].getPoint(j).getValue(p) << " ";

  //     string point_name = clusters[i].getPoint(j).getName();

  //     if(point_name != "")
  //       std::cout << "- " << point_name;

  //     std::cout << endl;
  //   }

    // std::cout << "Cluster values: ";

    // for(int j = 0; j < total_values; j++)
    //   std::cout << clusters[i].getCentralValue(j) << " ";

    // std::cout << "\n";
  }
}

void KMeans::run(vector<Point> & points, int index)
{
  if(K > total_points)
    return;

  vector<int> prohibited_indexes;

  // choose K distinct values for the centers of the clusters
  // for(int i = 0; i < K; i++)
  // {
  //   int index_point = i * (total_points / K);
  //   prohibited_indexes.push_back(index_point);
  //   points[index_point].setCluster(i);
  //   Cluster cluster(i, points[index_point]);
  //   clusters.push_back(cluster);
  // }

  initializeCentroidsPlusPlus(points, false);

  int iter = 1;

  while(true)
  {
    bool done = true;

    // associates each point to the nearest center
    for(int i = 0; i < total_points; i++)
    {
      if (i==index) continue;
      int id_old_cluster = points[i].getCluster();
      int id_nearest_center = getIDNearestCenter(points[i]);

      if(id_old_cluster != id_nearest_center)
      {
        if(id_old_cluster != -1)
          clusters[id_old_cluster].removePoint(points[i].getID());

        points[i].setCluster(id_nearest_center);
        clusters[id_nearest_center].addPoint(points[i]);
        done = false;
      }
    }

    // recalculating the center of each cluster
    for(int i = 0; i < K; i++)
    {
      for(int j = 0; j < total_values; j++)
      {
        int total_points_cluster = clusters[i].getTotalPoints();
        double sum = 0.0;

        if(total_points_cluster > 0)
        {
          for(int p = 0; p < total_points_cluster; p++)
            sum += clusters[i].getPoint(p).getValue(j);
          clusters[i].setCentralValue(j, sum / total_points_cluster);
        }
      }
    }

    if(done == true || iter >= max_iterations)
    {
      std::cout << "Break in iteration " << iter << "\n\n";
      break;
    }

    iter++;
  }


  // shows elements of clusters
  for(int i = 0; i < K; i++)
  {
    // int total_points_cluster =  clusters[i].getTotalPoints();

    // std::cout << "Cluster " << clusters[i].getID() + 1 << endl;
    // for(int j = 0; j < total_points_cluster; j++)
    // {
    //   std::cout << "Point " << clusters[i].getPoint(j).getID() + 1 << ": ";
    //   for(int p = 0; p < total_values; p++)
    //     std::cout << clusters[i].getPoint(j).getValue(p) << " ";

    //   string point_name = clusters[i].getPoint(j).getName();

    //   if(point_name != "")
    //     std::cout << "- " << point_name;

    //   std::cout << endl;
    // }

    std::cout << "Cluster values: ";

    for(int j = 0; j < total_values; j++)
      std::cout << clusters[i].getCentralValue(j) << " ";

    std::cout << "\n";
  }
}

void KMeans::runWithProv(vector<Point> & points) {
  if(K > total_points)
    return;

  // initialize variables
  for (int i=0; i<this->total_points; i++) {
    provG.addVariableVertex(ckpg::Input, "point_"+to_string(i)+"_"+to_string(0), &points+i);
    // provG.auxilary_data.push_back(points[i].getValues());
  }

  vector<int> prohibited_indexes;

  // choose K distinct values for the centers of the clusters
  for(int i = 0; i < K; i++)
  {
    int index_point = i * (total_points / K);
    prohibited_indexes.push_back(index_point);
    points[index_point].setCluster(i);
    Cluster cluster(i, points[index_point]);
    clusters.push_back(cluster);
    provG.addVariableVertex(ckpg::Parameter, "centroid_"+to_string(i)+"_"+to_string(0), &points+index_point);
  }

  int iter = 1;

  while(true)
  {
    bool done = true;

    // associates each point to the nearest center
    vector<vector<string> > input_names (K, vector<string> (0));
    for(int i = 0; i < total_points; i++)
    {
      int id_old_cluster = points[i].getCluster();
      int id_nearest_center = getIDNearestCenter(points[i]);

      // record input names of points
      input_names[id_nearest_center].push_back("point_"+to_string(i)+"_"+to_string(iter));
      
      // from centroids to points
      vector<string> centroid_input_names;
      for (int j=0; j<clusters.size(); j++) {
        centroid_input_names.push_back("centroid_"+to_string(j)+"_"+to_string(iter-1));
      }
      centroid_input_names.push_back("point_"+to_string(i)+"_"+to_string(iter-1));
      provG.addComputingSubgraph("point_"+to_string(i)+"_"+to_string(iter), &points+i, ckpg::NearestCentroid, centroid_input_names);

      if(id_old_cluster != id_nearest_center)
      {
        if(id_old_cluster != -1)
          clusters[id_old_cluster].removePoint(points[i].getID());

        points[i].setCluster(id_nearest_center);
        clusters[id_nearest_center].addPoint(points[i]);
        done = false;
      }
    }

    // recalculating the center of each cluster
    for(int i = 0; i < K; i++)
    {
      vector<double> centroid;
      for(int j = 0; j < total_values; j++)
      {
        int total_points_cluster = clusters[i].getTotalPoints();
        double sum = 0.0;

        if(total_points_cluster > 0)
        {
          for(int p = 0; p < total_points_cluster; p++)
            sum += clusters[i].getPoint(p).getValue(j);
          clusters[i].setCentralValue(j, sum / total_points_cluster);
          centroid.push_back(sum / total_points_cluster);
        }
      }
      // add provenance subgraph
      // provG.auxilary_data.push_back(centroid);
      string centroid_name = "centroid_"+to_string(i)+"_"+to_string(iter);
      provG.addComputingSubgraph(centroid_name, &centroid, ckpg::VectorMean, input_names[i]);
    }

    if(done == true || iter >= max_iterations)
    {
      std::cout << "Break in iteration " << iter << "\n\n";
      break;
    }

    iter++;
  }

}


int KMeans::runWithProv_v2(vector<Point> & points, vector<vector<double>*>& centroids, vector<vector<double>*>& distances) {
  if(K > total_points)
    return 0;

  // initialize variables
  for (int i=0; i<this->total_points; i++) {
    provG.addVariableVertex(ckpg::Input, "point_"+to_string(i)+"_"+to_string(0), &points[i]);
    // provG.auxilary_data.push_back(points[i].getValues());
  }

  vector<int> prohibited_indexes;

  // choose K distinct values for the centers of the clusters
  // vector<double>* centroid = new vector<double>();
  // centroids.push_back(centroid);
  // for(int i = 0; i < K; i++)
  // {
  //   int index_point = i * (total_points / K);
  //   prohibited_indexes.push_back(index_point);
  //   points[index_point].setCluster(i);
  //   Cluster cluster(i, points[index_point]);
  //   clusters.push_back(cluster);
  //   provG.addVariableVertex(ckpg::Parameter, "centroid_"+to_string(i)+"_"+to_string(0), &points[index_point]);
  // }

  initializeCentroidsPlusPlus(points, true);


  int iter = 1;

  while(true)
  {
    bool done = true;

    // associates each point to the nearest center
    vector<vector<string>> input_names (K, vector<string> (0));
     // Vector to store distances from points to centroids

    for(int i = 0; i < total_points; i++)
    {
      // Calculate distances from this point to all centroids
      vector<double> point_distances(K, 0.0);
      int id_old_cluster = points[i].getCluster();
      int id_nearest_center = -1;
      double min_dist = std::numeric_limits<double>::max();
      
      for(int k = 0; k < K; k++) {
        double dist = 0.0;
        for(int j = 0; j < total_values; j++) {
          dist += pow(clusters[k].getCentralValue(j) - points[i].getValue(j), 2.0);
        }
        dist = sqrt(dist);
        point_distances[k] = dist;
        if(dist < min_dist) {
          min_dist = dist;
          id_nearest_center = k;
        }
      }

      distances.push_back(new vector<double>(point_distances));
      
      // from centroids to points
      vector<string> centroid_input_names;
      for (int j=0; j<clusters.size(); j++) {
        centroid_input_names.push_back("centroid_"+to_string(j)+"_"+to_string(iter-1));
      }
      centroid_input_names.push_back("point_"+to_string(i)+"_"+to_string(iter-1));
      string dist_vertex_name = "dist_" + to_string(i) + "_" + to_string(iter);
      provG.addComputingSubgraph(dist_vertex_name, distances[i], ckpg::Dist, centroid_input_names);


      // Now, let's create a new point vertex representing the point's cluster assignment at this iteration
      string point_vertex_name = "point_" + to_string(i) + "_" + to_string(iter);
      vector<string> point_input_names = {dist_vertex_name}; // The input for this vertex is the mapping vertex
      provG.addComputingSubgraph(point_vertex_name, &points[i], ckpg::NearestCentroid, point_input_names, id_nearest_center);

      input_names[id_nearest_center].push_back(point_vertex_name);

      if(id_old_cluster != id_nearest_center)
      {
        if(id_old_cluster != -1)
          clusters[id_old_cluster].removePoint(points[i].getID());

        points[i].setCluster(id_nearest_center);
        clusters[id_nearest_center].addPoint(points[i]);
        done = false;
      }
    }

    // recalculating the center of each cluster
    for(int i = 0; i < K; i++)
    {
      // Dynamically allocate memory for the new centroid
      vector<double>* centroid = new vector<double>();
      centroids.push_back(centroid); // Add the pointer to the parameter vector

      for(int j = 0; j < total_values; j++)
      {
        int total_points_cluster = clusters[i].getTotalPoints();
        double sum = 0.0;
        if(total_points_cluster > 0)
        {
          for(int p = 0; p < total_points_cluster; p++)
            sum += clusters[i].getPoint(p).getValue(j);
          centroid->push_back(sum / total_points_cluster);
          clusters[i].setCentralValue(j, sum / total_points_cluster);
        }
      }

      // Add provenance subgraph
      string centroid_name = "centroid_" + to_string(i) + "_" + to_string(iter);
      provG.addComputingSubgraph(centroid_name, centroid, ckpg::VectorMean, input_names[i]);
    }

    if(done == true || iter >= max_iterations)
    {
      std::cout << "Break in iteration " << iter << "\n\n";
      std::cout << "Break in iteration " << iter << "\n\n";
      break;
    }

    iter++;
  }

  return iter;
}


void KMeans::deletePoint(vector<Point> & points, vector<vector<double>*>& centroids, vector<vector<double>*>& distances, int index, int iteration) {

  ckpg::Graph& g = this->provG.g;
  unordered_map<int, vector<int>> to_change;
  vector<int> prev_nearest_neightbors (points.size(), -1);

  bool done = false;
  int iter = 1;
  for (; iter<=iteration; iter++) {

    string delete_name = "point_" + to_string(index) + "_" + to_string(iter);
    ckpg::vertex_t point_vertex = this->provG.getVertexByName(delete_name);
    int nearest_centroid_id = g[point_vertex].centroid;

    to_change[nearest_centroid_id].push_back(-1*(index+1));

    cout << "iteration: " << iter << ", number of changed centroid: " << to_change.size() << endl;

    for (auto it : to_change) {
      int centroid_id = it.first;

      string centroid_name = "centroid_" + to_string(centroid_id) + "_" + to_string(iter);
      string operator_name = "vectormean_centroid_" + to_string(centroid_id) + "_" + to_string(iter);

      ckpg::vertex_t op_vertex = this->provG.getVertexByName(operator_name);

      auto in_edges = boost::in_edges(op_vertex, g);
      int num_contributing_points = std::distance(in_edges.first, in_edges.second);
      if (num_contributing_points==1) continue;

      // update the centroid
      ckpg::vertex_t centroid_vertex = this->provG.getVertexByName(centroid_name);
      vector<double>* centroid = static_cast<vector<double>*>(g[centroid_vertex].value);

      for (auto i=0; i<centroid->size(); i++) {
        (*centroid)[i] *= num_contributing_points;
      }

      // cout << it.second.size() << endl;
      for (auto point_id : it.second) {
        int add_or_remove = point_id / abs(point_id); // 1: add, -1: remove
        int real_point_id = abs(point_id)-1;
        for (auto i=0; i<centroid->size(); i++) {
          (*centroid)[i] += add_or_remove * points[real_point_id].getValue(i);
        }
        num_contributing_points += add_or_remove;
      }

      for (auto i=0; i<centroid->size(); i++) {
        (*centroid)[i] /= num_contributing_points;
      }

      if (iter==iteration) continue;

      // update distances between all points and the updated centroids
      for (auto i=0; i<total_points; i++) {
        if (i==index) continue;
        double sum = 0.0;

        for(auto j = 0; j < centroid->size(); j++)
        {
          sum += pow((*centroid)[j] - points[i].getValue(j), 2.0);
        }
        double dist = sqrt(sum);
        vector<double>* distance = distances[total_points*iter+i];
        (*distance)[centroid_id] = dist;
      }

    }

    if (iter==iteration || done) break;

    to_change.clear();
    done = true;
    for (int i=0; i<total_points; i++) {
      if (i==index) continue;
      vector<double>* distance = distances[total_points*iter+i];
      auto min_iter = std::min_element(distance->begin(), distance->end());
      int new_nearest_neightbor = static_cast<int>(std::distance(distance->begin(), min_iter));

      int old_nearest_neightbor = g[this->provG.getVertexByName("point_" + to_string(i) + "_" + to_string(iter+1))].centroid;
      int prev_nearest_neightbor = prev_nearest_neightbors[i];
      if (new_nearest_neightbor!=old_nearest_neightbor) {
        to_change[new_nearest_neightbor].push_back(i+1);
        to_change[old_nearest_neightbor].push_back(-1*(i+1));
      }
      if (prev_nearest_neightbor!=new_nearest_neightbor) done = false;
      prev_nearest_neightbors[i] = new_nearest_neightbor;
    }

  }

  // choose K distinct values for the centers of the clusters
  for(int i = 0; i < K; i++)
  {
    string centroid_name = "centroid_" + to_string(i) + "_" + to_string(iteration);
    ckpg::vertex_t centroid_vertex = this->provG.getVertexByName(centroid_name);
    vector<double>* centroid = static_cast<vector<double>*>(g[centroid_vertex].value);
    Cluster cluster(i, total_values, centroid);
    clusters.push_back(cluster);
  }

  // int iter = iteration+1;

  // while(true)
  // {
  //   bool done = true;

  //   // associates each point to the nearest center
  //   for(int i = 0; i < total_points; i++)
  //   {
  //     if (i==index) continue;
  //     int id_old_cluster = points[i].getCluster();
  //     int id_nearest_center = getIDNearestCenter(points[i]);

  //     if(id_old_cluster != id_nearest_center)
  //     {
  //       if(id_old_cluster != -1)
  //         clusters[id_old_cluster].removePoint(points[i].getID());

  //       points[i].setCluster(id_nearest_center);
  //       clusters[id_nearest_center].addPoint(points[i]);
  //       done = false;
  //     }
  //   }

  //   // recalculating the center of each cluster
  //   for(int i = 0; i < K; i++)
  //   {
  //     for(int j = 0; j < total_values; j++)
  //     {
  //       int total_points_cluster = clusters[i].getTotalPoints();
  //       double sum = 0.0;

  //       if(total_points_cluster > 0)
  //       {
  //         for(int p = 0; p < total_points_cluster; p++)
  //           sum += clusters[i].getPoint(p).getValue(j);
  //         clusters[i].setCentralValue(j, sum / total_points_cluster);
  //       }
  //     }
  //   }

  //   if(done == true || iter >= max_iterations)
  //   {
  //     std::cout << "Break in iteration " << iter << "\n\n";
  //     break;
  //   }

  //   iter++;
  // }

  for(int i = 0; i < K; i++)
  {
  //   int total_points_cluster =  clusters[i].getTotalPoints();

  //   std::cout << "Cluster " << clusters[i].getID() + 1 << endl;
  //   for(int j = 0; j < total_points_cluster; j++)
  //   {
  //     std::cout << "Point " << clusters[i].getPoint(j).getID() + 1 << ": ";
  //     for(int p = 0; p < total_values; p++)
  //       std::cout << clusters[i].getPoint(j).getValue(p) << " ";

  //     string point_name = clusters[i].getPoint(j).getName();

  //     if(point_name != "")
  //       std::cout << "- " << point_name;

  //     std::cout << endl;
  //   }

    std::cout << "Cluster values: ";

    for(int j = 0; j < total_values; j++)
      std::cout << clusters[i].getCentralValue(j) << " ";

    std::cout << "\n";
  }
}

  // choose K distinct values for the centers of the clusters
  for(int i = 0; i < K; i++)
  {
    string centroid_name = "centroid_" + to_string(i) + "_" + to_string(iteration);
    ckpg::vertex_t centroid_vertex = this->provG.getVertexByName(centroid_name);
    vector<double>* centroid = static_cast<vector<double>*>(g[centroid_vertex].value);
    Cluster cluster(i, total_values, centroid);
    clusters.push_back(cluster);
  }

  // int iter = iteration+1;

  // while(true)
  // {
  //   bool done = true;

  //   // associates each point to the nearest center
  //   for(int i = 0; i < total_points; i++)
  //   {
  //     if (i==index) continue;
  //     int id_old_cluster = points[i].getCluster();
  //     int id_nearest_center = getIDNearestCenter(points[i]);

  //     if(id_old_cluster != id_nearest_center)
  //     {
  //       if(id_old_cluster != -1)
  //         clusters[id_old_cluster].removePoint(points[i].getID());

  //       points[i].setCluster(id_nearest_center);
  //       clusters[id_nearest_center].addPoint(points[i]);
  //       done = false;
  //     }
  //   }

  //   // recalculating the center of each cluster
  //   for(int i = 0; i < K; i++)
  //   {
  //     for(int j = 0; j < total_values; j++)
  //     {
  //       int total_points_cluster = clusters[i].getTotalPoints();
  //       double sum = 0.0;

  //       if(total_points_cluster > 0)
  //       {
  //         for(int p = 0; p < total_points_cluster; p++)
  //           sum += clusters[i].getPoint(p).getValue(j);
  //         clusters[i].setCentralValue(j, sum / total_points_cluster);
  //       }
  //     }
  //   }

  //   if(done == true || iter >= max_iterations)
  //   {
  //     std::cout << "Break in iteration " << iter << "\n\n";
  //     break;
  //   }

  //   iter++;
  // }

  for(int i = 0; i < K; i++)
  {
  //   int total_points_cluster =  clusters[i].getTotalPoints();

  //   std::cout << "Cluster " << clusters[i].getID() + 1 << endl;
  //   for(int j = 0; j < total_points_cluster; j++)
  //   {
  //     std::cout << "Point " << clusters[i].getPoint(j).getID() + 1 << ": ";
  //     for(int p = 0; p < total_values; p++)
  //       std::cout << clusters[i].getPoint(j).getValue(p) << " ";

  //     string point_name = clusters[i].getPoint(j).getName();

  //     if(point_name != "")
  //       std::cout << "- " << point_name;

  //     std::cout << endl;
  //   }

    std::cout << "Cluster values: ";

    for(int j = 0; j < total_values; j++)
      std::cout << clusters[i].getCentralValue(j) << " ";

    std::cout << "\n";
  }

}