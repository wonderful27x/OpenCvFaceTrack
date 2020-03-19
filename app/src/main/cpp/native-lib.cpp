#include <jni.h>
#include <string>
#include <pthread.h>
#include <android/native_window_jni.h>
#include "androidlog.h"
#include "WonderfulOpenCv.h"

ANativeWindow *window = nullptr;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;//静态初始化
WonderfulOpenCv *wonderfulOpenCv = nullptr;

//渲染
void renderFrame(uint8_t *data, int width, int height) {
    pthread_mutex_lock(&mutex);
    if (!window) {
        pthread_mutex_unlock(&mutex);
        return;
    }
    //初始化窗口属性
    //注意：这里的宽高是指图像的宽高
    ANativeWindow_setBuffersGeometry(window, width, height, WINDOW_FORMAT_RGBA_8888);
    ANativeWindow_Buffer buffer;
    if (ANativeWindow_lock(window, &buffer, nullptr)) {
        ANativeWindow_release(window);
        window = nullptr;
        return;
    }
    uint8_t *windowData = static_cast<uint8_t *>(buffer.bits);

    //原始数据中一行rgba图像的字节数
    int lineSize = width * 4;
    //窗口中一行图像的字节数
    int windowDataLineSize = buffer.stride * 4;
    //内存拷贝，逐行拷贝
    for (int i = 0; i < buffer.height; ++i) {
        memcpy(windowData + i * windowDataLineSize, data + i * lineSize, windowDataLineSize);
    }

    ANativeWindow_unlockAndPost(window);
    pthread_mutex_unlock(&mutex);
}
//初始化，人脸模型路径
extern "C"
JNIEXPORT void JNICALL
Java_com_example_wyopencv_WonderfulOpenCv_initNative(JNIEnv *env, jobject thiz,
                                                     jstring face_model_path) {
    // TODO: implement initNative()
    const char *path = env->GetStringUTFChars(face_model_path,nullptr);
    if(!wonderfulOpenCv){
        wonderfulOpenCv = new WonderfulOpenCv(path);
        wonderfulOpenCv->initDetectorTracker();
    }
    env->ReleaseStringUTFChars(face_model_path,path);
}

//设置surface
extern "C"
JNIEXPORT void JNICALL
Java_com_example_wyopencv_WonderfulOpenCv_setSurfaceNative(JNIEnv *env, jobject thiz,
                                                           jobject surface) {
    // TODO: implement setSurfaceNative()
    pthread_mutex_lock(&mutex);
    //先移除之前的window
    if (window) {
        ANativeWindow_release(window);
        window = nullptr;
    }
    //创建新的window
    window = ANativeWindow_fromSurface(env, surface);
    pthread_mutex_unlock(&mutex);
}

/**将java层的图像数据进程处理并渲染*/
extern "C"
JNIEXPORT void JNICALL
Java_com_example_wyopencv_WonderfulOpenCv_renderFrameNative(JNIEnv *env, jobject thiz,
                                                            jbyteArray bytes, jint width,
                                                            jint height) {
//    LOGE("receive a frame from java!");
    jbyte *yuv = env->GetByteArrayElements(bytes, 0);

    /**人脸追踪*/
    //NV21转rgb（网上找的一个算法）
    uint8_t *rgb = wonderfulOpenCv->nv21ToRgb(reinterpret_cast<uint8_t *>(yuv), width, height);
    //rgb转mat
    Mat frame = wonderfulOpenCv->rgb2Mat(rgb,width,height);
    //TODO:人脸追踪（调用这个函数使tracker run起来之后，所有的代码都被阻塞住了，甚至连java层的camera回调都被阻塞了！！！）
    wonderfulOpenCv->dynamicFaceCheck(frame);
    //mat转rgb
    rgb = wonderfulOpenCv->mat2rgb(frame,0);
    //rgb转rgba
    uint8_t *rgba = wonderfulOpenCv->rgb2rgba(rgb, width, height, 50);
    //渲染
    renderFrame(rgba, width, height);

    /**rgb转灰度再显示*/
//    //这样做其实是相当复杂的，但是对学习Mat矩阵和指针的操作将非常有用
//    //实际上除了上层传过来的指针数据，其他地方都可以直接使用Mat操作
//    //NV21转rgb
//    uint8_t *rgb = wonderfulOpenCv->nv21ToRgb(reinterpret_cast<uint8_t *>(yuv), width, height);
//    //rgb转灰度
//    uint8_t *rgbGray = wonderfulOpenCv->transformGray(rgb, width, height);
//    //rgb转rgba
//    uint8_t *rgba = wonderfulOpenCv->rgb2rgba(rgbGray, width, height, 50);
//    //渲染
//    renderFrame(rgba, width, height);

    /**直接显示*/
//    //这里必须转成uint8_t，之前就是因为没转导致颜色不对
//    uint8_t *rgb = wonderfulOpenCv->nv21ToRgb(reinterpret_cast<uint8_t *>(yuv), width, height);
//    uint8_t *rgba = wonderfulOpenCv->rgb2rgba(rgb, width, height, 50);
//    renderFrame(rgba, width, height);

    env->ReleaseByteArrayElements(bytes, yuv, 0);
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_wyopencv_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

//jint JNI_OnLoad(JavaVM *vm,void *argc){
//    wonderfulOpenCv = new WonderfulOpenCv;
//}
//
////在so卸载时调用
//void JNI_OnUnload(JavaVM *jvm, void *reserved){
//    if(!wonderfulOpenCv){
//        delete (wonderfulOpenCv);
//        wonderfulOpenCv = nullptr;
//    }
//}

