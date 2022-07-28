package com.example.myapplication

import android.util.Log

interface AudioPlayerCallBack {
    fun onStateChange(code: Int);
    fun onPrepareSuccess();
    fun onPlay(current: Int, total: Int)
}

class AudioPlayer {
    var callback: AudioPlayerCallBack? = null
    var url: String = "";
    private var ptr: Long = 0;


    fun setDataSource(url: String) {
        this.url = url
    }

    fun prepare() {
        if (!url.isEmpty()) {
            ptr = nativeInit(url)
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

    external fun nativeInit(url: String): Long
    external fun nStop(ptr: Long);
    external fun nSeek(ptr: Long);
    external fun nPause(ptr: Long);
    external fun nPlay(url: Long);

}