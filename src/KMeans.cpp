#include "KMeans.h"

void KMeans::run(vector<Point> & points)
{
  if(K > total_points)
    return;

  vector<int> prohibited_indexes;

  // choose K distinct values for the centers of the clusters
  for(int i = 0; i < K; i++)
  {
    while(true)
    {
      int index_point = rand() % total_points;

      if(find(prohibited_indexes.begin(), prohibited_indexes.end(),
          index_point) == prohibited_indexes.end())
      {
        prohibited_indexes.push_back(index_point);
        points[index_point].setCluster(i);
        Cluster cluster(i, points[index_point]);
        clusters.push_back(cluster);
        break;
      }
    }
  }

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
      cout << "Break in iteration " << iter << "\n\n";
      break;
    }

    iter++;
  }

  // shows elements of clusters
  // for(int i = 0; i < K; i++)
  // {
  //   int total_points_cluster =  clusters[i].getTotalPoints();

  //   cout << "Cluster " << clusters[i].getID() + 1 << endl;
  //   for(int j = 0; j < total_points_cluster; j++)
  //   {
  //     cout << "Point " << clusters[i].getPoint(j).getID() + 1 << ": ";
  //     for(int p = 0; p < total_values; p++)
  //       cout << clusters[i].getPoint(j).getValue(p) << " ";

  //     string point_name = clusters[i].getPoint(j).getName();

  //     if(point_name != "")
  //       cout << "- " << point_name;

  //     cout << endl;
  //   }

  //   cout << "Cluster values: ";

  //   for(int j = 0; j < total_values; j++)
  //     cout << clusters[i].getCentralValue(j) << " ";

  //   cout << "\n\n";
  // }
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
    while(true)
    {
      int index_point = rand() % total_points;

      if(find(prohibited_indexes.begin(), prohibited_indexes.end(),
          index_point) == prohibited_indexes.end())
      {
        prohibited_indexes.push_back(index_point);
        points[index_point].setCluster(i);
        Cluster cluster(i, points[index_point]);
        clusters.push_back(cluster);

        // add initial cluster centrold to provG
        // provG.auxilary_data.push_back(points[index_point].getValues());
        provG.addVariableVertex(ckpg::Parameter, "centroid_"+to_string(i)+"_"+to_string(0), &points+index_point);

        break;
      }
    }
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
      cout << "Break in iteration " << iter << "\n\n";
      break;
    }

    iter++;
  }

  // shows elements of clusters
  // for(int i = 0; i < K; i++)
  // {
  //   int total_points_cluster =  clusters[i].getTotalPoints();

  //   cout << "Cluster " << clusters[i].getID() + 1 << endl;
  //   for(int j = 0; j < total_points_cluster; j++)
  //   {
  //     cout << "Point " << clusters[i].getPoint(j).getID() + 1 << ": ";
  //     for(int p = 0; p < total_values; p++)
  //       cout << clusters[i].getPoint(j).getValue(p) << " ";

  //     string point_name = clusters[i].getPoint(j).getName();

  //     if(point_name != "")
  //       cout << "- " << point_name;

  //     cout << endl;
  //   }

  //   cout << "Cluster values: ";

  //   for(int j = 0; j < total_values; j++)
  //     cout << clusters[i].getCentralValue(j) << " ";

  //   cout << "\n\n";
  // }
}

void KMeans::deletePoint(vector<Point> & points, int index) {
  ckpg::Graph g = this->provG.g;
  
}