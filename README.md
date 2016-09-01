# imageProcessing
SETUP:

    git clone https://github.com/schlesingerphilipp/imageProcessing.git
    
    cd imageProcessing
    
    mkdir build
    
    cd build
    
    cmake ..
    
    make
    
USAGE:

  There are some example pictures in the images folder
  
  There are three functions
  
    -fields: Compute the valley, peak and edge representation of a given image and draw these into the folder "images/results"
    
    -localization: Calculates a valley representation of an given image. 
      Finds the maximas of the valley representation and marks them with red dots on an image in "images/results"
    
    -fit calculates the representations, localizes maxima in the valley and fits a circle at each localization to the valley
  
  Example:
    (in build directory)
    ./main fields ../images/eye.png
  
