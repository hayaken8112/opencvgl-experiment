#include <opencv2/opencv.hpp>
#include <stdio.h>

cv::Mat inpaint_mask;
cv::Mat original_image, whiteLined_image, inpainted;

void myMouseEventHandler2(int event, int x , int y , int flags, void *){
  if(whiteLined_image.empty()){
    return;
  }
  cv::Mat temp = original_image.clone();

  static bool isBrushDown = false;
  static bool onceFlag = true;
  static cv::Point prevPt;
  cv::Point pt(x,y);

  bool isLButtonPressedBeforeEvent = (bool)(flags & CV_EVENT_FLAG_LBUTTON);
  if(isLButtonPressedBeforeEvent && isBrushDown){
    cv::rectangle(temp, prevPt, pt,0,3,4);
    cv::Point left_upper, right_lower;
    if (prevPt.x <= pt.x || prevPt.y <= pt.y) {
      left_upper = prevPt;
      right_lower = pt;
    }
    else {
      left_upper = pt;
      right_lower = prevPt;
    }
    cv::Rect roi_rect(left_upper.x, left_upper.y, abs(right_lower.x - left_upper.x), abs(left_upper.y - right_lower.y));
    cv::Mat srt_roi = original_image(roi_rect);
    cv::Mat dest_roi = temp(roi_rect);

    dest_roi = ~srt_roi;

    cv::imshow("image", temp);
  }

  // The XOR below means, isLButtonPressedAfterEvent
  // is usualy equal to isLButtonPressedBeforeEvent,
  // but not equal if the event is mouse down or up.
  bool isLButtonPressedAfterEvent = isLButtonPressedBeforeEvent
    ^ ((event == CV_EVENT_LBUTTONDOWN) || (event == CV_EVENT_LBUTTONUP));
  if(isLButtonPressedAfterEvent){
    if (onceFlag == true) {
        prevPt = pt;
        onceFlag = false;
      }
    isBrushDown = true;
  }else{
    isBrushDown = false;
    onceFlag = true;
  }
}

void myMouseEventHandler(int event, int x , int y , int flags, void *){
  if(whiteLined_image.empty()){
    return;
  }

  static bool isBrushDown = false;
  static cv::Point prevPt;
  cv::Point pt(x,y);

  bool isLButtonPressedBeforeEvent = (bool)(flags & CV_EVENT_FLAG_LBUTTON);
  if(isLButtonPressedBeforeEvent && isBrushDown){
    cv::line(inpaint_mask, prevPt, pt, cv::Scalar(255), 5, 8, 0);
    cv::line(whiteLined_image, prevPt, pt, cv::Scalar::all(255), 5, 8, 0);
    cv::imshow("image", whiteLined_image);
  }

  // The XOR below means, isLButtonPressedAfterEvent
  // is usualy equal to isLButtonPressedBeforeEvent,
  // but not equal if the event is mouse down or up.
  bool isLButtonPressedAfterEvent = isLButtonPressedBeforeEvent
    ^ ((event == CV_EVENT_LBUTTONDOWN) || (event == CV_EVENT_LBUTTONUP));
  if(isLButtonPressedAfterEvent){
    prevPt = pt;
    isBrushDown = true;
  }else{
    isBrushDown = false;
  }
}

int main(int argc, char *argv[]){

  // 1. read image file
  char *filename = (argc >= 2) ? argv[1] : (char *)"fruits.jpg";
  original_image = cv::imread(filename);
  if(original_image.empty()){
    printf("ERROR: image not found!\n");
    return 0;
  }

  //print hot keys
  printf( "Hot keys: \n"
      "\tESC - quit the program\n"
      "\ti or ENTER - run inpainting algorithm\n"
      "\t\t(before running it, paint something on the image)\n");

  // 2. prepare window
  cv::namedWindow("image",1);

  // 3. prepare Mat objects for processing-mask and processed-image
  whiteLined_image = original_image.clone();
  inpainted = original_image.clone();
  inpaint_mask.create(original_image.size(), CV_8UC1);

  inpaint_mask = cv::Scalar(0);
  inpainted = cv::Scalar(0);

  // 4. show image to window for generating mask
  cv::imshow("image", whiteLined_image);

  // 5. set callback function for mouse operations
  cv::setMouseCallback("image", myMouseEventHandler2, 0);

  bool loop_flag = true;
  while(loop_flag){

    // 6. wait for key input
    int c = cv::waitKey(0);

    // 7. process according to input
    switch(c){
      case 27://ESC
      case 'q':
        loop_flag = false;
        break;

      case 'r':
        inpaint_mask = cv::Scalar(0);
        original_image.copyTo(whiteLined_image);
        cv::imshow("image", whiteLined_image);
        break;

      case 'i':
      case 10://ENTER
        cv::namedWindow("inpainted image", 1);
        cv::inpaint(whiteLined_image, inpaint_mask, inpainted, 3.0, cv::INPAINT_TELEA);
        cv::imshow("inpainted image", inpainted);
        break;
    }
  }
  return 0;
}
