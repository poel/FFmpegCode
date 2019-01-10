package cc.gl.com.ffmpegcode

import android.content.Context
import android.graphics.PixelFormat
import android.util.AttributeSet
import android.view.Surface
import android.view.SurfaceView

class VideoView: SurfaceView {
    constructor(context: Context) : super(context) {}

    constructor(context: Context, attrs: AttributeSet) : super(context, attrs) {
        init()

    }

    private fun init() {
        val holder = holder
        holder.setFormat(PixelFormat.RGBA_8888)
    }

    constructor(context: Context, attrs: AttributeSet, defStyleAttr: Int) : super(context, attrs, defStyleAttr) {}

    fun player(input: String) {
        Thread(Runnable {
            //绘制功能 不需要交给SurfaveView        VideoView.this.getHolder().getSurface()
            render(input, this@VideoView.holder.surface)
        }).start()
    }

    external fun render(input: String, surface: Surface)

    companion object {
        init {
            System.loadLibrary("native-lib")
            System.loadLibrary("avcodec-57")
            System.loadLibrary("avdevice-57")
            System.loadLibrary("avfilter-6")
            System.loadLibrary("avformat-57")
            System.loadLibrary("avutil-55")
            System.loadLibrary("swresample-2")
            System.loadLibrary("swscale-4")
        }
    }
}