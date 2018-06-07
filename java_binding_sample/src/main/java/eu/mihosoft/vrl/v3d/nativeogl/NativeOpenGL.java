/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package eu.mihosoft.vrl.v3d.nativeogl;

import eu.mihosoft.nativeogl.vrl.system.IOUtil;
import eu.mihosoft.nativeogl.vrl.system.VSysUtil;
import java.io.File;
import java.net.URL;
import java.util.logging.Level;
import java.util.logging.Logger;
import org.apache.commons.io.FileUtils;
import org.apache.commons.io.filefilter.DirectoryFileFilter;

/**
 *
 * @author Michael Hoffer &lt;info@michaelhoffer.de&gt;
 */
public class NativeOpenGL {
    
    private static boolean initialized = false;
    
    private static void initNativeLib() {
//        String libName = "vrl_native_opengl_binding."
//                + VSysUtil.getPlatformSpecificLibraryEnding();
//
//        if(!VSysUtil.isWindows()) {
//            libName="lib"+libName;
//        }

        try {
//            String urlPath = "/nativeogl/natives/"
//                    + VSysUtil.getPlatformSpecificPath()
//                    + libName;
//
//            System.out.println(urlPath);
//
//            URL inputUrl = NativeOpenGL.class.
//                    getResource(urlPath);
//
//            File tmp = new File(System.getProperty("java.io.tmpdir"));
//            File destParent = new File(tmp, "vrl-native-ogl");
//            File dest = new File(destParent, libName);
//            IOUtil.deleteContainedFilesAndDirs(destParent);
//            System.out.println("dest: " + dest);
//
//            FileUtils.copyURLToFile(inputUrl, dest);
//
//            for(File f : destParent.listFiles()) {
//                System.out.println(" -> f: " + f.getAbsolutePath());
//            }
//
//            VSysUtil.loadNativeLibrariesInFolder(
//                    destParent);

            VSysUtil.loadNativeLibrariesInFolder(
                    new File("../build/src"));
            
        } catch (Exception ex) {
            Logger.getLogger(NativeOpenGL.class.getName()).
                    log(Level.SEVERE, null, ex);

            System.exit(1);
        }
    }
    
    public static void glInit() {
        prepareNativeCall();
        native_gl_init();
    }
    
    public static void glDisplay() {
        prepareNativeCall();
        native_gl_display();
    }
    
    static void glDispose() {
        prepareNativeCall();
        native_gl_dispose();
    }
    
    public static void glReshape(int x, int y, int w, int h) {
        prepareNativeCall();
        native_gl_reshape(x, y, w, h);
    }
    
    public static void mouseClickEvent(int x, int y, int btn, int click_count) {
        prepareNativeCall();
        native_mouse_click_event(x, y, btn, click_count);
    }
    
    public static void mouseMoveEvent(int x, int y) {
        prepareNativeCall();
        native_mouse_move_event(x, y);
    }
    
    public static void mouseDragEvent(int x, int y, int btn) {
        prepareNativeCall();
        native_mouse_drag_event(x, y, btn);
    }
    
    public static void mousePressEvent(int x, int y, int btn) {
        prepareNativeCall();
        native_mouse_press_event(x, y, btn);
    }
    
    public static void mouseReleaseEvent(int x, int y, int btn) {
        prepareNativeCall();
        native_mouse_release_event(x, y, btn);
    }
    
    public static void mouseEnterEvent(int x, int y) {
        prepareNativeCall();
        native_mouse_enter_event(x, y);
    }
    
    public static void mouseExitEvent(int x, int y) {
        prepareNativeCall();
        native_mouse_exit_event(x, y);
    }
    
    public static void mouseWheelEvent(double movement) {
        prepareNativeCall();
        native_mouse_wheel_event(movement);
    }
    
    private static void prepareNativeCall() {
        if (!initialized) {
            initNativeLib();
            initialized = true;
        }
    }
    
    private static native void native_gl_init();
    
    private static native void native_gl_display();
    
    private static native void native_gl_reshape(int x, int y, int w, int h);
    
    private static native void native_gl_dispose();
    
    private static native void native_mouse_click_event(int x, int y, int btn, int click_count);
    
    private static native void native_mouse_move_event(int x, int y);
    
    private static native void native_mouse_drag_event(int x, int y, int btn);
    
    private static native void native_mouse_press_event(int x, int y, int btn);
    
    private static native void native_mouse_release_event(int x, int y, int btn);
    
    private static native void native_mouse_enter_event(int x, int y);
    
    private static native void native_mouse_exit_event(int x, int y);
    
    private static native void native_mouse_wheel_event(double movement);
    
}
