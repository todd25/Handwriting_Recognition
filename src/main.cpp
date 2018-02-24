#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include "mnist_reader.hpp"
#include "mnist_utils.hpp"
#include "bitmap.hpp"
#include <sstream>
#define MNIST_DATA_DIR "../mnist_data"

using namespace std;

int main(int argc, char* argv[]) {
    //Read in the data set from the files
    mnist::MNIST_dataset<std::vector, std::vector<uint8_t>, uint8_t> dataset =
    mnist::read_dataset<std::vector, std::vector, uint8_t, uint8_t>(MNIST_DATA_DIR);
    //Binarize the data set (so that pixels have values of either 0 or 1)
    mnist::binarize_dataset(dataset);
    //There are ten possible digits 0-9 (classes)
    int numLabels = 10;
    //There are 784 features (one per pixel in a 28x28 image)
    int numFeatures = 784;
    //Each pixel value can take on the value 0 or 1
    int numFeatureValues = 2;
    //image width
    int width = 28;
    //image height
    int height = 28;
    //image to print (these two images were randomly selected by me with no particular preference)
    int trainImageToPrint = 50;
    int testImageToPrint = 5434;
    // get training images
    std::vector<std::vector<unsigned char>> trainImages = dataset.training_images;
    // cout<< trainImages.size() << endl;
    // cout<< trainImages[0].size()<<endl;


    // get training labels
    std::vector<unsigned char> trainLabels = dataset.training_labels;
    // get test images
    std::vector<std::vector<unsigned char>> testImages = dataset.test_images;
    // get test labels
    std::vector<unsigned char> testLabels = dataset.test_labels;

    std::vector<int> numofc(10);
    double total;
    for (int i =0; i<10 ; i++)
    {
      int counter = 0;
      for (int j =0; j<trainLabels.size(); j++)
      {
        // cout<<static_cast<int>(trainLabels[j])<<endl;
        if (static_cast<int>(trainLabels[j]) == i)
        {
          counter++;
        }
      }

      // cout<<"Prior Probability"<<i<<" "<<double(counter)/double(trainLabels.size())<<endl;
      total += double(counter)/double(trainLabels.size());
      numofc[i] = counter;
    }
    // cout<<"total: " <<total<<endl;

    std::vector<std::vector<double>> conditional (10, std::vector<double>(trainImages[0].size(),1));
    // cout<<"size check "<<conditional.size()<<" "<<conditional[0].size()<<endl;
    for (int i=0; i<trainImages.size(); i++)
    {
      int c = static_cast<int>(trainLabels[i]);
      for (int j=0; j<trainImages[0].size(); j++)
      {
          if (static_cast<int>(trainImages[i][j]) == 1)
          {
            conditional[c][j]++;
          }
      }
    }
    double total2 = 0;
    for (int i=0; i<10; i++)
    {
        total2 += conditional[i][trainImages[0].size()/2.5];
    }
    // cout<<"probability check "<<total2<<endl;



    std::vector<std::vector<int>> ten(10,std::vector<int>(10,0));


    int count = 0;
    for (int k=0; k<testImages.size(); k++)
    {
      std::vector<double> allC(10);
      for (int j=0; j<10; j++)
      {
        double pl = 0;
        double pc = double(numofc[j])/double(trainImages.size());
        for (int i=0; i< testImages[0].size(); i++)
        {
          if (static_cast<int>(testImages[k][i]) == 1)
          {
            pl += log(conditional[j][i]/(numofc[j]+2));
            // cout<<log(conditional[j][i]/(numofc[0]+2))<<endl;
          }
          else
          {
            pl += log(1.0-(conditional[j][i]/(numofc[j]+2)));
          }
        }
        double prob = pl + log(pc);
        allC[j] = prob;
      }
      std::vector<double>::iterator x = std::max_element(allC.begin(),allC.end());

      // cout<<std::distance(allC.begin(), x)<<endl;
      // cout<<static_cast<int>(testLabels[3])<<endl;
      if ((std::distance(allC.begin(), x)) == static_cast<int>(testLabels[k]))
      {
        count++;
        ten[(std::distance(allC.begin(), x))][(std::distance(allC.begin(), x))]++;
      }
      else
      {
        ten[static_cast<int>(testLabels[k])][(std::distance(allC.begin(), x))] ++;
      }
    }
    // cout<<count<<endl;
    // cout<<"correct rate: "<<double(count)/double(testImages.size())<<endl;





    for (int c=0; c<numLabels; c++) {
        std::vector<unsigned char> classFs(numFeatures);
        for (int f=0; f<numFeatures; f++) {
            double p = conditional[c][f]/(numofc[c]+2);
            uint8_t v = 255*p;
            classFs[f] = (unsigned char)v;
          }
        std::stringstream ss;
        ss << "../output/digit" <<c<<".bmp";
        Bitmap::writeBitmap(classFs, 28, 28, ss.str(), false);
    }

    // cout<<allC.find(std::max_element(allC,allC+10))<<endl;
    // cout<< "probability of test that is 0 "<<prob<<endl;

    //print out one of the training images
    // for (int f=0; f<numFeatures; f++) {
    //     // get value of pixel f (0 or 1) from training image trainImageToPrint
    //     int pixelIntValue = static_cast<int>(trainImages[trainImageToPrint][f]);
    //     if (f % width == 0) {
    //         std::cout<<std::endl;
    //     }
    //     std::cout<<pixelIntValue<<" ";
    // }
    // std::cout<<std::endl;
    // print the associated label (correct digit) for training image trainImageToPrint
    // std::cout<<"Label: "<<static_cast<int>(trainLabels[trainImageToPrint])<<std::endl;



    //print out one of the test images
    // for (int f=0; f<numFeatures; f++) {
    //     // get value of pixel f (0 or 1) from training image trainImageToPrint
    //     int pixelIntValue = static_cast<int>(testImages[testImageToPrint][f]);
    //     if (f % width == 0) {
    //         std::cout<<std::endl;
    //     }
    //     std::cout<<pixelIntValue<<" ";
    // }
    // std::cout<<std::endl;
    // print the associated label (correct digit) for test image testImageToPrint





    // std::cout<<"Label: "<<static_cast<int>(testLabels[testImageToPrint])<<std::endl;
    std::vector<unsigned char> trainI(numFeatures);
    std::vector<unsigned char> testI(numFeatures);
    for (int f=0; f<numFeatures; f++) {
        int trainV = 255*(static_cast<int>(trainImages[trainImageToPrint][f]));
        int testV = 255*(static_cast<int>(testImages[testImageToPrint][f]));
        trainI[f] = static_cast<unsigned char>(trainV);
        testI[f] = static_cast<unsigned char>(testV);
    }
    std::stringstream ssTrain;
    std::stringstream ssTest;
    ssTrain << "../output/train" <<trainImageToPrint<<"Label"<<static_cast<int>(trainLabels[trainImageToPrint])<<".bmp";
    ssTest << "../output/test" <<testImageToPrint<<"Label"<<static_cast<int>(testLabels[testImageToPrint])<<".bmp";
    Bitmap::writeBitmap(trainI, 28, 28, ssTrain.str(), false);
    Bitmap::writeBitmap(testI, 28, 28, ssTest.str(), false);


    ofstream myfile;
    myfile.open ("network.txt");
    for (int i=0; i<10;i++)
    {
      for (int j=0; j<conditional[0].size(); j++)
      {
        myfile << double(conditional[i][j])/double(numofc[i]+2) <<"\n";
      }
    }
    for (int i=0; i<10;i++)
    {
      myfile<< double(numofc[i])/double(trainImages.size())<<"\n";
    }
    myfile.close();

    myfile.open ("classification-summary.txt");



    for(int i=0; i<10;i++)
    {
      for (int j=0; j<10;j++)
      {
        myfile << ten[i][j]<<"    " ;
      }
      myfile << "\n" ;
    }
    myfile << double(count)/double(testImages.size());
    myfile.close();



}
