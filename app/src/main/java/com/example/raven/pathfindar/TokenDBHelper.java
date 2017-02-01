package com.example.raven.pathfindar;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;
import android.hardware.Camera;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by Raven on 31-Jan-17.
 */

public class TokenDBHelper extends SQLiteOpenHelper{
    private static final int DATABASE_VERSION = 1;
    private static final String DATABASE_FILENAME = "tokens.db";    // Create the database

    private static final String CREATE_STATEMENT = "" +             // Create the table
            "create table tokens(" +
            "  _id integer primary key autoincrement not null," +
            "  name text not null," +
            "  mrange integer not null," +
            "  w1name text null," +
            "  w1range integer not null," +
            "  w1ranged integer not null," +
            "  w1reach integer not null," +
            "  w2name text null," +
            "  w2range integer not null," +
            "  w2ranged integer not null," +
            "  w2reach integer not null," +
            "  w3name text null," +
            "  w3range integer not null," +
            "  w3ranged integer not null," +
            "  w3reach integer not null," +
            "  w4name text null," +
            "  w4range integer not null," +
            "  w4ranged integer not null," +
            "  w4reach integer not null)";

    private static final String DROP_STATEMENT = "" +
            "drop table tokens";

    public TokenDBHelper(Context context) {
        super(context, DATABASE_FILENAME, null, DATABASE_VERSION);
    }

    @Override
    public void onCreate(SQLiteDatabase db) {
        db.execSQL(CREATE_STATEMENT);
    }

    @Override
    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
        db.execSQL(DROP_STATEMENT);
        db.execSQL(CREATE_STATEMENT);
    }

    public void deleteElementByID(String id) {                      // Delete an element based on ID
        SQLiteDatabase db = this.getWritableDatabase();
        db.delete("places", "_id = ?", new String[] { ""+id });
    }

    public void addNewElement(Token token) {                        // Add a new place to the database
        SQLiteDatabase db = this.getWritableDatabase();
        ContentValues values = new ContentValues();
        values.put("name", token.name);
        values.put("mrange", token.mRange);
        values.put("w1name", token.w1.name);
        values.put("w1range", token.w1.range);

        if (token.w1.ranged){
            values.put("w1ranged", 1);
        } else {
            values.put("w1ranged", 0);
        }

        if (token.w1.reach){
            values.put("w1reach", 1);
        } else {
            values.put("w1reach", 0);
        }

        values.put("w2name", token.w2.name);
        values.put("w2range", token.w2.range);

        if (token.w2.ranged){
            values.put("w2ranged", 1);
        } else {
            values.put("w2ranged", 0);
        }

        if (token.w2.reach){
            values.put("w2reach", 1);
        } else {
            values.put("w2reach", 0);
        }

        values.put("w3name", token.w3.name);
        values.put("w3range", token.w3.range);

        if (token.w3.ranged){
            values.put("w3ranged", 1);
        } else {
            values.put("w3ranged", 0);
        }

        if (token.w3.reach){
            values.put("w3reach", 1);
        } else {
            values.put("w3reach", 0);
        }

        values.put("w4name", token.w4.name);
        values.put("w4range", token.w4.range);

        if (token.w4.ranged){
            values.put("w4ranged", 1);
        } else {
            values.put("w4ranged", 0);
        }

        if (token.w4.reach){
            values.put("w4reach", 1);
        } else {
            values.put("w4reach", 0);
        }

        db.insert("tokens", null, values);
    }

    public List<Token> getAllElements() {                       // Return a list of all elements
        SQLiteDatabase db = this.getReadableDatabase();
        ArrayList<Token> results = new ArrayList<>();

        String[] columns = new String[] {"_id",
                "name",
                "mrange",
                "w1name",
                "w1range",
                "w1ranged",
                "w1reach",
                "w2name",
                "w2range",
                "w2ranged",
                "w2reach",
                "w3name",
                "w3range",
                "w3ranged",
                "w3reach",
                "w4name",
                "w4range",
                "w4ranged",
                "w4reach"};

        String where = "";  // all contacts
        String[] whereArgs = new String[] {};
        String groupBy = "";  // no grouping
        String groupArgs = "";
        String orderBy = "name";

        Cursor cursor = db.query("places", columns, where, whereArgs,
                groupBy, groupArgs, orderBy);
        cursor.moveToFirst();
        while(!cursor.isAfterLast()) {
            int id = cursor.getInt(0);
            String name = cursor.getString(1);
            int mrange = cursor.getInt(2);
            String w1name = cursor.getString(3);
            int w1range = cursor.getInt(4);
            int w1ranged = cursor.getInt(5);
            int w1reach = cursor.getInt(6);
            String w2name = cursor.getString(7);
            int w2range = cursor.getInt(8);
            int w2ranged = cursor.getInt(9);
            int w2reach = cursor.getInt(10);
            String w3name = cursor.getString(11);
            int w3range = cursor.getInt(12);
            int w3ranged = cursor.getInt(13);
            int w3reach = cursor.getInt(14);
            String w4name = cursor.getString(15);
            int w4range = cursor.getInt(16);
            int w4ranged = cursor.getInt(17);
            int w4reach = cursor.getInt(18);

            Token tok = new Token();
            tok.id = id;
            tok.name = name;
            tok.mRange = mrange;
            tok.w1.name = w1name;
            tok.w1.range = w1range;
            if (w1ranged == 1){
                tok.w1.ranged = true;
            } else {
                tok.w1.ranged = false;
            }
            if (w1reach == 1){
                tok.w1.reach = true;
            } else {
                tok.w1.reach = false;
            }
            tok.w2.name = w2name;
            tok.w2.range = w2range;
            if (w2ranged == 1){
                tok.w2.ranged = true;
            } else {
                tok.w2.ranged = false;
            }
            if (w2reach == 1){
                tok.w2.reach = true;
            } else {
                tok.w2.reach = false;
            }
            tok.w3.name = w3name;
            tok.w3.range = w3range;
            if (w3ranged == 1){
                tok.w3.ranged = true;
            } else {
                tok.w3.ranged = false;
            }
            if (w3reach == 1){
                tok.w3.reach = true;
            } else {
                tok.w3.reach = false;
            }
            tok.w4.name = w4name;
            tok.w4.range = w4range;
            if (w4ranged == 1){
                tok.w4.ranged = true;
            } else {
                tok.w4.ranged = false;
            }
            if (w4reach == 1){
                tok.w4.reach = true;
            } else {
                tok.w4.reach = false;
            }

            results.add(tok);

            cursor.moveToNext();
        }
        return results;
    }

}
