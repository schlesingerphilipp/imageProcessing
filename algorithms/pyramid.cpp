#include <vigra/multi_array.hxx>
#include "pyramid.h"
#include <math.h>
#include <vigra/resizeimage.hxx>
Pyramid::Pyramid(MultiArray<2, float > image):original(image),origHeight(image.height()),origWidth(image.width()){};
MultiArray<2, float > Pyramid::get(int level)
{
    int factor(pow(2, level + 1));
    int newwidth((origWidth) / factor);
    int newheight((origHeight) / factor);

    // resize result image to appropriate size
    MultiArray<2, float > resized(Shape2(newwidth, newheight));
    resizeImageNoInterpolation(original, resized);
    return resized;  
}
MultiArray<2, float > Pyramid::toOriginalSize(MultiArray<2, float > smaller)
{
    MultiArray<2, float > resized(original.shape());
    resizeImageNoInterpolation(smaller, resized);
    return resized;
}
