package com.me.app

import android.graphics.Bitmap
import android.os.Bundle
import android.view.WindowManager
import androidx.appcompat.app.AppCompatActivity
import androidx.core.graphics.createBitmap
import androidx.core.splashscreen.SplashScreen.Companion.installSplashScreen
import androidx.core.view.WindowInsetsCompat
import androidx.core.view.WindowInsetsControllerCompat
import androidx.lifecycle.lifecycleScope
import com.me.app.databinding.ActivityMainBinding
import kotlinx.coroutines.delay
import kotlinx.coroutines.launch
import java.lang.Thread.sleep


class  MainActivity : AppCompatActivity() {
    private lateinit var binding: ActivityMainBinding

    // == TAKEN FROM C SIDE == //
    private val dimen = 1100

    init {
        System.loadLibrary("olivest")
    }


    external fun drawLines(bmp: Bitmap)
    external fun drawTriangle(bmp: Bitmap)


    override fun onCreate(savedInstanceState: Bundle?) {
        installSplashScreen().setKeepOnScreenCondition {
            sleep(800)
            false
        }
        super.onCreate(savedInstanceState)
        WindowInsetsControllerCompat(window, window.decorView).let { controller ->
            controller.hide(WindowInsetsCompat.Type.systemBars())
            controller.systemBarsBehavior =
                WindowInsetsControllerCompat.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE
        }
        window.addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)


        val bitmap = createBitmap(dimen, dimen)

        // == CALL OLIVE.c AND FILL IN THE BUFFER == //
        drawTriangle(bitmap)
        binding.surface.setImageBitmap(bitmap)

        lifecycleScope.launch {
            repeat(Int.MAX_VALUE) {
                delay(10)
                drawTriangle(bitmap)
                binding.surface.setImageBitmap(bitmap)

            }
        }
    }
}
