package com.example.raven.pathfindar;

import android.content.Context;
import android.graphics.Color;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
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
        tvName.setText(data.get(position).name);

        TextView tvMove = (TextView)convertView.findViewById(R.id.tv_movement);
        tvMove.setText(Integer.toString(data.get(position).mRange) + "ft.");

        TextView tvPosition = (TextView)convertView.findViewById(R.id.tv_position);
        tvPosition.setText("(" + Integer.toString((int)data.get(position).location.x) + "," + Integer.toString((int)data.get(position).location.y) + ")");

        TextView tvARange1 = (TextView)convertView.findViewById(R.id.tv_range1);
        tvARange1.setText(Integer.toString(data.get(position).aRange) + "ft.");

        Color nColour = new Color();
        ImageView ivColour = (ImageView)convertView.findViewById(R.id.iv_tokenColour);
        ivColour.setBackgroundColor(nColour.rgb((int)data.get(position).colour.val[0], (int)data.get(position).colour.val[1], (int)data.get(position).colour.val[2]));

        return convertView;
    }


}
