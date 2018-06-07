/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package eu.mihosoft.vrl.v3d.nativeogl;



import com.jogamp.opengl.*;
import com.jogamp.opengl.awt.GLJPanel;
import com.jogamp.opengl.glu.GLU;
import com.jogamp.opengl.util.FPSAnimator;
import java.awt.Dimension;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.awt.event.MouseWheelEvent;
import java.awt.event.MouseWheelListener;


import javax.swing.JFrame;
import javax.swing.SwingUtilities;
import javax.vecmath.Point3f;
import org.apache.commons.io.monitor.FileAlterationListenerAdaptor;
import org.apache.commons.io.monitor.FileAlterationMonitor;
import org.apache.commons.io.monitor.FileAlterationObserver;

import static com.jogamp.opengl.GL.GL_DEPTH_TEST;
import static com.jogamp.opengl.GL.GL_LEQUAL;
import static com.jogamp.opengl.GL.GL_NICEST;
import static com.jogamp.opengl.GL2ES1.GL_PERSPECTIVE_CORRECTION_HINT;
import static com.jogamp.opengl.fixedfunc.GLLightingFunc.GL_SMOOTH;
import static com.jogamp.opengl.fixedfunc.GLMatrixFunc.GL_MODELVIEW;
import static com.jogamp.opengl.fixedfunc.GLMatrixFunc.GL_PROJECTION;

/**
 * JOGL NativeOpenGL OpenGL Interface
 */
@SuppressWarnings("serial")
public class NativeOpenGLPanel extends GLJPanel {

    
    private GLU glu;  // the GL Utility

    /**
     * Constructor to setup the GUI for this Component
     */
    public NativeOpenGLPanel(GLCapabilities capabilities) {

        super(capabilities);
        
        this.addGLEventListener(new GLEventListener() {

            @Override
            public void init(GLAutoDrawable drawable) {
//                GL2 gl = drawable.getGL().getGL2();      // get the OpenGL graphics context
//                glu = new GLU();                         // get GL Utilities
//                gl.glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // set background (clear) color
//                gl.glClearDepth(1.0f);      // set clear depth value to farthest
//                gl.glEnable(GL_DEPTH_TEST); // enables depth testing
//                gl.glDepthFunc(GL_LEQUAL);  // the type of depth test to do
//                gl.glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // best perspective correction
//                gl.glShadeModel(GL_SMOOTH); // blends colors nicely, and smoothes out lighting

                NativeOpenGL.glInit();
            }

            @Override
            public void dispose(GLAutoDrawable glad) {
                NativeOpenGL.glDispose();
            }

            @Override
            public void display(GLAutoDrawable glad) {
                NativeOpenGL.glDisplay();
            }

            @Override
            public void reshape(GLAutoDrawable drawable, int x, int y, int w, int h) {

//                // get the OpenGL 2 graphics context
//                GL2 gl = drawable.getGL().getGL2();
//
//                if (w == 0) {
//                    w = 1;   // prevent divide by zero
//                }
//
//                float aspect = (float) w / h;
//
//                // Set the view port (display area) to cover the entire window
//                gl.glViewport(0, 0, w, h);
//
//                // Setup perspective projection, with aspect ratio matches viewport
//                gl.glMatrixMode(GL_PROJECTION);  // choose projection matrix
//                gl.glLoadIdentity();             // reset projection matrix
//                glu.gluPerspective(45.0, aspect, 0.1, 100.0); // fovy, aspect, zNear, zFar
//
//                // Enable the model-view transform
//                gl.glMatrixMode(GL_MODELVIEW);
//                gl.glLoadIdentity(); // reset

                NativeOpenGL.glReshape(x, y, w, h);
            }
        });

        this.addMouseListener(new MouseListener() {

            @Override
            public void mouseClicked(MouseEvent e) {
                NativeOpenGL.mouseClickEvent(e.getX(), e.getY(), e.getButton(), e.getClickCount());
            }

            @Override
            public void mousePressed(MouseEvent e) {
                NativeOpenGL.mousePressEvent(e.getX(), e.getY(), e.getButton());
            }

            @Override
            public void mouseReleased(MouseEvent e) {
                NativeOpenGL.mouseReleaseEvent(e.getX(), e.getY(), e.getButton());
            }

            @Override
            public void mouseEntered(MouseEvent e) {
                NativeOpenGL.mouseEnterEvent(e.getX(), e.getY());
            }

            @Override
            public void mouseExited(MouseEvent e) {
                NativeOpenGL.mouseExitEvent(e.getX(), e.getY());
            }
        });

        addMouseMotionListener(new MouseMotionListener() {

            @Override
            public void mouseDragged(MouseEvent e) {
                NativeOpenGL.mouseDragEvent(e.getX(), e.getY(), e.getButton());
            }

            @Override
            public void mouseMoved(MouseEvent e) {
                NativeOpenGL.mouseMoveEvent(e.getX(), e.getY());
            }
        });
        
        addMouseWheelListener(new MouseWheelListener() {

            @Override
            public void mouseWheelMoved(MouseWheelEvent e) {
                NativeOpenGL.mouseWheelEvent(e.getPreciseWheelRotation());
            }
        });
    }

}
