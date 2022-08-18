#ifndef TREE_FILTER_H
#define TREE_FILTER_H

#include <opencv2/opencv.hpp>
#include <SalienceTree.h>
#include <iostream>


using cv::Vec;
using cv::Mat;

template <typename chType, int nCh>
class AverageFilter{

	typedef Vec<double, nCh> Vect;


	private:
		Vect *nodeColours;
		int *sets;
		Mat image;
		int imgsize;
		SalienceTree *tree;
		double lambda_prev = -1;

		int NO_SET = -1;

	public:

		void colourNodes(){
			for(int i = imgsize; i < tree->curSize; i++){
				nodeColours[i] = Vect::zeros();
			}

			for(int i = 0; i < tree->curSize; i++){
				if(i < imgsize){
					nodeColours[i] = (Vect) image.at<Vec<chType, nCh>>(getPoint(i, image));
				}else{
					nodeColours[i] /= tree->nodes[i].area;
				}
				int parent = tree->nodes[i].parent;
				if (parent == BOTTOM)
					continue;
				nodeColours[parent] += nodeColours[i]*tree->nodes[i].area;
			}
		}

		void resetSets(){
			for(int i = 0; i < tree->curSize; i++){
				sets[i] = NO_SET;
			}
		}

		AverageFilter(SalienceTree *tr, Mat im){
			imgsize = im.cols*im.rows;
			image = im;
			tree = tr;

			nodeColours = (Vect*) malloc(tree->curSize*sizeof(Vect));
			sets = (int *) malloc(tree->curSize*sizeof(int));
			resetSets();

			colourNodes();
		}

		int findLambdaNode(int i, double lambda){
			int parent;
			if(sets[i] != NO_SET){
				parent = sets[i];
			}
			else{
				parent = tree->nodes[i].parent;
			}
			if(parent == BOTTOM)
				return i;
			if(tree->nodes[parent].alpha > lambda)
				return i;
			int lambdaNode = findLambdaNode(parent, lambda);
			sets[i] = lambdaNode;
			return lambdaNode;
		}

		Mat filter(double lambda){
			if(lambda < lambda_prev){
				std::cerr << "filtering at lambda=" << lambda << " after filtering at lambda=" << lambda_prev << "\n"; 
				std::cerr << "WARNING: filtering at ascending values of lambda is faster\n";
				resetSets();
			}
			lambda_prev = lambda;

			Mat result = Mat::zeros(image.rows, image.cols, CV_64FC(nCh));
			for(int i = 0; i < imgsize; i++){
				int n = findLambdaNode(i, lambda);
				result.at<Vect>(getPoint(i, image)) = nodeColours[n];
			}
			return result;
		}

		~AverageFilter(){
			free(sets);
			free(nodeColours);
		}
};

#endif