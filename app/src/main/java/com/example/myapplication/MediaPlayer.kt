package com.example.myapplication

import android.util.Log
import android.view.Surface

class MediaPlayer {

    interface MediaPlayerCallBack {
        fun onPlayProcess(current: Float)
        fun onRequestRender()
    }

    var url: String = "";
    private var ptr: Long = 0;


    var callBack: MediaPlayerCallBack? = null

    fun setDataSource(url: String) {
        this.url = url
    }


    fun prepare(surface: Surface?, renderType: Int) {
        if (!url.isEmpty()) {
            ptr = nativeInit()
            nPrepare(ptr, url, renderType, surface)
        } else {
            Log.e("tag", "please set url first!");
        }
    }


    fun play() {
        if (ptr != 0L) {
            nPlay(ptr)
        } else {
            Log.e("tag", "please prepare first!");
        }
    }


    fun stop() {
        nStop(ptr);
    }


    fun pause() {
        nPause(ptr);
    }

    fun getDuration(): Long {
        if (ptr != 0L) {
            return nGetDuration(ptr);
        } else {
            return 0L;
        }
    }


    private external fun nGetDuration(ptr: Long): Long
    private external fun nativeInit(): Long
    private external fun nStop(ptr: Long);
    private external fun nSeek(ptr: Long);
    private external fun nPause(ptr: Long);
    private external fun nPlay(url: Long);
    private external fun nUnInit(url: Long);
    private external fun nPrepare(ptr: Long, url: String, renderType: Int, surface: Surface?)

    companion object {

        var VIDEO_RENDER_ANWINDOW = 1
        var VIDEO_RENDER_OPENGL = 0
        external fun onSurfaceCreated(renderType: Int, renderStyle: Int)
        external fun onDrawFrame(renderType: Int)
    }

    fun unInit() {
        nUnInit(ptr)
    }


}