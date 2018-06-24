#include "main.hpp"

#define CLUSTER_UNDEFINED 0
#define CLUSTER_NOISE -1

///////////////////////////////////
// Utility
///////////////////////////////////

void load_data_set(vector<Point2f> * point_list, vector<long> * labels){
    ifstream sample_file("/home/mc/Desktop/container/1/currenttime-11-32-36--224.png.data.txt");
    string buffer;

    while (sample_file >> buffer){
        if (buffer == "points") break;
    }

    int x, y;
    while (sample_file >> x >> y){
        point_list->push_back(Point2f(x,y));
        labels->push_back(CLUSTER_UNDEFINED);
    }
}

Mat display_points_with_label(vector<Point2f> * point_list, vector<long> * labels){
    int max_width, max_height;
    max_width = max_height = 0;
    for (const auto & item: *point_list){
        if (item.x > max_width){
            max_width = (int)item.x;
        }
        if (item.y > max_height){
            max_height = (int)item.y;
        }
    }
    cout << max_height << "x" << max_width << endl;

    Mat image(max_height, max_width, CV_8UC3);
    image = Scalar::all(0);

    set<int> label_set;
    for (const auto & item: *labels){
        label_set.insert(item);
    }

    map<int,Scalar> random_color_list;
    for (const auto & item: label_set){
        random_color_list[item] = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
    }

    for (auto i = 0; i < point_list->size(); ++i){
        if (labels->at(i) == CLUSTER_NOISE){
            circle(image, point_list->at(i), 1, random_color_list[labels->at(i)], 2);
        }else{
            circle(image, point_list->at(i), 5, random_color_list[labels->at(i)], 2);
        }


    }

    return image;
}


///////////////////////////////////
// Helper Method
///////////////////////////////////


vector<unsigned long> square_query_range(vector<Point2f> * sorted_point_list, unsigned long mid, int range){

    // search left and right to see if the point falls in range
    //vector<Point2f> result;
    vector<unsigned long> result;
    auto i = mid;
    while (i < sorted_point_list->size() && sorted_point_list->at(i).x < sorted_point_list->at(mid).x + range){
        if ((sorted_point_list->at(i).y < sorted_point_list->at(mid).y + range) && (sorted_point_list->at(i).y > sorted_point_list->at(mid).y - range)){
            //result.push_back(sorted_point_list->at(i));
            result.push_back(i);
        }
        i++;
    }
    i = mid;
    while (i > 0 && sorted_point_list->at(i).x > sorted_point_list->at(mid).x - range){
        if ((sorted_point_list->at(i).y < sorted_point_list->at(mid).y + range) && (sorted_point_list->at(i).y > sorted_point_list->at(mid).y - range)){
            //result.push_back(sorted_point_list->at(i));
            result.push_back(i);
        }
        i--;
    }

    // return all the points
    return result;
}

vector<unsigned long> square_query_range(vector<Point2f> * sorted_point_list, Point2f point, int range) {
    // binary search point
    unsigned long left, mid, right;
    left = 0;
    right = sorted_point_list->size() -1;
    mid = (left+right)/2;
    while (left < mid){
        if (sorted_point_list->at(mid).x < point.x){
            left = mid;
        }else if (sorted_point_list->at(mid).x > point.x){
            right = mid;
        }else{
            break;
        }
        mid = (left + right)/2;
    }
    square_query_range(sorted_point_list, mid, range);
}


///////////////////////////////////
// Algorithm
///////////////////////////////////

void density_based_spatial_clustering_of_application_with_noise(vector<Point2f> * sorted_point_list, vector<long> * labels, int range, int density){
    long cluster = 0;
    for (unsigned long i = 0; i < sorted_point_list->size(); ++i){
        if (labels->at(i) != CLUSTER_UNDEFINED) {
            continue;
        }
        auto neighbours = square_query_range(sorted_point_list, i, range);
        if (neighbours.size() < density){
            labels->at(i) = CLUSTER_NOISE;
            continue;
        }
        cluster ++;
        labels->at(i) = cluster;
        for (const auto & item: neighbours){
            if (labels->at(item) != CLUSTER_NOISE){
                labels->at(item) = cluster;
                continue;
            }
            if (labels->at(item) != CLUSTER_UNDEFINED){
                continue;
            }
            labels->at(item) = cluster;
            auto recursive_neighbours = square_query_range(sorted_point_list, item, range);
            if (recursive_neighbours.size() > density){
                for (const auto & k : recursive_neighbours){
                    neighbours.push_back(k);
                }
            }
        }

    }

}

int main() {
    vector<Point2f> points;
    vector<long> labels;

    load_data_set(&points, &labels);
    sort(points.begin(), points.end(), [](Point2f a, Point2f b)->bool{
        return a.x < b.x;
    });

    for (const auto & item: points){
        cout << "(" << item.x << "," << item.y << ")" << endl;
    }

    density_based_spatial_clustering_of_application_with_noise(&points, &labels, 300, 100);
    auto display_image = display_points_with_label(&points, &labels);

    // Configuration
    namedWindow("test", WINDOW_FREERATIO);

    // Display
    imshow("test", display_image);


    waitKey(0);

    return 0;
}