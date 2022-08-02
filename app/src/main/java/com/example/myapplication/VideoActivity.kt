package com.example.myapplication

import android.os.Bundle
import android.os.Environment
import android.view.SurfaceHolder
import androidx.appcompat.app.AppCompatActivity
import kotlinx.android.synthetic.main.activity_video.*

class VideoActivity : AppCompatActivity() {

    var mMediaPlayer: MediaPlayer? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_video)
        sv_video.holder.addCallback(SurfaceCallBack)

    }

    var SurfaceCallBack = object : SurfaceHolder.Callback {
        override fun surfaceCreated(p0: SurfaceHolder) {
            mMediaPlayer = MediaPlayer()
            mMediaPlayer?.setDataSource(
                Environment.getExternalStorageDirectory().absolutePath + "/iehshx/zhuxianHD1080p.mp4"
            );
            mMediaPlayer?.callBack = object : MediaPlayer.MediaPlayerCallBack {
                override fun onPlayProcess(current: Float) {
                    process?.post {
                        process.text = current.toInt().toString()
                    }
                }

            }
            mMediaPlayer?.prepare(p0.surface)
            mMediaPlayer?.play()

        }

        override fun surfaceChanged(p0: SurfaceHolder, p1: Int, p2: Int, p3: Int) {
        }

        override fun surfaceDestroyed(p0: SurfaceHolder) {
            mMediaPlayer?.stop();
            mMediaPlayer?.unInit();
        }

    }
}