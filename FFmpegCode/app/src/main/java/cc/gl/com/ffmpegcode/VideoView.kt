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
            System.loadLibrary("avcodec")
            System.loadLibrary("avdevice")
            System.loadLibrary("avfilter")
            System.loadLibrary("avformat")
            System.loadLibrary("avutil")
            System.loadLibrary("swresample")
            System.loadLibrary("swscale")
            System.loadLibrary("native-lib")
        }
    }
}