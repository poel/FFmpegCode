package cc.gl.com.ffmpegcode

import android.content.Intent
import android.os.Bundle
import android.support.v7.app.AppCompatActivity
import android.widget.Button
import com.blankj.utilcode.constant.PermissionConstants
import com.blankj.utilcode.util.PermissionUtils
import android.app.Activity
import android.net.Uri


class ChooseFileActivity : AppCompatActivity() {

    companion object {
        const val READ_REQUEST_CODE = 42
    }
    private lateinit var chooseFileBtn: Button

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_choose_file)

        chooseFileBtn = findViewById(R.id.choose_file_btn)

        chooseFileBtn.setOnClickListener{
            performFileSearch()
        }
    }


    /**
     * Fires an intent to spin up the "file chooser" UI and select an image.
     */
    fun performFileSearch() {

        // ACTION_OPEN_DOCUMENT is the intent to choose a file via the system's file
        // browser.
        val intent = Intent(Intent.ACTION_OPEN_DOCUMENT)

        // Filter to only show results that can be "opened", such as a
        // file (as opposed to a list of contacts or timezones)
        intent.addCategory(Intent.CATEGORY_OPENABLE)

        // Filter to show only images, using the image MIME data type.
        // If one wanted to search for ogg vorbis files, the type would be "audio/ogg".
        // To search for all documents available via installed storage providers,
        // it would be "*/*".
        intent.type = "video/mp4"

        startActivityForResult(intent, READ_REQUEST_CODE)
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

    public override fun onActivityResult(requestCode: Int, resultCode: Int, resultData: Intent?) {

        // The ACTION_OPEN_DOCUMENT intent was sent with the request code
        // READ_REQUEST_CODE. If the request code seen here doesn't match, it's the
        // response to some other intent, and the code below shouldn't run at all.

        if (requestCode == READ_REQUEST_CODE && resultCode == Activity.RESULT_OK) {
            // The document selected by the user won't be returned in the intent.
            // Instead, a URI to that document will be contained in the return intent
            // provided to this method as a parameter.
            // Pull that URI using resultData.getData().
            var uri: Uri? = null
            if (resultData != null) {
                uri = resultData.data
                MainActivity.start(this@ChooseFileActivity, uri.path)
            }
        }
    }
}
