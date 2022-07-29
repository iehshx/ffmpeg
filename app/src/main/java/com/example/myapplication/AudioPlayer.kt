package com.example.myapplication

import android.util.Log


class AudioPlayer {
    interface AudioPlayerCallBack {
        fun onError(code: Int);
        fun onPrepareSuccess();
        fun onPlayProcess(current: Float)
    }

    var callback: AudioPlayerCallBack? = null
    var url: String = "";
    private var ptr: Long = 0;


    fun setDataSource(url: String) {
        this.url = url
    }

    fun prepare() {
        if (!url.isEmpty()) {
            ptr = nativeInit(url)
            nPrepare(ptr, url)
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
    private external fun nativeInit(url: String): Long
    private external fun nStop(ptr: Long);
    private external fun nSeek(ptr: Long);
    private external fun nPause(ptr: Long);
    private external fun nPlay(url: Long);
    private external fun nPrepare(ptr: Long, url: String)

}