/*

Author: Yves Pauchard, yves.pauchard@zhaw.ch
Date: Nov 5, 2013
Description: Adaptation of the 2D RGB version by David Doria to 3D single value images.


Based on ImageGraphCut.h:
Copyright (C) 2012 David Doria, daviddoria@gmail.com

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ImageGraphCut3D_H
#define ImageGraphCut3D_H


// ITK
#include "itkImage.h"
#include "itkSampleToHistogramFilter.h"
#include "itkHistogram.h"
#include "itkListSample.h"

// STL
#include <vector>

// Kolmogorov's code
#include "Kolmogorov/graph.h"

typedef Graph GraphType;

/** Perform graph cut based segmentation on a 3D image. Image pixels can contain only
* one component, i.e. grayscale.
*/
template<typename TImage>
class ImageGraphCut3D {
public:

    //YP constructor now in .hpp; was: ImageGraphCut3D(){}
    ImageGraphCut3D();

    /** This is the results image as uchar. */
    typedef itk::Image<unsigned char, 3> ResultImageType;

    /** This is a special type to keep track of the graph node labels. */
    typedef itk::Image<void *, 3> NodeImageType;

    /** The type of the histograms. */
    typedef itk::Statistics::Histogram<short,
            itk::Statistics::DenseFrequencyContainer2> HistogramType;

    /** The type of a list of pixels/indexes. */
    typedef std::vector<itk::Index<3> > IndexContainer;

    /** Type of input image pixels */
    typedef typename TImage::PixelType PixelType;

    /** Several initializations are done here. */
    void SetImage(TImage *const image);

    /** Get the image that we are segmenting. */
    TImage *GetImage();

    /** Create and cut the graph (The main driver function). */
    void PerformSegmentation();

    /** Return a list of the selected (via scribbling) pixels. */
    IndexContainer GetSources();

    IndexContainer GetSinks();

    /** Set the selected pixels. */
    void SetSources(const IndexContainer &sources);

    void SetSinks(const IndexContainer &sinks);

    /** Get the output of the segmentation. */
    ResultImageType::Pointer GetSegmentMask();

    /** Use the region term based on threshold. */
    void UseRegionTermBasedOnThresholdOn();

    /** DON'T use the region term based on threshold (default). */
    void UseRegionTermBasedOnThresholdOff();


    /** Use the region term based on log histogram. */
    void UseRegionTermBasedOnHistogramOn();

    /** DON'T use the region term based on log histogram (default). */
    void UseRegionTermBasedOnHistogramOff();

    /** Set the weight between the regional and boundary terms. */
    void SetLambda(const float);

    /** Set sigma for boundary term. */
    void SetSigma(const double);

    /** Set the number of bins per dimension of the foreground and background histograms. */
    void SetNumberOfHistogramBins(const int);

    /** Set threshold for threshold based region term */
    void SetRegionThreshold(PixelType);

    /** Enums used to specify Boundary term direction */
    typedef enum {
        NoDirection, BrightDark, DarkBright
    } BoundaryDirectionType;
    void SetBoundaryDirectionTypeToNoDirection();
    void SetBoundaryDirectionTypeToBrightDark();
    void SetBoundaryDirectionTypeToDarkBright();

    void SetLogToStd(bool);

protected:
    // Typedefs
    typedef itk::Vector<PixelType, 1> ListSampleMeasurementVectorType;
    typedef itk::Statistics::ListSample<ListSampleMeasurementVectorType> SampleType;
    typedef itk::Statistics::SampleToHistogramFilter<SampleType, HistogramType> SampleToHistogramFilterType;

    // members
    GraphType                   *m_Graph;               // kolmogorov graph object
    typename TImage::Pointer    m_Image;                // input image
    NodeImageType::Pointer      m_NodeImage;            // mapping pixel index -> graph node id
    ResultImageType::Pointer    m_ResultingSegments;    // resulting segmentation
    IndexContainer              m_Sources;              // foreground pixel indices
    IndexContainer              m_Sinks;                // background pixel indices

    // parameters
    double m_Sigma;                         // noise in boundary term
    float m_Lambda;                         // weighting between region and boundary terms
    bool m_UseRegionTermBasedOnHistogram;
    bool m_UseRegionTermBasedOnThreshold;   // simple threshold probablity (see sect. 2.3 in Boykov and Funka-Lea 2006)
    int m_NumberOfHistogramBins;            // number of bins per dimension of the foreground and background histograms
    PixelType m_RegionThreshold;            // Threshold for threshold based region term
    BoundaryDirectionType m_BoundaryDirectionType; // Direction of the Boundary term

    // member functions
    void CreateSamples();       // create histograms from the users selections
    double ComputeNoise();      // Estimate the "camera noise"
    void CreateGraph();         // Create a Kolmogorov graph structure from the image and selections
    void CutGraph();            // Perform the s-t min cut

    /** The ITK data structure for storing the values that we will compute the histogram of. */
    typename SampleType::Pointer m_ForegroundSample;
    typename SampleType::Pointer m_BackgroundSample;

    /** The histograms. */
    const HistogramType *m_ForegroundHistogram;
    const HistogramType *m_BackgroundHistogram;


    /** ITK filters to create histograms. */
    typename SampleToHistogramFilterType::Pointer m_ForegroundHistogramFilter;
    typename SampleToHistogramFilterType::Pointer m_BackgroundHistogramFilter;

    /** The image to be segmented */


private:
    // in-class initializations of dependent types is only possible in >=C++11.
    // to ensure backward compatibility, the initialization is done in the constructors initialization list
    const typename ResultImageType::PixelType RESULT_FOREGROUND_PIXEL_VALUE;
    const typename ResultImageType::PixelType RESULT_BACKGROUND_PIXEL_VALUE;
    bool m_LogToStd;

};

#include "ImageGraphCut3D.hpp"

#endif
