package com.example.myapplication

import android.content.Intent
import android.graphics.Color
import android.graphics.drawable.Drawable
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.Environment
import android.util.Log
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.AdapterView
import android.widget.ArrayAdapter
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

    var mCurrent = -1;
    var mAudioPlayer: AudioPlayer? = null

    var mUrl: String = ""
    var mPlayDrawable: Drawable? = null
    var mPauseDrawable: Drawable? = null
    var mPath = Environment.getExternalStorageDirectory().absolutePath + "/iehshx/";

    var mStyle = 0

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityMainBinding.inflate(layoutInflater)
        mPlayDrawable = resources.getDrawable(android.R.drawable.ic_media_play)
        mPauseDrawable = resources.getDrawable(android.R.drawable.ic_media_pause)
        setContentView(binding.root)
        binding.spVideoRenderType.adapter = ArrayAdapter.createFromResource(
            this,
            R.array.video_types,
            android.R.layout.simple_spinner_dropdown_item
        )
        binding.spVideoRenderType.onItemSelectedListener = object :
            AdapterView.OnItemSelectedListener {
            override fun onItemSelected(
                parent: AdapterView<*>?,
                view: View?,
                position: Int,
                id: Long
            ) {
                mStyle = position
            }

            override fun onNothingSelected(parent: AdapterView<*>?) {
                mStyle = 0
            }
        }
        mAudioPlayer = AudioPlayer()
        mAudioPlayer?.callback = object : AudioPlayer.AudioPlayerCallBack {

            override fun onError(code: Int) {
                Log.e("tag", "code = $code")
            }

            override fun onPrepareSuccess() {
                mAudioPlayer?.play()
                started = true;
                binding.pbProcess?.post {
                    binding.pbProcess.max = mAudioPlayer!!.getDuration()!!.toInt()
                    Log.e("tag", "准备完毕,max = ${binding.pbProcess.max}")
                    binding.play.setCompoundDrawablesWithIntrinsicBounds(
                        mPauseDrawable,
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
                        binding.play?.setCompoundDrawables(mPlayDrawable, null, null, null);
                        started = false;
                        binding.next.performClick()
                    }

                    binding.tvTime?.text = "${binding.pbProcess.progress}/ ${binding.pbProcess.max}"
                }
            }
        }
        mAudioPlayer?.setDataSource(mPath)

        binding.pre.setOnClickListener {
            mCurrent -= 1;
            if (mCurrent < 0) mCurrent = (songList?.size ?: 1) - 1;
            binding.play.setCompoundDrawablesWithIntrinsicBounds(
                mPlayDrawable, null, null, null
            )
            for (item in songList!!) {
                item.selected = false;
            }
            songList?.get(mCurrent)?.selected = true
            binding.rvSongList.adapter?.notifyDataSetChanged()
            mAudioPlayer?.stop();
            mAudioPlayer?.setDataSource(songList?.get(mCurrent)?.path ?: "");
            mAudioPlayer?.prepare();
        }
        binding.next.setOnClickListener {
            mCurrent += 1;
            if (mCurrent > (songList?.size ?: 1) - 1) mCurrent = 0;
            binding.play.setCompoundDrawablesWithIntrinsicBounds(
                mPlayDrawable, null, null, null
            )
            for (item in songList!!) {
                item.selected = false;
            }
            songList?.get(mCurrent)?.selected = true
            binding.rvSongList.adapter?.notifyDataSetChanged()
            mAudioPlayer?.stop();
            mAudioPlayer?.setDataSource(songList?.get(mCurrent)?.path ?: "");
            mAudioPlayer?.prepare();
        }
        binding.play.setOnClickListener {
            if (!started || mAudioPlayer?.url != mUrl) {//没有播放 或者修改了歌曲
                mAudioPlayer?.stop()
                binding.play.setCompoundDrawablesWithIntrinsicBounds(
                    mPlayDrawable, null, null, null
                )
                mAudioPlayer?.setDataSource(mUrl)
                mAudioPlayer?.prepare()
            } else {
                if (pause) {
                    binding.play.setCompoundDrawablesWithIntrinsicBounds(
                        mPauseDrawable, null, null, null
                    );
                    mAudioPlayer?.play()
                    pause = false
                } else {
                    binding.play.setCompoundDrawablesWithIntrinsicBounds(
                        mPlayDrawable, null, null, null
                    )
                    mAudioPlayer?.pause()
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
                    mCurrent = position;
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
        var songDir = File(mPath)
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
        mAudioPlayer?.prepare();
    }

    fun video(view: View) {
//        startActivity(Intent(this, GLVideoActivity::class.java).apply {
//            putExtra(
//                "videoRenderStyle",
//                mStyle
//            )
//        })
        startActivity(Intent(this,VideoActivity::class.java))
    }

}