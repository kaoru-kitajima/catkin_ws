#include<iostream>
#include<libfreenect.h>
#include<opencv2/opencv.hpp>

cv::Mat img;

void depth_cb(freenect_device *dev, void *depth, uint32_t timestamp) 
{
    cv::Mat d8;
    //深度画像データを入れる
    cv::Mat temp = cv::Mat(480, 640, CV_16UC1, depth);

    //8bitグレースケール画像に変換, 4000は測定可能な最大距離
        temp.convertTo(d8, CV_8UC1, -255.0f/4000, 255.0f);
    //データをコピー
        memcpy(img.data, d8.data, 640*480);
}

int main()
{
    img = cv::Mat(480, 640, CV_8UC1);
    freenect_context *ctx;
    freenect_device *dev;

    //コンテキストの情報を初期化. 成功すると0を返す.
    if (freenect_init(&ctx, NULL) < 0)
    {
            std::cout << "初期化に失敗しました." << std::endl; 
            return 1;
    }

    //デバイスに接続. 成功すると0を返す.
    if (freenect_open_device(ctx, &dev, 0) < 0)
    {
            std::cout << "Kinectに接続できません." << std::endl;
            return 1;
    }

    //depth_cb内の処理は自分で書く
    freenect_set_depth_callback(dev, depth_cb);

    //データを流す
    freenect_start_depth(dev);
    while(freenect_process_events(ctx) >= 0 ){
        cv::imshow("Depth", img);
        if(cv::waitKey(1) == 'q'){
            break;  
        }
    }
}