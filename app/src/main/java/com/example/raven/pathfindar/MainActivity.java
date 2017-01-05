package com.example.raven.pathfindar;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewFrame;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.aruco.Aruco;
import org.opencv.aruco.DetectorParameters;
import org.opencv.aruco.Dictionary;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.*;

import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewListener2;
import org.opencv.core.Point;
import org.opencv.imgproc.Imgproc;

import android.app.Activity;
import android.content.Intent;
import android.hardware.Camera;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.MenuItem;
import android.view.SurfaceView;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.hardware.camera2.params.*;

import java.util.ArrayList;
import java.util.List;

public class MainActivity extends AppCompatActivity implements CvCameraViewListener2 {
    private static final String TAG = "OCVSample::Activity";

    private CameraBridgeViewBase mOpenCvCameraView;
    private boolean mIsJavaCamera = true;
    private MenuItem mItemSwitchCamera = null;

    public static final int REQUEST_GRID_MENU = 1;
    public static final int REQUEST_TOKEN_MENU = 2;

    public static final int MOVEMENT = 0;
    public static final int ATTACK = 1;

    private int gridToggle = 0;
    private int selWeapon = 0;

    private boolean circleFlag = false;
    private int x = 10, y = 10;

    private static ArrayList<Token> tokenList = new ArrayList();

    private BaseLoaderCallback mLoaderCallback = new BaseLoaderCallback(this) {
        @Override
        public void onManagerConnected(int status) {
            switch (status) {
                case LoaderCallbackInterface.SUCCESS:
                {
                    Log.i(TAG, "OpenCV loaded successfully");
                    mOpenCvCameraView.enableView();
                } break;
                default:
                {
                    super.onManagerConnected(status);
                } break;
            }
        }
    };

    public MainActivity() {
        Log.i(TAG, "Instantiated new " + this.getClass());
    }

    static {
        System.loadLibrary("native-lib");
    }

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        Log.i(TAG, "called onCreate");
        super.onCreate(savedInstanceState);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        setContentView(R.layout.activity_main);

        mOpenCvCameraView = (CameraBridgeViewBase) findViewById(R.id.tutorial1_activity_java_surface_view);

        mOpenCvCameraView.setVisibility(SurfaceView.VISIBLE);
        mOpenCvCameraView.setCvCameraViewListener(this);
        mOpenCvCameraView.enableView();

    }

    @Override
    public void onPause()
    {
        super.onPause();
        if (mOpenCvCameraView != null)
            mOpenCvCameraView.disableView();
    }

    @Override
    public void onResume()
    {
        super.onResume();
        if (!OpenCVLoader.initDebug()) {
            Log.d(TAG, "Internal OpenCV library not found. Using OpenCV Manager for initialization");
            OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_3_1_0, this, mLoaderCallback);
        } else {
            Log.d(TAG, "OpenCV library found inside package. Using it!");
            mLoaderCallback.onManagerConnected(LoaderCallbackInterface.SUCCESS);
        }
    }

    public void onDestroy() {
        super.onDestroy();
        if (mOpenCvCameraView != null)
            mOpenCvCameraView.disableView();
    }

    public void onCameraViewStarted(int width, int height) {
    }

    public void onCameraViewStopped() {
    }

    public Mat onCameraFrame(CameraBridgeViewBase.CvCameraViewFrame inputFrame) {

        if (circleFlag){

            Mat col = inputFrame.rgba();
            Mat secondary = new Mat();

            col.copyTo(secondary);

            List<Mat> corners = new ArrayList<>();

            // Crashes here
            //Dictionary dict = Aruco.getPredefinedDictionary(Aruco.DICT_6X6_250);

            //Size szSource = new Size(640,480);
            //Size szFin = new Size(720,1280);

            long addr1 = col.getNativeObjAddr(), addr2 = secondary.getNativeObjAddr();

            detectMarkers(addr1, addr2);

            return col;

        } else {
            return inputFrame.rgba();
        }

    }

    public void toggleTracking(View view) {

        if (circleFlag){
            circleFlag = false;
        } else {
            circleFlag = true;
            init();
        }
    }


    public void toggleGridType(View view) {

        if (gridToggle == MOVEMENT) {
            gridToggle = ATTACK;
            Button toggleButton = (Button)findViewById(R.id.bt_swapGridType);
            toggleButton.setText("Attack");
        } else {
            gridToggle = MOVEMENT;
            Button toggleButton = (Button)findViewById(R.id.bt_swapGridType);
            toggleButton.setText("Move");
        }

        setDisplayType(gridToggle);

    }

    public void openGridMenu(View view) {
        mOpenCvCameraView.disableView();

        Intent intent = new Intent(this, gridMenu.class);

        intent.putExtra("x", x);
        intent.putExtra("y", y);

        startActivityForResult(intent, REQUEST_GRID_MENU);



        //setContentView(R.layout.grid_edit_menu);

        //TextView yLabel = (TextView) findViewById(R.id.tv_numV);
        //yLabel.setText(Long.toString(y));

        //TextView xLabel = (TextView) findViewById(R.id.tv_numH);
        //xLabel.setText(Long.toString(x));
    }

    public void openTokenMenu(View view) {
        mOpenCvCameraView.disableView();
        tokenList = getFormattedTokenList(tokenList);

        tokenMenu.tokenList = tokenList;
        tokenEditMenu.tokenList = tokenList;
        Intent intent = new Intent(this, tokenMenu.class);

        startActivityForResult(intent, REQUEST_TOKEN_MENU);
    }

    public ArrayList<Token> getFormattedTokenList(ArrayList<Token> tl) {
        tl.clear();

        String tokListString = getTokenList();

        if (!tokListString.equals("")){
            for (String token:  tokListString.split("│")) {

                Token nTok = new Token();
                String[] tokPieces = token.split("¦");
                String[] colours = tokPieces[23].split(",");

                nTok.id = Integer.parseInt(tokPieces[0]);
                nTok.name = tokPieces[1];
                nTok.location.x = Double.parseDouble(tokPieces[2]);
                nTok.location.y = Double.parseDouble(tokPieces[3]);
                nTok.mRange = Integer.parseInt(tokPieces[4]);
                nTok.lifespan = Integer.parseInt(tokPieces[5]);

                if (tokPieces[6].equals("0")){
                    nTok.found = false;
                } else {
                    nTok.found = true;
                }

                nTok.w1 = new Weapon (tokPieces[7], Integer.parseInt(tokPieces[8]));

                if (tokPieces[9].equals("0")){
                    nTok.w1.reach = false;
                } else {
                    nTok.w1.reach = true;
                }

                if (tokPieces[10].equals("0")){
                    nTok.w1.ranged = false;
                } else {
                    nTok.w1.ranged = true;
                }

                nTok.w2 = new Weapon (tokPieces[11], Integer.parseInt(tokPieces[12]));

                if (tokPieces[13].equals("0")){
                    nTok.w2.reach = false;
                } else {
                    nTok.w2.reach = true;
                }

                if (tokPieces[14].equals("0")){
                    nTok.w2.ranged = false;
                } else {
                    nTok.w2.ranged = true;
                }

                nTok.w3 = new Weapon (tokPieces[15], Integer.parseInt(tokPieces[16]));

                if (tokPieces[17].equals("0")){
                    nTok.w3.reach = false;
                } else {
                    nTok.w3.reach = true;
                }

                if (tokPieces[18].equals("0")){
                    nTok.w3.ranged = false;
                } else {
                    nTok.w3.ranged = true;
                }

                nTok.w4 = new Weapon (tokPieces[19], Integer.parseInt(tokPieces[20]));

                if (tokPieces[21].equals("0")){
                    nTok.w4.reach = false;
                } else {
                    nTok.w4.reach = true;
                }

                if (tokPieces[22].equals("0")){
                    nTok.w4.ranged = false;
                } else {
                    nTok.w4.ranged = true;
                }

                //nTok.found = Boolean.parseBoolean(tokPieces[6]);
                nTok.colour = new Scalar(Double.parseDouble(colours[0]),Double.parseDouble(colours[1]),Double.parseDouble(colours[2]),Double.parseDouble(colours[3]));

                tl.add(nTok);
            }
        }

        return tl;

    }

    public String setTokenString(ArrayList<Token> tl){

        String out = "";
        for (int i = 0; i < tl.size(); i++){
            out = out.concat(tl.get(i).id + "@");
            out = out.concat(tl.get(i).name + "@");
            out = out.concat(tl.get(i).location.x + "@");
            out = out.concat(tl.get(i).location.y + "@");
            out = out.concat(tl.get(i).mRange + "@");
            out = out.concat(tl.get(i).lifespan + "@");
            out = out.concat(tl.get(i).found + "@");
            out = out.concat(tl.get(i).colour.val[0] + "," + tl.get(i).colour.val[1] + "," + tl.get(i).colour.val[2] + "," + tl.get(i).colour.val[3] + "@");
            out = out.concat(tl.get(i).w1.name + "@");
            out = out.concat(tl.get(i).w1.range + "@");
            out = out.concat(tl.get(i).w1.reach + "@");
            out = out.concat(tl.get(i).w1.ranged + "@");
            out = out.concat(tl.get(i).w2.name + "@");
            out = out.concat(tl.get(i).w2.range + "@");
            out = out.concat(tl.get(i).w2.reach + "@");
            out = out.concat(tl.get(i).w2.ranged + "@");
            out = out.concat(tl.get(i).w3.name + "@");
            out = out.concat(tl.get(i).w3.range + "@");
            out = out.concat(tl.get(i).w3.reach + "@");
            out = out.concat(tl.get(i).w3.ranged + "@");
            out = out.concat(tl.get(i).w4.name + "@");
            out = out.concat(tl.get(i).w4.range + "@");
            out = out.concat(tl.get(i).w4.reach + "@");
            out = out.concat(tl.get(i).w4.ranged + "|");
        }

        return out;
    }

    public void onActivityResult(int req, int res, Intent in){
        setContentView(R.layout.activity_main);

        if (req == REQUEST_GRID_MENU){

            if (in.getBooleanExtra("saved", false)){
                x = in.getIntExtra("x",10);
                y = in.getIntExtra("y",10);

                adjustGridDimensions(x,y);
            }

        }

        if (req == REQUEST_TOKEN_MENU){

            int selectedID = in.getIntExtra("selectedToken", -1);


            String out = setTokenString(tokenList);
            setTokenList(out);
            setSelectedToken(selectedID);
        }


        mOpenCvCameraView = (CameraBridgeViewBase) findViewById(R.id.tutorial1_activity_java_surface_view);
        mOpenCvCameraView.setVisibility(SurfaceView.VISIBLE);
        mOpenCvCameraView.setCvCameraViewListener(this);
        //mOpenCvCameraView.setMaxFrameSize(848, 480);
        mOpenCvCameraView.enableView();

    }

    public native void detectMarkers(long image_final,long image_editable);
    public native void adjustGridDimensions(int x,int y);
    public native String getTokenList();
    public native void setTokenList(String tokenListString);
    public native void setDisplayType(int type);
    public native void setSelectedToken(int selectedTokenID);
    public native void init();

}
