package com.example.raven.pathfindar;

import org.opencv.core.*;

import java.io.Serializable;

/**
 * Created by Raven on 30-Dec-16.
 */

public class Token implements Serializable{
    int id = 0;
    String name = "";
    Point location = new Point(0.0,0.0);
    int mRange = 30;
    int mRemain = 0;
    int aRange = 0;
    int lifespan = 0;
    boolean found = false;
    Scalar colour = new Scalar(0,0,0);

    public String toString() {
        return Integer.toString(id) + " " + name + " " + location.toString() + " " + Integer.toString(mRange) + " " + Integer.toString(mRemain) + " " + Integer.toString(lifespan) + " " + Boolean.toString(found) + " " + colour.toString();
    }

}
