package com.example.raven.pathfindar;

/**
 * Created by Raven on 02-Jan-17.
 */

public class Weapon {
    String name = "";
    int range = 5;
    boolean reach = false;
    boolean ranged = false;

    public Weapon(String name, int range, boolean reach, boolean ranged) {
        this.name = name;
        this.range = range;
        this.reach = reach;
        this.ranged = ranged;
    }

    public Weapon(String name, int range) {
        this.name = name;
        this.range = range;
    }

    public String toString(){
        String rangedS = "";
        String reachS = "";

        if (ranged){
            rangedS = " - Ranged";
        }

        if (reach){
            reachS = " - Reach";
        }
        return this.name + ": " + this.range + "ft." + reachS + rangedS ;
    }
}
