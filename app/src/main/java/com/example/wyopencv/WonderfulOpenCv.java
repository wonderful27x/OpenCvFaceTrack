package com.example.wyopencv;

import android.app.Activity;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class WonderfulOpenCv implements CameraHelper.PreviewCallback,CameraHelper.SizeChangedListener, SurfaceHolder.Callback {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    private static final String TAG = "WonderfulOpenCv";

    private SurfaceHolder surfaceHolder;                   //surfaceHolder
    private CameraHelper cameraHelper;                     //摄像头帮助类
    private Activity activity;
    private int width;
    private int height;
    private ExecutorService executorService;       //单线程池

    public WonderfulOpenCv(Activity activity){
        this.activity = activity;
        executorService = Executors.newSingleThreadExecutor();
    }

    //创建摄像头
    public void createCamera(SurfaceView surfaceView){
        cameraHelper = new CameraHelper(activity,surfaceView);
        cameraHelper.setPreviewCallback(this);
        cameraHelper.setSizeChangedListener(this);
    }

    //初始化surface
    public void setSurfaceView(SurfaceView surfaceView){
        if (surfaceHolder != null){
            surfaceHolder.removeCallback(this);
        }
        surfaceHolder = surfaceView.getHolder();
        surfaceHolder.addCallback(this);
    }

    //CameraHelper摄像头预览获取到数据回调
    @Override
    public void onPreviewFrame(final byte[] bytes) {
        renderFrameNative(bytes,width,height);
//        Log.d(TAG, "width: " + width + " height: " + height);
    }

    //CameraHelper方向大小发生改变时回调，一般是横竖屏切换
    @Override
    public void onChanged(int width, int height) {
        this.width = width;
        this.height = height;
    }


    //surface创建回调
    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        setSurfaceNative(holder.getSurface());
    }

    //surface发送改变回调，一般是横竖屏切换
    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

    }

    //surface销毁回调
    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {

    }

    /**
     * native 方法
     */
    //初始化：人脸模型路径，追踪器
    public native void initNative(String faceModelPath);
    //设置native surface
    private native void setSurfaceNative(Surface surface);
    //渲染
    private native void renderFrameNative(byte[] bytes,int width,int height);
}
