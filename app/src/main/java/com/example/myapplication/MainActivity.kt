package com.example.myapplication

import android.graphics.Color
import android.graphics.drawable.Drawable
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.Environment
import android.util.Log
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import com.example.myapplication.databinding.ActivityMainBinding
import java.io.File

class MainActivity : AppCompatActivity() {

    private var pause: Boolean = false
    private var songList: List<Node>? = null
    private var started: Boolean = false
    private lateinit var binding: ActivityMainBinding

    var current = -1;
    var audioPlayer: AudioPlayer? = null

    var mUrl: String = ""
    var playDrawable: Drawable? = null
    var pauseDrawable: Drawable? = null
    var path = Environment.getExternalStorageDirectory().absolutePath + "/iehshx/";

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityMainBinding.inflate(layoutInflater)
        playDrawable = resources.getDrawable(android.R.drawable.ic_media_play)
        pauseDrawable = resources.getDrawable(android.R.drawable.ic_media_pause)
        setContentView(binding.root)
        audioPlayer = AudioPlayer()
        audioPlayer?.callback = object : AudioPlayer.AudioPlayerCallBack {

            override fun onError(code: Int) {
                Log.e("tag", "code = $code")
            }

            override fun onPrepareSuccess() {
                audioPlayer?.play()
                started = true;
                binding.pbProcess?.post {
                    binding.pbProcess.max = audioPlayer!!.getDuration()!!.toInt()
                    Log.e("tag", "准备完毕,max = ${binding.pbProcess.max}")
                    binding.play.setCompoundDrawablesWithIntrinsicBounds(
                        pauseDrawable,
                        null,
                        null,
                        null
                    );
                }


            }

            override fun onPlayProcess(current: Float) {
                Log.e("tag", "current = ${current}")
                binding.pbProcess?.post {
                    var process = current.toInt()
                    binding.pbProcess!!.setProgress(process, true);
                    if (binding.pbProcess.progress + 1 >= binding.pbProcess.max) {
                        binding.play?.setCompoundDrawables(playDrawable, null, null, null);
                        started = false;
                        binding.next.performClick()
                    }

                    binding.tvTime?.text = "${binding.pbProcess.progress}/ ${binding.pbProcess.max}"
                }
            }
        }
        audioPlayer?.setDataSource(path)

        binding.pre.setOnClickListener {
            current -= 1;
            if (current < 0) current = (songList?.size ?: 1) - 1;
            binding.play.setCompoundDrawablesWithIntrinsicBounds(
                playDrawable, null, null, null
            )
            for (item in songList!!) {
                item.selected = false;
            }
            songList?.get(current)?.selected = true
            binding.rvSongList.adapter?.notifyDataSetChanged()
            audioPlayer?.stop();
            audioPlayer?.setDataSource(songList?.get(current)?.path ?: "");
            audioPlayer?.prepare();
        }
        binding.next.setOnClickListener {
            current += 1;
            if (current > (songList?.size ?: 1) - 1) current = 0;
            binding.play.setCompoundDrawablesWithIntrinsicBounds(
                playDrawable, null, null, null
            )
            for (item in songList!!) {
                item.selected = false;
            }
            songList?.get(current)?.selected = true
            binding.rvSongList.adapter?.notifyDataSetChanged()
            audioPlayer?.stop();
            audioPlayer?.setDataSource(songList?.get(current)?.path ?: "");
            audioPlayer?.prepare();
        }
        binding.play.setOnClickListener {
            if (!started || audioPlayer?.url != mUrl) {//没有播放 或者修改了歌曲
                audioPlayer?.stop()
                binding.play.setCompoundDrawablesWithIntrinsicBounds(
                    playDrawable, null, null, null
                )
                audioPlayer?.setDataSource(mUrl)
                audioPlayer?.prepare()
            } else {
                if (pause) {
                    binding.play.setCompoundDrawablesWithIntrinsicBounds(
                        pauseDrawable, null, null, null
                    );
                    audioPlayer?.play()
                    pause = false
                } else {
                    binding.play.setCompoundDrawablesWithIntrinsicBounds(
                        playDrawable, null, null, null
                    )
                    audioPlayer?.pause()
                    pause = true
                }
            }
        }

        songList = findSongList()

        if (songList != null) {
            binding.rvSongList.layoutManager =
                LinearLayoutManager(this, RecyclerView.VERTICAL, false)
            binding.rvSongList.adapter = Adapter(songList!!);
        }
    }

    inner class MyViewHolder(itemView: View) : RecyclerView.ViewHolder(itemView) {

    }

    inner class Node(val path: String, var selected: Boolean)

    inner class Adapter(var datas: List<Node>) : RecyclerView.Adapter<MyViewHolder>() {
        override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): MyViewHolder {
            return MyViewHolder(
                LayoutInflater.from(parent.context).inflate(R.layout.item_song, null, false)
            )
        }

        override fun onBindViewHolder(holder: MyViewHolder, position: Int) {
            holder.itemView.findViewById<TextView>(R.id.tvSongName)?.apply {
                if (datas[position].selected) {
                    setTextColor(Color.parseColor("#ff4338"))
                } else {
                    setTextColor(Color.parseColor("#000000"))
                }
                text = datas[position].path
                setOnClickListener {
                    current = position;
                    mUrl = datas[position].path
                    datas.forEach { it.selected = false }
                    datas[position].selected = true
                    notifyDataSetChanged()
                }
            }
        }

        override fun getItemCount(): Int {
            return datas.size
        }

    }

    private fun findSongList(): List<Node> {
        var songList = ArrayList<Node>()
        var songDir = File(path)
        if (songDir.exists()) {
            var files = songDir.listFiles()
            for (file in files) {
                if (file.name.endsWith(".mp3") || file.name.endsWith(".acc")) {
                    songList.add(Node(file.absolutePath, false))
                }
            }
        }
        return songList
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

}