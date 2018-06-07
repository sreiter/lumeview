/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package eu.mihosoft.vrl.v3d.nativeogl;

import com.jogamp.opengl.GLCapabilities;
import com.jogamp.opengl.GLProfile;
import com.jogamp.opengl.awt.GLJPanel;
import com.jogamp.opengl.util.FPSAnimator;
import java.awt.Dimension;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;

import javax.swing.JFrame;
import javax.swing.SwingUtilities;

/**
 * JOGL NativeOpenGL OpenGL Interface
 */
@SuppressWarnings("serial")
public class Main {

    // Define constants for the top-level container
    private static String TITLE = "Native OpenGL Viewer";  // window's title
    private static final int CANVAS_WIDTH = 400;  // width of the drawable
    private static final int CANVAS_HEIGHT = 400; // height of the drawable
    private static final int FPS = 60; // animator's target frames per second

    /**
     * The entry main() method to setup the top-level container and animator
     *
     * @param args
     */
    public static void main(String[] args) {
        // Run the GUI codes in the event-dispatching thread for thread safety
        SwingUtilities.invokeLater( () -> {
            // Create the OpenGL rendering canvas
            GLProfile glp = GLProfile.get(GLProfile.GL3);
            GLCapabilities capabilities = new GLCapabilities(glp);
            capabilities.setPBuffer(true);
            capabilities.setFBO(false);
//        capabilities.setAlphaBits(8);
//        capabilities.setRedBits(8);
//        capabilities.setGreenBits(8);
//        capabilities.setBlueBits(8);
            capabilities.setDepthBits(24);
            capabilities.setDoubleBuffered(true);

            // create jogl panel
            GLJPanel panel = new GLJPanel(capabilities);
            GLJPanel canvas = new NativeOpenGLPanel(capabilities);
            canvas.setPreferredSize(new Dimension(CANVAS_WIDTH, CANVAS_HEIGHT));

            // Create a animator that drives canvas' display() at the specified FPS.
            final FPSAnimator animator = new FPSAnimator(canvas, FPS, true);

            // Create the top-level container
            final JFrame frame = new JFrame();
            frame.getContentPane().add(canvas);
            frame.addWindowListener(new WindowAdapter() {
                @Override
                public void windowClosing(WindowEvent e) {
                    // We use a thread to run the stop() to ensure that the
                    // animator stops before program exits.
                    new Thread(() -> {
                        if (animator.isStarted()) {
                            animator.stop();
                        }
                        System.exit(0);
                    }).start();
                }
            });

            frame.setTitle(TITLE);
            frame.pack();
            frame.setVisible(true);

            animator.start(); // start the animation loop
        });
    }
}
