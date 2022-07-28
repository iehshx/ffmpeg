package com.example.myapplication

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.Environment
import android.util.Log
import android.view.View
import android.widget.TextView
import com.example.myapplication.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    var audioPlayer: AudioPlayer? = null
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)
        audioPlayer = AudioPlayer()
        var path = Environment.getExternalStorageDirectory().absolutePath + "/iehshx/zhaji.mp3"
        audioPlayer?.callback = object : AudioPlayerCallBack {
            override fun onStateChange(code: Int) {
                Log.e("tag", "code = $code")
            }

            override fun onPrepareSuccess() {
                Log.e("tag", "准备完毕")
            }

            override fun onPlay(current: Int, total: Int) {
                Log.e("tag", "current = $current,total = $total")
                binding.pbProcess.max = total;
                binding.pbProcess.progress = current;
            }

        }
        audioPlayer?.setDataSource(path)

    }

    /**
     * A native method that is implemented by the 'myapplication' native library,
     * which is packaged with this application.
     */

    companion object {
        // Used to load the 'myapplication' library on application startup.
        init {
            System.loadLibrary("myapplication")
        }
    }

    fun prepare(view: View) {
        audioPlayer?.prepare();
    }

    fun play(view: View) {
        audioPlayer?.play()
    }

}