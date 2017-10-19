/**
 * @file    isee_deepmar_attribute_recognition.hpp
 * @brief   The interface to do attribute recognition by calling the trained 
 *          caffe model which is provided by dangwei li.
 * @version 0.1 on 2017/10/18 (the source version is by kai yu)
 * @author  by da.li
 */

#ifndef _ISEE_DEEPMAR_ATTRIBUTE_RECOGNITION_HPP_
#define _ISEE_DEEPMAR_ATTRIBUTE_RECOGNITION_HPP_

#include <memory>
#include <boost/shared_ptr.hpp>

namespace caffe {
template<typename Dtype>
class Net;
template<typename Dtype>
class Blob;
}

namespace cripac {

/*
 * @class ISEEDeepMarAttrRecognition
 * @brief This version of DeepMAR model is based on ResNet50
 */

class ISEEDeepMarAttrRecognition {
  
 public:
  /*Constructor & Destructor*/
  ISEEDeepMarAttrRecognition(void) {
    input_width_ = 0;
    input_height_ = 0;
    gpu_index_ = -1;
  }
  ~ISEEDeepMarAttrRecognition(void) {
    release();
  }
  
  /* Dimensions of the feature. */
  const static int kFeatureDims = 1000;

  /**
   * @enum  ISEEDeepMarStatus
   * @brief Error types
   */
  enum ISEEDeepMarStatus {
    ISEE_DEEPMAR_OK = 0,
    ISEE_DEEPMAR_ILLEGAL_ARG = -1,
    ISEE_DEEPMAR_NO_INPUT_BLOB = -2,
  };

  /**
   * \brief Initialization.
   * \param[IN] proto_filename - path of the protofile
   * \param[IN] caffemodel_filename - path of caffe model
   * \param[IN] gpu_index - the selected gpu index (-1 for CPU_ONLY)
   * \param[IN] input_height - height of input
   * \param[IN] input_width  - width of input
   * \return the running status.
   */
  int initialize(const char *proto_filename,
                 const char *caffemodel_filename,
                 int gpu_index,
                 int input_height,
                 int input_width);

  /**
   * \brief Recognize attributes from a single image.
   * \param[IN] data - input_height x input_width x 3 input.
   * \return pointer to fc8 data.
   */
  const float* recognize(const float *data);

  /**
   * \brief Recognize attributes from an image batch.
   * \param[IN]  numImages: number of images in the batch
   * \param[IN]  data: numImages x input_height x input_width x 3 input
   * \return pointer to fc8 data.
   */
  const float* recognize(int numImages, const float *data[]);

  /**
   * Release the resources.
   */
  void release(void);

 private:
  // Set the index of selected gpu.
  int setDevice(int gpu_index);

  int input_width_;
  int input_height_;
  int gpu_index_;
  std::shared_ptr<caffe::Net<float> > net_;
};

}

#endif // _ISEE_DEEPMAR_ATTRIBUTE_RECOGNITION_HPP_
