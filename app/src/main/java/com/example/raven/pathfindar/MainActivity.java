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
import android.content.Context;
import android.content.Intent;
import android.graphics.drawable.Drawable;
import android.hardware.Camera;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.text.Layout;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.SurfaceView;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.RelativeLayout;
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
    public static final int PLOTTING = 2;

    private int gridToggle = 0;
    private int selWeapon = -1;
    private int selToken = -1;

    private String plotPath = "";
    private int mRemaining = 30;

    private boolean tracking = false;

    public static final int LINE = 0;
    public static final int CONE = 1;
    public static final int SPHERE = 2;


    private boolean paused = false;
    public static final int PAUSE_TIME = 100;
    private int pauseTimer = PAUSE_TIME;

    private int x = 10, y = 10;
    private int blastRad = 0;
    private int blastType = -1;

    int screenWidth;
    int screenHeight;

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

        ImageButton w1 = (ImageButton)findViewById(R.id.bt_selWeapon1);
        ImageButton w2 = (ImageButton)findViewById(R.id.bt_selWeapon2);
        ImageButton w3 = (ImageButton)findViewById(R.id.bt_selWeapon3);
        ImageButton w4 = (ImageButton)findViewById(R.id.bt_selWeapon4);
        w1.setVisibility(View.INVISIBLE);
        w2.setVisibility(View.INVISIBLE);
        w3.setVisibility(View.INVISIBLE);
        w4.setVisibility(View.INVISIBLE);

        RelativeLayout dpad = (RelativeLayout)findViewById(R.id.layt_dPad);
        dpad.setVisibility(View.INVISIBLE);

        setSelectedWeapon(-1);

        DisplayMetrics displayMetrics = new DisplayMetrics();
        WindowManager wm = (WindowManager) getApplicationContext().getSystemService(Context.WINDOW_SERVICE); // the results will be higher than using the activity context object or the getWindowManager() shortcut
        wm.getDefaultDisplay().getMetrics(displayMetrics);
        screenWidth = displayMetrics.widthPixels;
        screenHeight = displayMetrics.heightPixels;


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

        if (tracking){

            if (paused){

                pauseTimer--;

                final TextView pauseLabel = (TextView)findViewById(R.id.tv_countTime);
                final String outString = "Tracking will resume in: " + Integer.toString(pauseTimer) + " Frames\nPlease Center Camera Above Grid";

                if (pauseTimer < 0){
                    pauseTimer = PAUSE_TIME;
                    paused = false;

                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            pauseLabel.setVisibility(View.INVISIBLE);
                        }
                    });

                } else {
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            pauseLabel.setVisibility(View.VISIBLE);
                            pauseLabel.setText(outString);
                        }
                    });

                    return inputFrame.rgba();
                }

            }

            Mat col = inputFrame.rgba();
            Mat secondary = new Mat();

            col.copyTo(secondary);

            long addr1 = col.getNativeObjAddr(), addr2 = secondary.getNativeObjAddr();

            detectMarkers(addr1, addr2);

            return col;

        } else {
            return inputFrame.rgba();
        }

    }

    public void toggleTracking(View view) {

        ImageButton trackingButton = (ImageButton) findViewById(R.id.btn_tracking);

        if (paused){
            paused = false;
            pauseTimer = PAUSE_TIME;
            TextView pauseLabel = (TextView)findViewById(R.id.tv_countTime);
            pauseLabel.setVisibility(View.INVISIBLE);
        }

        if (tracking){
            tracking = false;
            trackingButton.setImageDrawable(getDrawable(R.drawable.play));
        } else {
            tracking = true;
            trackingButton.setImageDrawable(getDrawable(R.drawable.pause));
            init();
        }
    }

    public void toggleGridType(View view) {

        if (gridToggle == MOVEMENT) {
            gridToggle = PLOTTING;
            ImageButton toggleButton = (ImageButton)findViewById(R.id.bt_swapGridType);
            toggleButton.setImageDrawable(getDrawable(R.drawable.compass));

            RelativeLayout dpad = (RelativeLayout)findViewById(R.id.layt_dPad);
            dpad.setVisibility(View.VISIBLE);

            mRemaining = getMovementRemain();
            updateMRemainLabel();

        } else if (gridToggle == PLOTTING){
            gridToggle = ATTACK;

            ImageButton toggleButton = (ImageButton) findViewById(R.id.bt_swapGridType);
            toggleButton.setImageDrawable(getDrawable(R.drawable.crossedswords));

            RelativeLayout dpad = (RelativeLayout)findViewById(R.id.layt_dPad);
            dpad.setVisibility(View.INVISIBLE);

            if (selToken >= 0) {
                ImageButton w1 = (ImageButton) findViewById(R.id.bt_selWeapon1);
                ImageButton w2 = (ImageButton) findViewById(R.id.bt_selWeapon2);
                ImageButton w3 = (ImageButton) findViewById(R.id.bt_selWeapon3);
                ImageButton w4 = (ImageButton) findViewById(R.id.bt_selWeapon4);

                Token selectedToken = tokenList.get(selToken);

                if (!selectedToken.w1.name.equals("")) {
                    w1.setVisibility(View.VISIBLE);
                } else {
                    w1.setVisibility(View.INVISIBLE);
                }

                if (!selectedToken.w2.name.equals("")) {
                    w2.setVisibility(View.VISIBLE);
                } else {
                    w2.setVisibility(View.INVISIBLE);
                }

                if (!selectedToken.w3.name.equals("")) {
                    w3.setVisibility(View.VISIBLE);
                } else {
                    w3.setVisibility(View.INVISIBLE);
                }

                if (!selectedToken.w4.name.equals("")) {
                    w4.setVisibility(View.VISIBLE);
                } else {
                    w4.setVisibility(View.INVISIBLE);
                }
            }

        } else {
            gridToggle = MOVEMENT;
            ImageButton toggleButton = (ImageButton)findViewById(R.id.bt_swapGridType);
            toggleButton.setImageDrawable(getDrawable(R.drawable.fwayarrow));

            RelativeLayout dpad = (RelativeLayout)findViewById(R.id.layt_dPad);
            dpad.setVisibility(View.INVISIBLE);

            ImageButton w1 = (ImageButton)findViewById(R.id.bt_selWeapon1);
            ImageButton w2 = (ImageButton)findViewById(R.id.bt_selWeapon2);
            ImageButton w3 = (ImageButton)findViewById(R.id.bt_selWeapon3);
            ImageButton w4 = (ImageButton)findViewById(R.id.bt_selWeapon4);
            w1.setVisibility(View.INVISIBLE);
            w2.setVisibility(View.INVISIBLE);
            w3.setVisibility(View.INVISIBLE);
            w4.setVisibility(View.INVISIBLE);

            setSelectedWeapon(-1);



        }

        setDisplayType(gridToggle);

    }

    public void toggleBlastTemplate(View view) {

        ImageButton blastButton = (ImageButton)findViewById(R.id.bt_blastTemplate);

        if (blastType == -1){
            TextView blastRadL = (TextView)findViewById(R.id.tv_blastRad);
            Button plus = (Button)findViewById(R.id.bt_plusBlast);
            Button minus = (Button)findViewById(R.id.bt_minusBlast);

            blastRadL.setVisibility(View.VISIBLE);
            plus.setVisibility(View.VISIBLE);
            minus.setVisibility(View.VISIBLE);

            blastType = LINE;
            blastButton.setImageDrawable(getDrawable(R.drawable.line));
        } else if (blastType == LINE){
            TextView blastRadL = (TextView)findViewById(R.id.tv_blastRad);
            Button plus = (Button)findViewById(R.id.bt_plusBlast);
            Button minus = (Button)findViewById(R.id.bt_minusBlast);

            blastRadL.setVisibility(View.VISIBLE);
            plus.setVisibility(View.VISIBLE);
            minus.setVisibility(View.VISIBLE);

            blastType = CONE;
            blastButton.setImageDrawable(getDrawable(R.drawable.cone));
        } else if (blastType == CONE){
            TextView blastRadL = (TextView)findViewById(R.id.tv_blastRad);
            Button plus = (Button)findViewById(R.id.bt_plusBlast);
            Button minus = (Button)findViewById(R.id.bt_minusBlast);

            blastRadL.setVisibility(View.VISIBLE);
            plus.setVisibility(View.VISIBLE);
            minus.setVisibility(View.VISIBLE);

            blastType = SPHERE;
            blastButton.setImageDrawable(getDrawable(R.drawable.sphere));
        } else if (blastType == SPHERE){
            blastType = -1;
            blastButton.setImageDrawable(getDrawable(R.drawable.fireball));

            TextView blastRadL = (TextView)findViewById(R.id.tv_blastRad);
            Button plus = (Button)findViewById(R.id.bt_plusBlast);
            Button minus = (Button)findViewById(R.id.bt_minusBlast);

            blastRadL.setVisibility(View.INVISIBLE);
            plus.setVisibility(View.INVISIBLE);
            minus.setVisibility(View.INVISIBLE);
        }

        TextView blastRadL = (TextView)findViewById(R.id.tv_blastRad);

        String blastText = blastRadL.getText().toString();

        blastRad = Integer.parseInt(blastText);

        setBlastTemplate(blastType, blastRad);

    }

    public void incrementBlast(View view) {
        TextView blastRadL = (TextView)findViewById(R.id.tv_blastRad);

        blastRad += 5;

        blastRadL.setText(Integer.toString(blastRad));
        setBlastTemplate(blastType, blastRad);
    }

    public void decrementBlast(View view) {
        TextView blastRadL = (TextView)findViewById(R.id.tv_blastRad);

        if (blastRad > 5){
            blastRad -= 5;
        }

        blastRadL.setText(Integer.toString(blastRad));
        setBlastTemplate(blastType, blastRad);
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

    public void selWeapon1(View view) {
        setSelectedWeapon(1);
    }

    public void selWeapon2(View view) {
        setSelectedWeapon(2);
    }

    public void selWeapon3(View view) {
        setSelectedWeapon(3);
    }

    public void selWeapon4(View view) {
        setSelectedWeapon(4);
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
            setSelectedWeapon(-1);

            String out = setTokenString(tokenList);
            setTokenList(out);
            setSelectedToken(selectedID);
            selToken = selectedID;
        }

        mOpenCvCameraView = (CameraBridgeViewBase) findViewById(R.id.tutorial1_activity_java_surface_view);
        mOpenCvCameraView.setVisibility(SurfaceView.VISIBLE);
        mOpenCvCameraView.setCvCameraViewListener(this);
        //mOpenCvCameraView.setMaxFrameSize(848, 480);
        mOpenCvCameraView.enableView();

        /*
        if (gridToggle == ATTACK) {
            ImageButton toggleButton = (ImageButton)findViewById(R.id.bt_swapGridType);
            toggleButton.setImageDrawable(getDrawable(R.drawable.crossedswords));
            View dpad = (View) findViewById(R.id.layt_dPad);
            dpad.setVisibility(View.INVISIBLE);

            if (selToken >= 0) {
                ImageButton w1 = (ImageButton) findViewById(R.id.bt_selWeapon1);
                ImageButton w2 = (ImageButton) findViewById(R.id.bt_selWeapon2);
                ImageButton w3 = (ImageButton) findViewById(R.id.bt_selWeapon3);
                ImageButton w4 = (ImageButton) findViewById(R.id.bt_selWeapon4);

                Token selectedToken = tokenList.get(selToken);

                if (!selectedToken.w1.name.equals("")) {
                    w1.setVisibility(View.VISIBLE);
                } else {
                    w1.setVisibility(View.INVISIBLE);
                }

                if (!selectedToken.w2.name.equals("")) {
                    w2.setVisibility(View.VISIBLE);
                } else {
                    w2.setVisibility(View.INVISIBLE);
                }

                if (!selectedToken.w3.name.equals("")) {
                    w3.setVisibility(View.VISIBLE);
                } else {
                    w3.setVisibility(View.INVISIBLE);
                }

                if (!selectedToken.w4.name.equals("")) {
                    w4.setVisibility(View.VISIBLE);
                } else {
                    w4.setVisibility(View.INVISIBLE);
                }
            }

        } else {
            ImageButton toggleButton = (ImageButton)findViewById(R.id.bt_swapGridType);
            toggleButton.setImageDrawable(getDrawable(R.drawable.fwayarrow));

            ImageButton w1 = (ImageButton)findViewById(R.id.bt_selWeapon1);
            ImageButton w2 = (ImageButton)findViewById(R.id.bt_selWeapon2);
            ImageButton w3 = (ImageButton)findViewById(R.id.bt_selWeapon3);
            ImageButton w4 = (ImageButton)findViewById(R.id.bt_selWeapon4);
            w1.setVisibility(View.INVISIBLE);
            w2.setVisibility(View.INVISIBLE);
            w3.setVisibility(View.INVISIBLE);
            w4.setVisibility(View.INVISIBLE);

            setSelectedWeapon(-1);
        }
*/

        if (gridToggle == MOVEMENT) {
            ImageButton toggleButton = (ImageButton)findViewById(R.id.bt_swapGridType);
            toggleButton.setImageDrawable(getDrawable(R.drawable.fwayarrow));

            View dpad = (View) findViewById(R.id.layt_dPad);
            dpad.setVisibility(View.INVISIBLE);

            ImageButton w1 = (ImageButton)findViewById(R.id.bt_selWeapon1);
            ImageButton w2 = (ImageButton)findViewById(R.id.bt_selWeapon2);
            ImageButton w3 = (ImageButton)findViewById(R.id.bt_selWeapon3);
            ImageButton w4 = (ImageButton)findViewById(R.id.bt_selWeapon4);
            w1.setVisibility(View.INVISIBLE);
            w2.setVisibility(View.INVISIBLE);
            w3.setVisibility(View.INVISIBLE);
            w4.setVisibility(View.INVISIBLE);

            setSelectedWeapon(-1);
        } else if (gridToggle == PLOTTING){
            ImageButton toggleButton = (ImageButton)findViewById(R.id.bt_swapGridType);
            toggleButton.setImageDrawable(getDrawable(R.drawable.compass));

            View dpad = (View) findViewById(R.id.layt_dPad);
            dpad.setVisibility(View.VISIBLE);

            plotPath = "";
            setPlottingPath(plotPath);

            mRemaining = getMovementRemain();
            updateMRemainLabel();
        } else {
            View dpad = (View) findViewById(R.id.layt_dPad);
            dpad.setVisibility(View.INVISIBLE);
            ImageButton toggleButton = (ImageButton) findViewById(R.id.bt_swapGridType);
            toggleButton.setImageDrawable(getDrawable(R.drawable.crossedswords));

            if (selToken >= 0) {
                ImageButton w1 = (ImageButton) findViewById(R.id.bt_selWeapon1);
                ImageButton w2 = (ImageButton) findViewById(R.id.bt_selWeapon2);
                ImageButton w3 = (ImageButton) findViewById(R.id.bt_selWeapon3);
                ImageButton w4 = (ImageButton) findViewById(R.id.bt_selWeapon4);

                Token selectedToken = tokenList.get(selToken);

                if (!selectedToken.w1.name.equals("")) {
                    w1.setVisibility(View.VISIBLE);
                } else {
                    w1.setVisibility(View.INVISIBLE);
                }

                if (!selectedToken.w2.name.equals("")) {
                    w2.setVisibility(View.VISIBLE);
                } else {
                    w2.setVisibility(View.INVISIBLE);
                }

                if (!selectedToken.w3.name.equals("")) {
                    w3.setVisibility(View.VISIBLE);
                } else {
                    w3.setVisibility(View.INVISIBLE);
                }

                if (!selectedToken.w4.name.equals("")) {
                    w4.setVisibility(View.VISIBLE);
                } else {
                    w4.setVisibility(View.INVISIBLE);
                }
            }
        }

        ImageButton trackingButton = (ImageButton) findViewById(R.id.btn_tracking);
        if (tracking){
            trackingButton.setImageDrawable(getDrawable(R.drawable.pause));
        } else {
            trackingButton.setImageDrawable(getDrawable(R.drawable.play));
        }

        paused = true;

    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {

        if (blastType == -1){
            return false;
        }

        int x = (int)event.getX();
        int y = (int)event.getY();

        int realX = 0;
        int realY = 0;

        switch (event.getAction()) {
            case MotionEvent.ACTION_DOWN:
                realX = (x * 720) / screenWidth;
                realY = (y * 480) / screenHeight;
                setTouchPos(realX, realY);
        }
        return false;
    }

    public void updateMRemainLabel(){
        TextView mRLabel = (TextView)findViewById(R.id.tv_mRemain);
        mRLabel.setText(Integer.toString(mRemaining));
    }

    public boolean overextended(){

        for (int i = 0; i < tokenList.size(); i++){
            if (tokenList.get(i).id == selToken){
                mRemaining = tokenList.get(i).mRange;
            }
        }

        String[] parts = plotPath.split("-");

        int diagonal = 0;

        for (int i = 0; i < parts.length; i++){
            if (parts[i].equals("UL") || parts[i].equals("UR") || parts[i].equals("DL") || parts[i].equals("DR")){
                diagonal++;
                if (diagonal % 2 == 1){
                    mRemaining -= 5;
                } else {
                    mRemaining -= 10;
                }
            } else {
                mRemaining -= 5;
            }
        }

        if (mRemaining < 0){
            mRemaining = getMovementRemain();
            return true;
        } else {
            return false;
        }

    }

    public void adjustPathUpLeft(View view) {
        plotPath = plotPath.concat("UL-");
        if (!overextended()) {
            setPlottingPath(plotPath);
            mRemaining = getMovementRemain();
            updateMRemainLabel();
        } else {
            plotPath = plotPath.substring(0, plotPath.length() - 3);
        }
    }

    public void adjustPathLeft(View view) {
        plotPath = plotPath.concat("L-");
        if (!overextended()) {
            setPlottingPath(plotPath);
            mRemaining = getMovementRemain();
            updateMRemainLabel();
        } else {
            plotPath = plotPath.substring(0, plotPath.length() - 2);
        }
    }

    public void adjustPathDownLeft(View view) {
        plotPath = plotPath.concat("DL-");
        if (!overextended()) {
            setPlottingPath(plotPath);
            mRemaining = getMovementRemain();
            updateMRemainLabel();
        } else {
            plotPath = plotPath.substring(0, plotPath.length() - 3);
        }
    }

    public void adjustPathDown(View view) {
        plotPath = plotPath.concat("D-");
        if (!overextended()) {
            setPlottingPath(plotPath);
            mRemaining = getMovementRemain();
            updateMRemainLabel();
        } else {
            plotPath = plotPath.substring(0, plotPath.length() - 2);
        }
    }

    public void adjustPathDownRight(View view) {
        plotPath = plotPath.concat("DR-");
        if (!overextended()) {
            setPlottingPath(plotPath);
            mRemaining = getMovementRemain();
            updateMRemainLabel();
        } else {
            plotPath = plotPath.substring(0, plotPath.length() - 3);
        }
    }

    public void adjustPathRight(View view) {
        plotPath = plotPath.concat("R-");
        if (!overextended()) {
            setPlottingPath(plotPath);
            mRemaining = getMovementRemain();
            updateMRemainLabel();
        } else {
            plotPath = plotPath.substring(0, plotPath.length() - 2);
        }
    }

    public void adjustPathUpRight(View view) {
        plotPath = plotPath.concat("UR-");
        if (!overextended()) {
            setPlottingPath(plotPath);
            mRemaining = getMovementRemain();
            updateMRemainLabel();
        } else {
            plotPath = plotPath.substring(0, plotPath.length() - 3);
        }
    }

    public void adjustPathUp(View view) {
        plotPath = plotPath.concat("U-");
        if (!overextended()) {
            setPlottingPath(plotPath);
            mRemaining = getMovementRemain();
            updateMRemainLabel();
        } else {
            plotPath = plotPath.substring(0, plotPath.length() - 2);
        }
    }

    public void resetPath(View view) {
        plotPath = "";
        setPlottingPath(plotPath);
        mRemaining = getMovementRemain();
        updateMRemainLabel();
    }

    public native void detectMarkers(long image_final,long image_editable);
    public native void adjustGridDimensions(int x,int y);
    public native String getTokenList();
    public native void setTokenList(String tokenListString);
    public native void setDisplayType(int type);
    public native void setSelectedToken(int selectedTokenID);
    public native void setSelectedWeapon(int selectedWeapon);
    public native void setBlastTemplate(int blastID, int radius);
    public native int getMovementRemain();
    public native void setPlottingPath(String path);
    public native void setTouchPos(int x, int y);
    public native void init();
}
