package com.example.myapplication

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.Environment
import android.view.View
import android.widget.TextView
import com.example.myapplication.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)
    }


    external fun getInfo(url: String)

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

    fun getInfo(view: View) {
        var path = Environment.getExternalStorageDirectory().absolutePath + "/y958.wav";
        getInfo(path);
    }
}