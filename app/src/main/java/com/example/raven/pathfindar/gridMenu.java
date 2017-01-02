package com.example.raven.pathfindar;

import android.content.Intent;
import android.os.Bundle;
import android.view.SurfaceView;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import org.opencv.android.CameraBridgeViewBase;

/**
 * Created by Raven on 31-Dec-16.
 */

public class gridMenu extends MainActivity {

    private int x = 10, y = 10;
    private boolean saved = false;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.grid_edit_menu);

        Intent callingIntent = getIntent();

        x = callingIntent.getIntExtra("x", 10);
        y = callingIntent.getIntExtra("y", 10);

        TextView yLabel = (TextView) findViewById(R.id.tv_numV);
        yLabel.setText(Integer.toString(y));

        TextView xLabel = (TextView) findViewById(R.id.tv_numH);
        xLabel.setText(Integer.toString(x));

    }

    public void incrementVert(View view) {
        y++;
        TextView yLabel = (TextView) findViewById(R.id.tv_numV);
        yLabel.setText(Integer.toString(y));
    }

    public void decrementVert(View view) {
        y--;
        TextView yLabel = (TextView) findViewById(R.id.tv_numV);
        yLabel.setText(Integer.toString(y));
    }

    public void incrementHoriz(View view) {
        x++;
        TextView xLabel = (TextView) findViewById(R.id.tv_numH);
        xLabel.setText(Integer.toString(x));
    }

    public void decrementHoriz(View view) {
        x--;
        TextView xLabel = (TextView) findViewById(R.id.tv_numH);
        xLabel.setText(Integer.toString(x));
    }

    public void saveGrid(View view){
        saved = true;

        Toast toast = Toast.makeText(this, "Grid Dimensions Updated", Toast.LENGTH_SHORT);
        toast.show();

    }

    public void closeGridMenu(View view) {

        Intent resultIntent = new Intent();

        resultIntent.putExtra("saved", saved);
        resultIntent.putExtra("x", x);
        resultIntent.putExtra("y", y);
        setResult(this.RESULT_OK, resultIntent);

        saved = false;

        finish();
    }

}
