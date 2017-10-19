/**
 * \file  isee_deepmar_attribute_recognition.cpp
 * \brief Implement the interfaces.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cassert>

#include <vector>
#include <string>

#include <caffe/caffe.hpp>

using namespace std;
using namespace caffe;

#include "isee_deepmar_attribute_recognition.hpp"

namespace cripac {

/*Initialization*/
int ISEEDeepMarAttrRecognition::initialize(const char *proto_filename,
                                           const char *caffemodel_filename,
                                           int gpu_index,
                                           int input_height,
                                           int input_width) {
  // Input checking.
  if (proto_filename == NULL) {
    fprintf(stderr, "Error: proto_filename is NULL\n");
    return ISEE_DEEPMAR_ILLEGAL_ARG;
  }
  if (caffemodel_filename == NULL) {
    fprintf(stderr, "Error: caffemodel_filename is NULL\n");
    return ISEE_DEEPMAR_ILLEGAL_ARG;
  }
  if (input_width < 0 || input_height < 0) {
    fprintf(stderr, "Error: BAD image information!\n");
    return ISEE_DEEPMAR_ILLEGAL_ARG;
  }

  input_width_ = input_width;
  input_height_ = input_height;
  gpu_index_ = gpu_index;

  // Set gpu.
  setDevice(gpu_index_);

  // Load the network.
  fprintf(stdout, "Loading protocol from %s...\n", proto_filename);
  net_.reset(new Net<float>(proto_filename, TEST));
  fprintf(stdout, "Loading caffemodel from %s...\n", caffemodel_filename);
  net_->CopyTrainedLayersFrom(caffemodel_filename);
  fprintf(stdout, "Managing I/O blobs...\n");
  vector<Blob<float> *> input_blobs = net_->input_blobs();
  if (input_blobs.size() == 0) {
    return ISEE_DEEPMAR_NO_INPUT_BLOB;
  }

  return ISEE_DEEPMAR_OK; 
}

/* Attributes recognition. */
// Single image.
const float* ISEEDeepMarAttrRecognition::recognize(const float* data) {
  // Input checking.
  assert(data != NULL);

  // Set device.
  setDevice(gpu_index_);

  // Get features.
  const char* layer_name = "fine_fc8";
  Blob<float> *input_blob = net_->input_blobs()[0];
  input_blob->Reshape(1, 3, input_height_, input_width_);
  boost::shared_ptr<caffe::Blob<float> > output_blob = 
    net_->blob_by_name(layer_name);
  float *dst = input_blob->mutable_cpu_data();
  int data_size = input_height_ * input_width_ * 3;
  memcpy(dst, data, sizeof(float) * data_size);

  net_->Forward();
  return output_blob->cpu_data();
}

// Image batch.
const float* ISEEDeepMarAttrRecognition::recognize(int numImages, 
                                                   const float *data[]) {
  // Input checking.
  assert(data != NULL);

  // Set device.
  setDevice(gpu_index_);

  // Get features.
  const char* layer_name = "fine_fc8";
  Blob<float> *input_blob = net_->input_blobs()[0];
  input_blob->Reshape(numImages, 3, input_height_, input_width_);
  boost::shared_ptr<caffe::Blob<float> > output_blob = 
    net_->blob_by_name(layer_name);
  float *dst = input_blob->mutable_cpu_data();
  int data_size = input_height_ * input_width_ * 3;
  for (int i = 0; i < numImages; ++i) {
    memcpy(dst, data[i], sizeof(float) * data_size);
    dst += data_size;
  }
  net_->Forward();
  return output_blob->cpu_data();
}

/* Release. */
void ISEEDeepMarAttrRecognition::release(void) {
  net_.reset();
}

/* Set Device. */
int ISEEDeepMarAttrRecognition::setDevice(int gpu_index) {
  if (gpu_index < 0) {
    Caffe::set_mode(Caffe::CPU);
  } else {
    Caffe::set_mode(Caffe::GPU);
    Caffe::SetDevice(gpu_index);
    //Caffe::DeviceQuery();
  }
  
  return ISEE_DEEPMAR_OK;
}

}

