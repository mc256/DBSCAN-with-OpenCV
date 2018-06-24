#include "main.hpp"


void generate_random_points(vector<Point2f> * point_list){

}

void load_data_set(vector<Point2f> * point_list, vector<int> * labels){
    ifstream sample_file("/home/mc/Desktop/container/1/currenttime-11-32-36--224.png.data.txt");
    string buffer;

    while (sample_file >> buffer){
        if (buffer == "points") break;
    }

    int x, y;
    while (sample_file >> x >> y){
        point_list->push_back(Point2f(x,y));
        labels->push_back(0);
    }
}

Mat display_points_with_label(vector<Point2f> * point_list, vector<int> * labels){
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
        circle(image, point_list->at(i), 5, random_color_list[labels->at(i)], 2);
    }

    return image;
}

int main() {
    vector<Point2f> points;
    vector<int> labels;

    load_data_set(&points, &labels);

    for (const auto & item: points){
        cout << "(" << item.x << "," << item.y << ")" << endl;
    }


    auto display_image = display_points_with_label(&points, &labels);

    // Configuration
    namedWindow("test", WINDOW_FREERATIO);

    // Display
    imshow("test", display_image);


    waitKey(0);

    return 0;
}