//
// Created by ken.yu on 17-3-27.
//

#include <isee_deepmar_attribute_recognition.hpp>
#include <jni_isee_deepmar_attribute_recognition.h>

using namespace cripac;

/*
 * Class:     org_cripac_isee_alg_pedestrian_attr_DeepMARCaffeNative
 * Method:    initialize
 * Signature: (IIILjava/lang/String;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_org_cripac_isee_alg_pedestrian_attr_DeepMARCaffeNative_initialize
    (JNIEnv *env, jobject self, jint gpu_id, jint input_width, jint input_height, 
     jstring j_pb_path, jstring j_model_path) {
  ISEEDeepMarAttrRecognition *deepMAR = new ISEEDeepMarAttrRecognition();

  const int pb_len = env->GetStringUTFLength(j_pb_path);
  const int model_len = env->GetStringUTFLength(j_model_path);
  char *c_pb_path = new char[pb_len + 1];
  char *c_model_path = new char[model_len + 1];
  env->GetStringUTFRegion(j_pb_path, 0, pb_len, c_pb_path);
  env->GetStringUTFRegion(j_model_path, 0, model_len, c_model_path);
  c_pb_path[pb_len] = '\0';
  c_model_path[model_len] = '\0';

  assert(c_pb_path != nullptr);
  assert(c_model_path != nullptr);
  assert(pb_len > 0);
  assert(model_len > 0);
  int ret = deepMAR->initialize(c_pb_path, c_model_path, gpu_id, input_height, input_width);
  if (ret != ISEEDeepMarAttrRecognition::ISEE_DEEPMAR_OK) {
    static const char *className = "java/lang/RuntimeException";
    jclass exClass = env->FindClass(className);
    assert(exClass != nullptr);
    switch (ret) {
      case ISEEDeepMarAttrRecognition::ISEE_DEEPMAR_NO_INPUT_BLOB:
        return env->ThrowNew(exClass, "No input blob found!");
      default:
        break;
    }
  }

  delete[](c_model_path);
  delete[](c_pb_path);

  return (jlong) deepMAR;
}

/*
 * Class:     org_cripac_isee_alg_pedestrian_attr_DeepMARCaffeNative
 * Method:    free
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_org_cripac_isee_alg_pedestrian_attr_DeepMARCaffeNative_free
    (JNIEnv *env, jobject self, jlong net) {
  //free((ISEEDeepMarAttrRecognition *) net);
  ISEEDeepMarAttrRecognition* deepMAR = (ISEEDeepMarAttrRecognition *)net;
  if (deepMAR) {
    deepMAR->release();
  }
  delete deepMAR;
  deepMAR = NULL;
}

/*
 * Class:     org_cripac_isee_alg_pedestrian_attr_DeepMARCaffeNative
 * Method:    recognize
 * Signature: (J[F[F)V
 */
JNIEXPORT void JNICALL Java_org_cripac_isee_alg_pedestrian_attr_DeepMARCaffeNative_recognize__J_3F_3F
    (JNIEnv *env, jobject self, jlong net, jfloatArray j_input, jfloatArray output) {
  ISEEDeepMarAttrRecognition *deepMAR = (ISEEDeepMarAttrRecognition *) net;
  float *c_input = env->GetFloatArrayElements(j_input, nullptr);
  env->SetFloatArrayRegion(output, 0, env->GetArrayLength(output), deepMAR->recognize(c_input));
  env->ReleaseFloatArrayElements(j_input, c_input, 0);
}

/*
 * Class:     org_cripac_isee_alg_pedestrian_attr_DeepMARCaffeNative
 * Method:    recognize
 * Signature: (J[[F[[F)V
 */
JNIEXPORT void JNICALL Java_org_cripac_isee_alg_pedestrian_attr_DeepMARCaffeNative_recognize__J_3_3F_3_3F
    (JNIEnv *env, jobject self, jlong net, jobjectArray j_input, jobjectArray output) {
  ISEEDeepMarAttrRecognition *deepMAR = (ISEEDeepMarAttrRecognition *) net;

  int num_images = env->GetArrayLength(j_input);
  float **inputs = new float *[num_images];
  for (int i = 0; i < num_images; ++i)
    inputs[i] = env->GetFloatArrayElements((jfloatArray) env->GetObjectArrayElement(j_input, i), nullptr);

  const float *fc8 = deepMAR->recognize(num_images, (const float **)inputs);
  for (int i = 0; i < num_images; ++i) {
    jfloatArray slice = (jfloatArray) env->GetObjectArrayElement(output, i);
    env->SetFloatArrayRegion(slice, 0, env->GetArrayLength(slice), fc8);
    fc8 += ISEEDeepMarAttrRecognition::kFeatureDims;
  }

  for (int i = 0; i < num_images; ++i)
    env->ReleaseFloatArrayElements((jfloatArray) env->GetObjectArrayElement(j_input, i), inputs[i], 0);
  delete[] inputs;
}
