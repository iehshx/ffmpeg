package com.example.myapplication

import android.opengl.GLSurfaceView
import android.os.Bundle
import android.os.Environment
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import com.example.myapplication.MediaPlayer.Companion.VIDEO_RENDER_OPENGL
import kotlinx.android.synthetic.main.activity_video_gl.*
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

class GLVideoActivity : AppCompatActivity() {

    private var mStyle: Int = 0
    private lateinit var mMediaPlayer: MediaPlayer
    var mRender: GLSurfaceView.Renderer = object : GLSurfaceView.Renderer {
        override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {
            MediaPlayer.onSurfaceCreated(VIDEO_RENDER_OPENGL, mStyle);
        }

        override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {
        }

        override fun onDrawFrame(gl: GL10?) {
            MediaPlayer.onDrawFrame(VIDEO_RENDER_OPENGL)
        }

    }

    override fun onPause() {
        super.onPause()
        mMediaPlayer.pause()
    }

    override fun onDestroy() {
        super.onDestroy()
        mMediaPlayer?.stop()
        mMediaPlayer?.unInit();
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_video_gl)
        glSurfaceView.setEGLContextClientVersion(3)
        glSurfaceView.setRenderer(mRender)
        glSurfaceView.renderMode = GLSurfaceView.RENDERMODE_WHEN_DIRTY
        mMediaPlayer = MediaPlayer()
        mStyle = intent.getIntExtra("videoRenderStyle", 0)
        mMediaPlayer?.setDataSource(
            Environment.getExternalStorageDirectory().absolutePath + "/iehshx/zhuxianHD1080p.mp4"
        );
        mMediaPlayer?.callBack = object : MediaPlayer.MediaPlayerCallBack {
            override fun onPlayProcess(current: Float) {
                process?.post {
                    process.text = current.toInt().toString()
                }
            }

            override fun onRequestRender() {
                Log.e("iehshx-jni", "onRequestRender")
                glSurfaceView.requestRender()
            }

        }
        mMediaPlayer.prepare(null, VIDEO_RENDER_OPENGL);
        mMediaPlayer?.play()
    }
}