#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

/**Compute distance between two points*/

 float Distance(Point p1, Point p2){
	 int dx = abs(p2.x - p1.x);
	 //~ cout << "Dx " << dx << endl;
	 int dy = abs(p2.y - p1.y);
	 //~ cout << "Dy " << dy << endl;
	 return sqrt(dx*dx + dy*dy);
 }
 
 /** Rotate image to align eyes*/
 Mat rotate(Mat& image, double angle, Point centre){
	Point2f src_center(centre.x, centre.y);
	angle = angle*180.0/3.14157;
	Mat rot_matrix = getRotationMatrix2D(src_center, angle, 1.0);
	 
	Mat rotated_image(Size(image.size().height, image.size().width), 
		image.type());
		
	warpAffine(image, rotated_image, rot_matrix, image.size());
	
	return(rotated_image);
 }

int main(int argc, const char** argv){
	
	if (argc!=9){
		cout << "Usage: ./af path_to_image.ext offset_percentage "
				"cropped_square_face_image_size right_eye_x_location "
				"right_eye_y_location left_eye_x_location "
				"left_eye_y_location filename_of_output_image" << endl << endl;
		cout << "Example: ./af picture.jpg 0.2 200 197 254 150 251 output.pgm" 
			 << endl;
		exit(0);
	}
	
/**-------------------------Initialize inputs-------------------------*/
	
	//Read the image file into Mat frame
	Mat frame = imread(argv[1]);
	Mat image = frame.clone();	
	Mat gray;
	cvtColor(image, gray, CV_BGR2GRAY);
	equalizeHist(gray, gray);
		
	//Offset percentage
	Point offset_pct;
	offset_pct.x = atof(argv[2])*100;
	offset_pct.y = offset_pct.x;
	
	//Size of new picture
	Point dest_sz;
	dest_sz.x = atoi(argv[3]);
	dest_sz.y = dest_sz.x;
	
	//Right eye location
	Point eye_right;
	eye_right.x = atoi(argv[4]);
	eye_right.y = atoi(argv[5]);
	
	//Left eye location
	Point eye_left;
	eye_left.x = atoi(argv[6]);
	eye_left.y = atoi(argv[7]);
	
	circle(frame, eye_right, 4, Scalar(0,255,255), -1, 8, 0);
	circle(frame, eye_left, 4, Scalar(0,255,255), -1, 8, 0);
	line(frame, eye_right, eye_left, Scalar(255,0,0), 2, 8, 0);
	
	//Name of output image
	string outputName = argv[8];	
	vector<int> compression_params;
    compression_params.push_back(IMWRITE_PXM_BINARY);
	
/**------------------------Align, rotate, crop------------------------*/
	
	//Calculate offsets in original image
	int offset_h = (offset_pct.x*dest_sz.x/100);
	int offset_v = (offset_pct.y*dest_sz.y/100);
	
	//Get the direction
	Point eye_direction;
	eye_direction.x = eye_right.x - eye_left.x;
	cout << "X: " << eye_direction.x;
	eye_direction.y = eye_right.y - eye_left.y;
	cout << " Y: " << eye_direction.y << endl;
	
	//Calculate rotation angle in radians
	float rotation = atan2((float)(eye_direction.y),
		(float)(eye_direction.x));
	cout << "Rotation: " << rotation  << " radians" << endl;
			
	//Distance between them
	float dist = Distance(eye_left, eye_right);
	cout << "Distance between eyes: " << dist << endl;
		
	//Calculate the reference eye-width
	int reference = dest_sz.x - 2*offset_h;
		
	//Scale factor
	float scale = dist/(float)reference;
	printf("Scale: %f\n", scale);
		
	//Rotate original image around the left eye
	Mat frameRot = rotate(gray, (double)rotation, eye_left);
	if (frame.rows > 400){
		namedWindow("Original image", 0);
		resizeWindow("Original image", 400, 400);
		
		namedWindow("Rotated image", 0);
		resizeWindow("Rotated image", 400, 400);
	}	
	
	imshow("Rotated image", frameRot);
	//~ imwrite("data2.jpg",frameRot);
	
	//Crop the rotated image
	Point crop_xy;		//Top left corner coordinates
	crop_xy.x = eye_left.x - scale*offset_h;
	crop_xy.y = eye_left.y - scale*offset_v;
	//~ cout << "Cropped image TL.x: " << crop_xy.x << endl;
	//~ cout << "Cropped image TL.y: " << crop_xy.x << endl;
		
	Point crop_size;
	crop_size.x = dest_sz.x*scale;	//Cropped image width
	crop_size.y = dest_sz.y*scale;	//Cropped image height
	//~ cout << "Cropped image width: " << crop_size.x << endl;
	//~ cout << "Cropped image height: " << crop_size.y << endl;
		
	//Crop the full image 
	Rect myROI(crop_xy.x, crop_xy.y, crop_size.x, crop_size.y);
	if((crop_xy.x + crop_size.x < frameRot.size().width) &&
		(crop_xy.y + crop_size.y < frameRot.size().height)){
			frameRot = frameRot(myROI);
		}
	else{
		cout << "Error cropping" << endl;
		exit(0);
	}
	
	//Resize
	resize(frameRot, frameRot, Size(dest_sz));	
	
	//Save output frame
	imwrite(outputName, frameRot, compression_params);
		
	cout << endl << "Face alignment successful! Output image saved as "
		 << endl << outputName << " in the current directory." << endl;
	
	cout << frame.rows << endl;
	
	//~ imwrite("data1.jpg",frame);
	
	for (;;){
		
		imshow("Original image", frame);			
		imshow("Cropped image",frameRot);
				
		int c = waitKey(27);
		if (27 == char(c)){
			break;
		}
	}
	
	exit(0);
}
