#include <stdlib.h>
#include<iostream>
#include <fstream>
#include <string>
#include <math.h>


struct coordinates {
	int x;
	int y;
};

unsigned int getNumLines(std::ifstream &);
void populateArrays(int, std::ifstream &, int *, int *);
void kMeans(int *, int *, int *, std::ifstream &, int, int);

int main(int argc, char ** argv){
	int *x, *y, *cluster;
	int numLines = 0, numClusters;
	std::string text;
	std::string fileName;

	if(argc != 3){
		std::cout << "Expecting exactly 2 arguments. Exiting..." << std::endl;
		exit(-1);
	}

	// Get terminal args 
	numClusters = std::stoi(argv[1]);
	fileName = argv[2];

	//std::cout << "Num clusters: " << numClusters << ", File name: " << fileName << std::endl;


	std::ifstream inputFile;
	inputFile.open(fileName);
	if (inputFile.is_open()){

		numLines = getNumLines(inputFile);

		x = new int[numLines];
		y = new int[numLines];
		cluster = new int[numLines];

		populateArrays(numLines, inputFile, x, y);


		/*
		for (int i = 0; i < numLines; i++){
			std::cout << x[i] << ", " << y[i] << std::endl;
		}
		*/

	}else{
		std::cout << "Could not open file. Exiting..." << std::endl;
		exit(-1);
	}

	kMeans(x, y, cluster, inputFile, numLines, numClusters);

    return 0;
}


// Get number of data points from file
unsigned int getNumLines(std::ifstream & inputFile){
	unsigned int numLines = 0;
	std::string text;

	while (std::getline(inputFile, text)){
		numLines++;
	}

	inputFile.clear();
	inputFile.seekg(0);

	return numLines;
}

// Populate arrays with the data points
void populateArrays(int numLines, std::ifstream & inputFile, int * x, int * y){
	unsigned int separator, counter = 0;
	std::string text;

	while(std::getline(inputFile, text)){
		separator = text.find('\t');
		x[counter] = std::stoi(text.substr(0, separator));
		y[counter] = std::stoi(text.substr(separator + 1, text.length() - 1));
		counter ++;
	}
}

// K-means operation
void kMeans(int *x, int *y, int *cluster, std::ifstream & inputFile, int numPoints, int numClusters){

	if (numClusters > numPoints){
		std::cout << "Number of clusters must be less than number of data points. Exiting" << std::endl;
	}

	int pointsPerCluster = 0;
	int remainder = 0;
	int currentDistance, tempDistance;

	bool repeat = true;
	bool centroidChange;
	bool pointChange;

	// Actual and temp centroids
	coordinates centroids[numClusters]; 
	coordinates centroidsTemp[numClusters];

	int counter[numClusters];

	if (numClusters > 1){
		pointsPerCluster = numPoints / numClusters;
		remainder = numPoints % numClusters;
		
		// Assign initial clusters
		for (int i = 0; i < numClusters; i++){
			for(int j = pointsPerCluster * i; j < (pointsPerCluster * (i + 1)); j++){
				cluster[j] = i;
			}
		}

		// If number of points is not divisible by number of clusters, assign remaining points to the last cluster
		if (remainder > 0){
			for (int l = numPoints - remainder; l < numPoints; l++){
				cluster[l] = numClusters - 1;
			}
		}

		/*
		for (int k = 0; k < numPoints; k++){
			std::cout << cluster[k] << ", "; 
		}*/

		// calculate first set of centroids
		for (int i = 0; i < numClusters; i++){
			centroids[i].x = 0;
			centroids[i].y = 0;
			centroidsTemp[i].x = 0;
			centroidsTemp[i].y = 0;
		}
		


		while (repeat){
			pointChange = false;
			centroidChange = false;

			// set counter to zero
			for (int i = 0; i < numClusters; i++){
				counter[i] = 0;
				centroidsTemp[i].x = 0;
				centroidsTemp[i].y = 0;
			}

			// Compute new centroids
			for (int i = 0; i < numPoints; i++){
				centroidsTemp[cluster[i]].x += x[i];
				centroidsTemp[cluster[i]].y += y[i];
				counter[cluster[i]]++;
			}
			for(int i = 0; i < numClusters; i++){
				//std::cout << "Counter: " << counter[i] << std::endl;
				if (counter[i] != 0){
					centroidsTemp[i].x = centroidsTemp[i].x / counter[i];
					centroidsTemp[i].y = centroidsTemp[i].y / counter[i];
				}

				//std::cout << "Centroid " << i << " - x: " << centroidsTemp[i].x << ", y: " << centroidsTemp[i].y << std::endl; 
			}

			//std::cout << "copy arrays" << std::endl;
			// Copy over to the centroid array and check for changes
			for (int i = 0; i < numClusters; i++){
				if (centroids[i].x != centroidsTemp[i].x && centroids[i].y != centroidsTemp[i].y)
					centroidChange = true;
				
				centroids[i].x = centroidsTemp[i].x;
				centroids[i].y = centroidsTemp[i].y;
			}

			// Reassign points to centroids
			for (int i = 0; i < numPoints; i++){
				currentDistance = sqrt(((centroids[cluster[i]].x - x[i]) * (centroids[cluster[i]].x - x[i])) + ((centroids[cluster[i]].y - y[i]) * (centroids[cluster[i]].y - y[i])));
				//std::cout << "Current distance from centroid " << cluster[i] << " " << centroids[cluster[i]].x << ", " << centroids[cluster[i]].y << ": " << currentDistance << std::endl;
				for (int j = 0; j < numClusters; j++){
					if (j == cluster[i])
						continue;
					
					tempDistance = sqrt(((centroids[j].x - x[i]) * (centroids[j].x - x[i])) + ((centroids[j].y - y[i]) * (centroids[j].y - y[i])));
					//std::cout << "Distance from centroid " << j << " " << centroids[j].x << ", " << centroids[j].y << ": " << tempDistance << std::endl;

					if (currentDistance > tempDistance){
						cluster[i] = j;
						currentDistance = tempDistance;

						pointChange = true;

						//std::cout << "Re-assigned to centroid " << j << std::endl;
					}
					
				}
			}


			if (!centroidChange && !pointChange)
				repeat = false;
		}

		std::ofstream outputfile;
		outputfile.open("output.txt");

		for (int i = 0; i < numPoints; i++){
			outputfile << x[i] << "\t" << y[i] << "\t" << cluster[i] + 1 << std::endl;
			
			//std::cout << centroids[cluster[i]].x << ", " << centroids[cluster[i]].y << std::endl;
		}
		
		/*for (int j = 0; j < numClusters; j++){
			outputfile << centroids[j].x << "\t" << centroids[j].y << "\t" << numClusters + 2 << std::endl; 
		}*/
		
		/*for (int j = 0; j < numClusters; j++){
			std::cout << "Values for last calculated centroids: " << std::endl;
			std::cout << centroids[j].x <<  ", " << centroids[j].y << std::endl;
		}*/

	}else{
		std::cout << "Number of clusters must be greater than 1. Exiting..." << std::endl;
		exit(-1);
	}
}
