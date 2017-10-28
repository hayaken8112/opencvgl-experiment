#include <opencv2/opencv.hpp>
#include <stdio.h>

int size_of_mosaic = 4;

int main(int argc, char *argv[])
{
  // 1. load classifier
  std::string cascadeName = "/usr/share/opencv/haarcascades/haarcascade_frontalface_alt.xml"; //Haar-like
  // std::string eye_cascadeName = "/usr/share/opencv/haarcascades/haarcascade_eye_tree_eyeglasses.xml"; //Haar-like
  cv::CascadeClassifier cascade;
  // cv::CascadeClassifier eye_cascade;
  if(!cascade.load(cascadeName)){
    printf("ERROR: cascadeFile not found\n");
    return -1;
  }
  /*
  if(!cascade.load(eye_cascadeName)){
    printf("ERROR: eye_cascadeFile not found\n");
    return -1;
  }
  */
  
  // 2. initialize VideoCapture
  cv::Mat frame;
  cv::VideoCapture cap;
  cap.open(0);
  cap >> frame;
  
  // 3. prepare window and trackbar
  cv::namedWindow("result", 1);
  cv::createTrackbar("size", "resize", &size_of_mosaic, 30, 0);
  

  double scale = 4.0;
  cv::Mat gray, smallImg(cv::saturate_cast<int>(frame.rows/scale),
               cv::saturate_cast<int>(frame.cols/scale), CV_8UC1);

  for(;;){
    
    // 4. capture frame
    cap >> frame;
    // convert to gray scale
    cv::cvtColor(frame, gray, CV_BGR2GRAY);
    
    // 5. scale-down the image
    cv::resize(gray, smallImg, smallImg.size(), 0, 0, cv::INTER_LINEAR);
    cv::equalizeHist(smallImg, smallImg);    

    // 6. detect face using Haar-classifier
    std::vector<cv::Rect> faces;
    // multi-scale face searching
    // image, size, scale, num, flag, smallest rect
    cascade.detectMultiScale(smallImg, faces, 1.1, 2, CV_HAAR_SCALE_IMAGE, cv::Size(30, 30));

    // 7. mosaic(pixelate) face-region
    for(int i = 0; i < faces.size(); i++){
      cv::Point center;
      int radius;
      center.x = cv::saturate_cast<int>((faces[i].x + faces[i].width * 0.5) * scale);
      center.y = cv::saturate_cast<int>((faces[i].y + faces[i].height * 0.5) * scale);
      radius = cv::saturate_cast<int>((faces[i].width + faces[i].height) * 0.25 * scale);
      // cv::ellipse( frame, center, cv::Size( faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, cv::Scalar( 255, 0, 255 ), 4, 8, 0 );

      // mosaic
      if(size_of_mosaic < 1) size_of_mosaic = 1;
      cv::Rect roi_rect(center.x - radius, center.y - radius, radius * 2, radius * 2);
      cv::Mat mosaic = frame(roi_rect);
      cv::Mat tmp;
      cv::resize(mosaic, tmp, cv::Size(radius / size_of_mosaic, radius / size_of_mosaic), 0, 0);
      cv::resize(tmp, mosaic, cv::Size(radius * 2, radius * 2), 0, 0, CV_INTER_NN);
/*
      cv::Mat faceROI = gray( faces[i] );
      std::vector<cv::Rect> eyes;
  
      //-- In each face, detect eyes
      eye_cascade.detectMultiScale( faceROI, eyes, 1.1, 2,CV_HAAR_SCALE_IMAGE, cv::Size(30, 30) );
  
      for( size_t j = 0; j < eyes.size(); j++ )
       {
         cv::Point center( faces[i].x + eyes[j].x + eyes[j].width*0.5, faces[i].y + eyes[j].y + eyes[j].height*0.5 );
         int radius = cvRound( (eyes[j].width + eyes[j].height)*0.25 );
         circle( frame, center, radius, cv::Scalar( 255, 0, 0 ), 4, 8, 0 );
       }
*/
    }
    
    // 8. show mosaiced image to window
    cv::imshow("result", frame);

    int key = cv::waitKey(10);
    if(key == 'q' || key == 'Q')
        break;

  }
 return 0;
}

