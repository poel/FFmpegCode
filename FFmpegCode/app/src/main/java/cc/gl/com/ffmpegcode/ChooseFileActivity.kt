package cc.gl.com.ffmpegcode

import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import android.os.Environment
import android.util.Log
import android.widget.Button
import cc.gl.com.ffmpegcode.view.FileDialog
import com.blankj.utilcode.constant.PermissionConstants
import com.blankj.utilcode.util.PermissionUtils
import java.io.File

class ChooseFileActivity : AppCompatActivity() {

    private lateinit var chooseFileBtn: Button

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_choose_file)

        chooseFileBtn = findViewById(R.id.choose_file_btn)

        chooseFileBtn.setOnClickListener{
            openDir()
        }
    }

    fun openDir() {
        val mPath = File(Environment.getExternalStorageDirectory().toString() + "//DIR//")
        val fileDialog = FileDialog(this, mPath, ".mp4")
        fileDialog.addFileListener(object : FileDialog.FileSelectedListener {
            override fun fileSelected(file: File) {
                Log.d(javaClass.name, "selected file " + file.toString())

                MainActivity.start(this@ChooseFileActivity, file.absolutePath)
            }
        })
        //fileDialog.addDirectoryListener(new FileDialog.DirectorySelectedListener() {
        //  public void directorySelected(File directory) {
        //      Log.d(getClass().getName(), "selected dir " + directory.toString());
        //  }
        //});
        //fileDialog.setSelectDirectoryOption(false);
        fileDialog.showDialog()
    }

    override fun onResume() {
        super.onResume()

        PermissionUtils.permission(
            PermissionConstants.CAMERA,
            PermissionConstants.MICROPHONE,
            PermissionConstants.STORAGE,
            PermissionConstants.SENSORS
        )
            .rationale { shouldRequest ->

            }
            .callback(object : PermissionUtils.FullCallback {
                override fun onGranted(permissionsGranted: List<String>) {

                }

                override fun onDenied(permissionsDeniedForever: List<String>, permissionsDenied: List<String>) {

                }
            }).request()
    }
}
