package com.example.raven.pathfindar;

import org.opencv.core.*;

/**
 * Created by Raven on 30-Dec-16.
 */

public class Token {
    int id = 0;
    String name = "";
    Point location = new Point(0.0,0.0);
    int mRange = 0;
    int mRemain = 0;
    int aRange = 0;
    int lifespan = 0;
    boolean found = false;
    Scalar colour = new Scalar(0,0,0);
}
