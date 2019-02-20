package cc.gl.com.ffmpegcode

import android.content.Context
import android.content.Intent
import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import android.os.Environment
import android.view.WindowManager
import cc.gl.com.ffmpegcode.constant.ExtraKey
import kotlinx.android.synthetic.main.activity_main.*
import java.io.File

class MainActivity : AppCompatActivity() {

    val input = File(Environment.getExternalStorageDirectory(), "a.mp4").absolutePath!!

    companion object {
        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("native-lib")
        }

        const val TAG = "CallActivity"
        fun start(context: Context, filePath: String?) {
            val intent = Intent(context, MainActivity::class.java)
            intent.putExtra(ExtraKey.EXTRA_FILE_PATH, filePath)
            context.startActivity(intent)
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        window.setFlags(
            WindowManager.LayoutParams.FLAG_HARDWARE_ACCELERATED,
            WindowManager.LayoutParams.FLAG_HARDWARE_ACCELERATED)

        // Example of a call to a native method
        play.player(input)
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(): String

    external fun helloNDK(): String

    external fun getInfo(): String
}
