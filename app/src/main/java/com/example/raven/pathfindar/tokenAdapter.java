package com.example.raven.pathfindar;

import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;

import java.util.ArrayList;

/**
 * Created by Raven on 01-Jan-17.
 */

public class tokenAdapter extends BaseAdapter {
    private Context context;
    private ArrayList<Token> data;

    public tokenAdapter(Context context, ArrayList<Token> data) {
        this.context = context;
        this.data = data;
    }

    @Override
    public int getCount() {
        return data.size();
    }

    @Override
    public Object getItem(int position) {
        return data.get(position);
    }

    @Override
    public long getItemId(int position) {
        return data.get(position).id;
    }

    @Override
    public View getView(final int position, View convertView, ViewGroup parent) {

        Token tokenToDisplay = data.get(position);

        if (convertView == null){
            LayoutInflater inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            convertView = inflater.inflate(R.layout.token_data, parent, false);
        }

        TextView tvName = (TextView)convertView.findViewById(R.id.tv_tokenName);
        tvName.setText(tokenToDisplay.name);

        TextView tvMove = (TextView)convertView.findViewById(R.id.tv_movement);
        tvMove.setText(Integer.toString(tokenToDisplay.mRange) + "ft.");

        TextView tvPosition = (TextView)convertView.findViewById(R.id.tv_position);
        tvPosition.setText("(" + Integer.toString((int)tokenToDisplay.location.y) + "," + Integer.toString((int)tokenToDisplay.location.x) + ")");
        // There's an error in my grid with the way it's indexed. the x and y positions are switched.
        // Makes no difference for the math, though, so unless I have the time to fix it, I'm just going to invert the display.

        TextView tvARange1 = (TextView)convertView.findViewById(R.id.tv_range1);
        tvARange1.setText(tokenToDisplay.w1.toString());

        TextView tvARange2 = (TextView)convertView.findViewById(R.id.tv_range2);
        tvARange2.setText(tokenToDisplay.w2.toString());

        TextView tvARange3 = (TextView)convertView.findViewById(R.id.tv_range3);
        tvARange3.setText(tokenToDisplay.w3.toString());

        TextView tvARange4 = (TextView)convertView.findViewById(R.id.tv_range4);
        tvARange4.setText(tokenToDisplay.w4.toString());

        TextView tvFound = (TextView)convertView.findViewById(R.id.tv_found);
        if (tokenToDisplay.found){
            tvFound.setText("True");
        } else {
            tvFound.setText("False");
        }

        Color nColour = new Color();
        ImageView ivColour = (ImageView)convertView.findViewById(R.id.iv_tokenColour);
        ivColour.setBackgroundColor(nColour.rgb((int)tokenToDisplay.colour.val[0], (int)tokenToDisplay.colour.val[1], (int)tokenToDisplay.colour.val[2]));

        final Button editButton = (Button)convertView.findViewById(R.id.bt_editToken);
        editButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(context, tokenEditMenu.class);
                intent.putExtra("pos", position);
                context.startActivity(intent);
            }
        });

        return convertView;
    }


}
