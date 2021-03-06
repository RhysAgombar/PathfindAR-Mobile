#include <jni.h>
#include <string>
#include <opencv2/aruco.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/calib3d.hpp>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <time.h>

template <typename T>
std::string to_string(T value)
{
    std::ostringstream os ;
    os << value ;
    return os.str() ;
}

int hSize = 10;
int vSize = 10;
int** gridCount; //gridCount[11][11];

std::vector<cv::Point> squaresToColour;
int selectedID = -1;
int selectedWeapon = -1;

int gridType = 0;
static const int MOVEMENT = 0;
static const int ATTACK = 1;
static const int PLOTTING = 2;

static const int LINE = 0;
static const int CONE = 1;
static const int SPHERE = 2;

int selectedBlast = -1;
int blastRadius = 0;
int mRemain = 0;

std::vector<std::string> movementPieces;


class gridSquare {
public:
    cv::Point corner[4];
    std::vector<cv::Point> getPointsArray() {
        std::vector<cv::Point> cornerVec(4);
        for (int i = 0; i < 4; i++) {
            cornerVec.at(i) = corner[i];
        }
        return cornerVec;
    }

    bool contains(cv::Point center) {

        int nvert = 4;
        int i, j, c = 0;

        for(i = 0, j = nvert - 1; i < nvert; j = i++) {
            if( ( (corner[i].y >= center.y ) != (corner[j].y >= center.y) ) &&
                (center.x <= (corner[j].x - corner[i].x) * (center.y - corner[i].y) / (corner[j].y - corner[i].y) + corner[i].x)
                    )
                c = !c;
        }

        return c;

        //std::vector<cv::Point> cVec = getPointsArray();
        //int result = cv::pointPolygonTest(cVec, center, false); // doesn't work
        //if (result == 1) {
        //    return true;
        //} else {
        //    return false;
        //}
    }

    void shadeSquare(cv::Mat imageOut, cv::Scalar colour) {
        cv::Point polyPoints[1][4];
        polyPoints[0][0] = corner[0];
        polyPoints[0][1] = corner[1];
        polyPoints[0][2] = corner[2];
        polyPoints[0][3] = corner[3];

        const cv::Point* ppt[1] = { polyPoints[0] };
        int npt[] = { 4 };

        cv::fillPoly(imageOut, ppt, npt, 1, colour);
    }
    // add  shading functions
};

std::vector<cv::Point> colourRadiusFromSquare(cv::Point pos, int radius, int exclusionRadius){
    radius = radius / 5; // Convert from ft. to squares

    double countRadius = 0.0;

    int diagonal = 0;
    int holder = 0;

    std::vector<cv::Point> positions;
    std::vector<cv::Point> excluded;

    if (exclusionRadius > 0){
        excluded = colourRadiusFromSquare(pos, exclusionRadius, 0);
    }

    while (floor(countRadius + 1.5) <= radius){
        countRadius += 1.5;
        diagonal++;
    }

    countRadius = 0.0;

    // Upper Right
    holder = diagonal;
    while (diagonal >= 0) {
        cv::Point current;
        current.x = pos.x - diagonal;
        current.y = pos.y + diagonal;

        if (current.x != pos.x){
            if (current.y != pos.y) {

                bool found = false;
                for (int i = 0; i < excluded.size(); i++){
                    if (current == excluded.at(i)){
                        found = true;
                    }
                }

                if (!found) {
                    positions.push_back(current);
                }

            }
        }

        countRadius = 1.5 * diagonal;

        while (countRadius < radius){
            countRadius += 1.0;
            current.x -= 1;

            bool found = false;
            for (int i = 0; i < excluded.size(); i++){
                if (current == excluded.at(i)){
                    found = true;
                }
            }

            if (!found) {
                positions.push_back(current);
            }
        }

        diagonal--;
    }

    diagonal = holder;
    while (diagonal >= 0) {
        cv::Point current;
        current.x = pos.x - diagonal;
        current.y = pos.y + diagonal;

        if (current.x != pos.x){
            if (current.y != pos.y) {
                bool found = false;
                for (int i = 0; i < excluded.size(); i++){
                    if (current == excluded.at(i)){
                        found = true;
                    }
                }

                if (!found) {
                    positions.push_back(current);
                }
            }
        }

        countRadius = 1.5 * diagonal;

        while (countRadius < radius){
            countRadius += 1.0;
            current.y += 1;

            bool found = false;
            for (int i = 0; i < excluded.size(); i++){
                if (current == excluded.at(i)){
                    found = true;
                }
            }

            if (!found) {
                positions.push_back(current);
            }
        }

        diagonal--;
    }


    // Lower Right
    diagonal = holder;
    while (diagonal >= 0) {
        cv::Point current;
        current.x = pos.x + diagonal;
        current.y = pos.y + diagonal;

        if (current.x != pos.x){
            if (current.y != pos.y) {
                bool found = false;
                for (int i = 0; i < excluded.size(); i++){
                    if (current == excluded.at(i)){
                        found = true;
                    }
                }

                if (!found) {
                    positions.push_back(current);
                }
            }
        }

        countRadius = 1.5 * diagonal;

        while (countRadius < radius){
            countRadius += 1.0;
            current.x += 1;

            bool found = false;
            for (int i = 0; i < excluded.size(); i++){
                if (current == excluded.at(i)){
                    found = true;
                }
            }

            if (!found) {
                positions.push_back(current);
            }
        }

        diagonal--;
    }

    diagonal = holder;
    while (diagonal >= 0) {
        cv::Point current;
        current.x = pos.x + diagonal;
        current.y = pos.y + diagonal;

        if (current.x != pos.x){
            if (current.y != pos.y) {
                bool found = false;
                for (int i = 0; i < excluded.size(); i++){
                    if (current == excluded.at(i)){
                        found = true;
                    }
                }

                if (!found) {
                    positions.push_back(current);
                }
            }
        }

        countRadius = 1.5 * diagonal;

        while (countRadius < radius){
            countRadius += 1.0;
            current.y += 1;

            bool found = false;
            for (int i = 0; i < excluded.size(); i++){
                if (current == excluded.at(i)){
                    found = true;
                }
            }

            if (!found) {
                positions.push_back(current);
            }
        }

        diagonal--;
    }

    // Lower Left
    diagonal = holder;
    while (diagonal >= 0) {
        cv::Point current;
        current.x = pos.x + diagonal;
        current.y = pos.y - diagonal;

        if (current.x != pos.x){
            if (current.y != pos.y) {
                bool found = false;
                for (int i = 0; i < excluded.size(); i++){
                    if (current == excluded.at(i)){
                        found = true;
                    }
                }

                if (!found) {
                    positions.push_back(current);
                }
            }
        }

        countRadius = 1.5 * diagonal;

        while (countRadius < radius){
            countRadius += 1.0;
            current.x += 1;

            bool found = false;
            for (int i = 0; i < excluded.size(); i++){
                if (current == excluded.at(i)){
                    found = true;
                }
            }

            if (!found) {
                positions.push_back(current);
            }
        }

        diagonal--;
    }

    diagonal = holder;
    while (diagonal >= 0) {
        cv::Point current;
        current.x = pos.x + diagonal;
        current.y = pos.y - diagonal;

        if (current.x != pos.x){
            if (current.y != pos.y) {
                bool found = false;
                for (int i = 0; i < excluded.size(); i++){
                    if (current == excluded.at(i)){
                        found = true;
                    }
                }

                if (!found) {
                    positions.push_back(current);
                }
            }
        }

        countRadius = 1.5 * diagonal;

        while (countRadius < radius){
            countRadius += 1.0;
            current.y -= 1;

            bool found = false;
            for (int i = 0; i < excluded.size(); i++){
                if (current == excluded.at(i)){
                    found = true;
                }
            }

            if (!found) {
                positions.push_back(current);
            }
        }

        diagonal--;
    }

    // Upper Left
    diagonal = holder;
    while (diagonal >= 0) {
        cv::Point current;
        current.x = pos.x - diagonal;
        current.y = pos.y - diagonal;

        if (current.x != pos.x){
            if (current.y != pos.y) {
                bool found = false;
                for (int i = 0; i < excluded.size(); i++){
                    if (current == excluded.at(i)){
                        found = true;
                    }
                }

                if (!found) {
                    positions.push_back(current);
                }
            }
        }

        countRadius = 1.5 * diagonal;

        while (countRadius < radius){
            countRadius += 1.0;
            current.x -= 1;

            bool found = false;
            for (int i = 0; i < excluded.size(); i++){
                if (current == excluded.at(i)){
                    found = true;
                }
            }

            if (!found) {
                positions.push_back(current);
            }
        }

        diagonal--;
    }

    diagonal = holder;
    while (diagonal >= 0) {
        cv::Point current;
        current.x = pos.x - diagonal;
        current.y = pos.y - diagonal;

        if (current.x != pos.x){
            if (current.y != pos.y) {
                bool found = false;
                for (int i = 0; i < excluded.size(); i++){
                    if (current == excluded.at(i)){
                        found = true;
                    }
                }

                if (!found) {
                    positions.push_back(current);
                }
            }
        }

        countRadius = 1.5 * diagonal;

        while (countRadius < radius){
            countRadius += 1.0;
            current.y -= 1;

            bool found = false;
            for (int i = 0; i < excluded.size(); i++){
                if (current == excluded.at(i)){
                    found = true;
                }
            }

            if (!found) {
                positions.push_back(current);
            }
        }

        diagonal--;
    }

    //squaresToColour = positions;

    return positions;
};

class arMarker {
public:
    cv::Point corner[4];
    cv::Point center;
    int id;
};
gridSquare** grid;
cv::Point **hLines;
cv::Point **vLines;
arMarker arUco[4];
bool start;

cv::Point touchPoint = cv::Point(-1,-1);

class Weapon {
public:
    std::string name = "";
    int range = 5;
    bool reach = false;
    bool ranged = false;
};

// Token Variables
class Token {
public:
    int id = 0;
    std::string name = "";
    cv::Point location = cv::Point(0.0,0.0);
    int mRange = 30;
    //int mRemain = 30;
    int lifespan = 0;
    bool found = false;
    cv::Scalar colour = cv::Scalar(0,0,0);
    Weapon w1, w2, w3, w4;

    std::string toString() {
        std::string out = to_string(id) + "¦";
        out = out.append(name + "¦");
        out = out.append(to_string(location.x) + "¦");
        out = out.append(to_string(location.y) + "¦");
        out = out.append(to_string(mRange) + "¦");
        out = out.append(to_string(lifespan) + "¦");
        out = out.append(to_string(found) + "¦");
        out = out.append(w1.name + "¦");
        out = out.append(to_string(w1.range) + "¦");
        out = out.append(to_string(w1.reach) + "¦");
        out = out.append(to_string(w1.ranged) + "¦");
        out = out.append(w2.name + "¦");
        out = out.append(to_string(w2.range) + "¦");
        out = out.append(to_string(w2.reach) + "¦");
        out = out.append(to_string(w2.ranged) + "¦");
        out = out.append(w3.name + "¦");
        out = out.append(to_string(w3.range) + "¦");
        out = out.append(to_string(w3.reach) + "¦");
        out = out.append(to_string(w3.ranged) + "¦");
        out = out.append(w4.name + "¦");
        out = out.append(to_string(w4.range) + "¦");
        out = out.append(to_string(w4.reach) + "¦");
        out = out.append(to_string(w4.ranged) + "¦");
        out = out.append(to_string(colour.val[0]) + "," + to_string(colour.val[1]) + "," + to_string(colour.val[2]) + "," + to_string(colour.val[3]) + "│");

        return out;
    }
};

int idGen = 0;
std::vector<Token> tokenVec;
std::vector<Token> psuedoTokenVec;
std::vector<cv::Point> uTokens;
cv::RNG rng(12345);


float distance(cv::Point p1, cv::Point p2) {
    return sqrt(pow(abs(p2.x - p1.x), 2.0) + pow(abs(p2.y - p1.y), 2.0));
}

std::vector<cv::Point2f> updateCorners(std::vector<std::vector<cv::Point2f> > corners, std::vector<int> ids) {

    std::vector<cv::Point2f> gridCorners(4);

    cv::Point2f midPoint;
    midPoint.x = 0;
    midPoint.y = 0;

    for (int i = 0; i < corners.size(); i++) {
        for (int j = 0; j < 4; j++) {
            midPoint.x += corners.at(i).at(j).x;
            midPoint.y += corners.at(i).at(j).y;
        }
    }

    midPoint.x = midPoint.x / (corners.size() * 4);
    midPoint.y = midPoint.y / (corners.size() * 4);



    for (int i = 0; i < corners.size(); i++) {

        float pDist = INFINITY;

        for (int j = 0; j < 4; j++) {
            float dist = distance(corners.at(i).at(j), midPoint);

            if (dist < pDist) {
                pDist = dist;
                gridCorners.at(ids.at(i)) = corners.at(i).at(j);

                // updates arUco markers
                for (int k = 0; k < 4; k++) {
                    arUco[ids.at(i)].corner[k] = corners.at(i).at(k);
                }

                arUco[ids.at(i)].id = ids.at(i);

                cv::Point arUcoCenter = cv::Point(0, 0);
                for (int l = 0; l < 4; l++) {
                    arUcoCenter.x += arUco[ids.at(i)].corner[l].x;
                    arUcoCenter.y += arUco[ids.at(i)].corner[l].y;
                }

                arUcoCenter.x = arUcoCenter.x / 4;
                arUcoCenter.y = arUcoCenter.y / 4;

                arUco[ids.at(i)].center = arUcoCenter;

            }

        }
    }

    return gridCorners;

}

std::vector<cv::Point2f> findCorners(std::vector<std::vector<cv::Point2f> > corners, std::vector<int> ids) {

    if (corners.size() == 4) {

        // if all markers are found, we're good to go. Update the markers.
        return updateCorners(corners, ids);

    }  else {

        // If only some of the markers are found, we need to compensate.
        for (int i = 0; i < corners.size(); i++) { // for each detected marker...

            cv::Point center; // Get the center of it
            center.x = (corners[i][0].x + corners[i][1].x + corners[i][2].x + corners[i][3].x) / 4;
            center.y = (corners[i][0].y + corners[i][1].y + corners[i][2].y + corners[i][3].y) / 4;

            float dist = 1e9;
            float ndist;
            int swap = 0;

            for (int j = 0; j < 4; j++) { // out of all the previously detected markers

                ndist = distance(center, arUco[j].center); // find the closest marker to the detected one

                if (ndist < dist)
                {
                    dist = ndist;
                    swap = j; // Save the location of the marker to update
                }
            }

            arUco[swap].corner[0] = corners[i][0]; // update marker
            arUco[swap].corner[1] = corners[i][1];
            arUco[swap].corner[2] = corners[i][2];
            arUco[swap].corner[3] = corners[i][3];

            arUco[swap].center = center;
            // In this way, we only update the markers we have info for. The missing markers retain their last known value.
        }

        // Format the data in such a way that the Update function can read it.
        std::vector<std::vector<cv::Point2f> > cCorners(4, std::vector<cv::Point2f> (4));
        std::vector<int> cIds(4);

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                cCorners.at(i).at(j) = arUco[i].corner[j];
            }
            cIds.at(i) = arUco[i].id;
        }

        return updateCorners(cCorners, cIds);

    }

}

cv::Point findIntersection(cv::Point h1, cv::Point h2, cv::Point v1, cv::Point v2) {

    cv::Point x = v1 - h1;
    cv::Point hDist = h2 - h1;
    cv::Point vDist = v2 - v1;

    float cross = hDist.x*vDist.y - hDist.y*vDist.x;

    // I'm not sure how this part works, but it does.
    double t = (x.x * vDist.y - x.y * vDist.x) / cross;
    cv::Point intersection = h1 + hDist * t;

    return intersection;

}



// Token Code
cv::Point getCenter(std::vector<cv::Point> contour) {
    // We assume all contours are somewhat balanced, therefore the center is determined using a bounding box.
    int minX = 1e9;
    int maxX = -1;
    int minY = 1e9;
    int maxY = -1;

    int boundXmin = 1e9, boundXmax = -1; // Bounding box around the aruco markers to avoid contours outside the grid from being counted - Improves accuracy and performance
    int boundYmin = 1e9, boundYmax = -1;

    for (int i = 0; i < 4; i++){
        if (arUco[i].center.x > boundXmax) {
            boundXmax = arUco[i].center.x;
        }
        if (arUco[i].center.y > boundYmax) {
            boundYmax = arUco[i].center.y;
        }
        if (arUco[i].center.x < boundXmin){
            boundXmin = arUco[i].center.x;
        }
        if (arUco[i].center.y < boundYmin){
            boundYmin = arUco[i].center.y;
        }
    }


    for (int i = 0; i < contour.size() - 1; i++) {

        if (contour.at(i).x > boundXmax || contour.at(i).x < boundXmin){
            return cv::Point(-1,-1);
        }

        if (contour.at(i).y > boundYmax || contour.at(i).y < boundYmin){
            return cv::Point(-1,-1);
        }

        if (contour.at(i).x < minX) {
            minX = contour.at(i).x;
        }
        if (contour.at(i).x > maxX) {
            maxX = contour.at(i).x;
        }

        if (contour.at(i).y < minY) {
            minY = contour.at(i).y;
        }
        if (contour.at(i).y > maxY) {
            maxY = contour.at(i).y;
        }

    }
    return cv::Point((minX + maxX) / 2, (minY + maxY) / 2);
}

bool isWithin(cv::Point p1, cv::Point p2, int rad) {
    if (distance(p1, p2) <= rad) {
        return true;
    }
    else {
        return false;
    }
}

void idTokens() {
    int closestDist = 1e9;
    int pos = 0;

    for (int i = 0; i < tokenVec.size(); i++) {
        tokenVec.at(i).found = false;
    }

    if (tokenVec.size() > 0) {
        std::vector<std::vector<float> > uTokenDists;

        for (int i = 0; i < tokenVec.size(); i++) {
            std::vector<float> newVec;
            uTokenDists.push_back(newVec);
            for (int k = 0; k < uTokens.size(); k++) {
                uTokenDists.at(i).push_back(distance(tokenVec.at(i).location, uTokens.at(k)));
            }
        }

        for (int k = 0; k < uTokens.size(); k++) {
            float dist = 1e9;
            int pos = -1;

            for (int i = 0; i < tokenVec.size(); i++) {
                if (uTokenDists.at(i).at(k) < dist) {
                    float tempDist = uTokenDists.at(i).at(k);
                    bool flag = true;

                    for (int j = 0; j < uTokens.size(); j++) {
                        if (tempDist > uTokenDists.at(i).at(j)) {
                            flag = false;
                        }
                    }

                    if (flag == true) {
                        dist = uTokenDists.at(i).at(k);

                        if (dist > 2) {
                            if (dist < 3) {
                                int stop = 0;
                                stop++;
                            }
                        }
                        pos = i;
                    }
                }
            }

            if (pos >= 0) {
                if (tokenVec.at(pos).found == false) {
                    tokenVec.at(pos).location = uTokens.at(k);
                    tokenVec.at(pos).found = true;
                    uTokens.at(k).x = -1e9;
                }
            }
        }
    }

    for (int k = 0; k < uTokens.size(); k++) {
        if (uTokens.at(k).x > -1e4) {
            Token newToken;
            newToken.location = uTokens.at(k);
            newToken.lifespan = 1;
            newToken.id = idGen++;
            newToken.name = to_string(idGen);
            newToken.found = true;

            newToken.colour = cv::Scalar(rand() % 255, rand() % 255, rand() % 255);

            tokenVec.push_back(newToken);
        }
    }


    std::vector<Token> newTokenVec;

    for (int i = 0; i < tokenVec.size(); i++) {
        if (tokenVec.at(i).found == true) {
            tokenVec.at(i).lifespan++;

            if (tokenVec.at(i).lifespan > 25) {
                tokenVec.at(i).lifespan = 25;
            }

        }
        else {
            tokenVec.at(i).lifespan--;
        }

        if (tokenVec.at(i).lifespan > 0) {
            newTokenVec.push_back(tokenVec.at(i));
        }

    }

    tokenVec = newTokenVec;

    uTokens.clear();

}

void findTokens(cv::Mat imageCopy, cv::Mat imageOut) {
    cv::Mat imageGray, image2;
    std::vector<std::vector<cv::Point> > contours, cont2;
    std::vector<cv::Vec4i> hierarchy;

    cv::cvtColor(imageCopy,imageGray,CV_RGB2GRAY);
    cv::GaussianBlur(imageGray, imageGray, cv::Size(5, 5), 1.5, 1.5);
    cv::Canny(imageGray, imageGray, 100, 200, 3);
    cv::findContours(imageGray, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE,cv::Point(0, 0));

    cv::Mat drawing = cv::Mat::zeros(imageGray.size(), CV_8UC3);
    cv::Mat con = cv::Mat::zeros(imageGray.size(), CV_8UC3);

    for (int i = 0; i < contours.size(); i++) {
        if (contours.at(i).capacity() > 3) { // Eliminates the remaining noise
            cv::Point cent = getCenter(contours.at(i));

            if (cent.x != -1 && cent.y != -1) {
                bool flag = true;

                // eliminate aruco markers
                for (int j = 0; j < 4; j++) {
                    if (isWithin(cent, arUco[j].center, (0.75 * distance(arUco[j].corner[0], arUco[j].corner[2])))) {
                        flag = false;
                    }
                }

                if (flag == true) {
                    //if (hierarchy[i][3] < 0) {
                    cont2.push_back(contours.at(i));
                    //}
                }
            }

        }
    }

    for (int i = 0; i < cont2.size(); i++)
    {
        cv::Scalar color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));

        drawContours(con, cont2, i, color, 2, 8, hierarchy, 0, cv::Point());


        // IDEA! Let's take advantage of the fact that all the contours for each token will be situated around a central point.
        // Let's find the squares where more than 1 or 2 countour centers are found (to eliminate noise/displacement)
        cv::Point cent = getCenter(cont2.at(i));

        for (int j = 0; j < vSize; j++) {
            for (int k = 0; k < hSize; k++) {
                //bool contains = grid[j][k].contains(cent);

                gridSquare test = grid[j][k];


                if (grid[j][k].contains(cent) == true) {
                    gridCount[j][k]++;
                }

                // something is wrong with my grid assignment
/*
        cv::circle(drawing, grid[j][k].corner[0], 5, cv::Scalar(255, 0, 0), -1);
        cv::circle(drawing, grid[j][k].corner[1], 5, cv::Scalar(255, 0, 0), -1);
        cv::circle(drawing, grid[j][k].corner[2], 5, cv::Scalar(255, 0, 0), -1);
        cv::circle(drawing, grid[j][k].corner[3], 5, cv::Scalar(255, 0, 0), -1);*/

                //cv::imshow("Contours", con);

                //cv::waitKey(0);

            }
        }

        for (int j = 0; j < vSize; j++) {
            for (int k = 0; k < hSize; k++) {

                int holder = gridCount[j][k];

                if (gridCount[j][k] > 1) { // Requiring more than one countour in a square cuts down on noise/drift a bit

                    //std::vector<cv::Point> testVec = grid[j][k].getPointsArray();

                    cv::Point polyPoints[1][4];
                    polyPoints[0][0] = grid[j][k].corner[0];
                    polyPoints[0][1] = grid[j][k].corner[1];
                    polyPoints[0][2] = grid[j][k].corner[2];
                    polyPoints[0][3] = grid[j][k].corner[3];

                    const cv::Point* ppt[1] = { polyPoints[0] };
                    int npt[] = { 4 };

                    //cv::fillPoly(imageOut, ppt, npt, 1, cv::Scalar(255, 255, 255));

                    gridCount[j][k] = 0;

                    if (!(find(uTokens.begin(), uTokens.end(), cv::Point(j, k)) != uTokens.end())) {
                        uTokens.push_back(cv::Point(j, k));
                    }

                }

            }
        }

       //cv::circle(imageOut, cent, 5, cv::Scalar(255, 0, 255), -1);

    }


    bool pauseTracking = false;
    float squareAverage = 0;
    bool flag;


    for (int i = 0; i < uTokens.size(); i++){
        flag = true;
        Token nTok;
        nTok.location = uTokens.at(i);
        for (int j = 0; j < psuedoTokenVec.size(); j++){
            if (nTok.location == psuedoTokenVec.at(j).location){
                flag = false;
            }
        }

        if (flag){
            psuedoTokenVec.push_back(nTok);
        }
    }


    for (int i = 0; i < psuedoTokenVec.size(); i++){
        flag = false;
        for (int j = 0; j < uTokens.size(); j++){
            if (uTokens.at(j) == psuedoTokenVec.at(i).location){
                psuedoTokenVec.at(i).lifespan++;
                flag = true;
            }
        }
        if (!flag){

            if (psuedoTokenVec.at(i).lifespan > 7){
                psuedoTokenVec.at(i).lifespan = 7;
            }
            psuedoTokenVec.at(i).lifespan--;

        }
    }


    std::vector<Token> psuedoHolder;

    for (int i = 0; i < psuedoTokenVec.size(); i++){
        if (psuedoTokenVec.at(i).lifespan >= 0){
            psuedoHolder.push_back(psuedoTokenVec.at(i));
        }
    }

    psuedoTokenVec.clear();
    for (int i = 0; i < psuedoHolder.size(); i++){
        psuedoTokenVec.push_back(psuedoHolder.at(i));
    }
    psuedoHolder.clear();

    uTokens.clear();
    for (int i = 0; i < psuedoTokenVec.size(); i++){
        if (psuedoTokenVec.at(i).lifespan > 7){
            uTokens.push_back(psuedoTokenVec.at(i).location);
        }
    }

    idTokens();
    uTokens.clear();

}

void drawTokens(cv::Mat imageOut){
    cv::Mat hMat;
    imageOut.copyTo(hMat);

    for (int i = 0; i < tokenVec.size(); i++) {

        int j = tokenVec.at(i).location.x;
        int k = tokenVec.at(i).location.y;

        cv::Point polyPoints[1][4];
        polyPoints[0][0] = grid[j][k].corner[0];
        polyPoints[0][1] = grid[j][k].corner[1];
        polyPoints[0][2] = grid[j][k].corner[2];
        polyPoints[0][3] = grid[j][k].corner[3];

        grid[j][k].shadeSquare(imageOut, tokenVec.at(i).colour);

    }

}

void computeGrid(cv::Mat imageCopy, std::vector<cv::Point2f> gridCorners, int horiz, int vert) {
    cv::Mat dirVec[4]; // top left -> top right, bottom left -> bottom right, top left -> bottom left, top right -> bottom right
    float hDist, vDist, dist;

    std::vector<cv::Point2f> hCorners;

    if (gridCorners.size() >= 3) {

        for (int i = 0; i <= vert; i++) {
            float distx;
            float disty;

            distx = gridCorners.at(3).x - gridCorners.at(0).x;
            disty = gridCorners.at(3).y - gridCorners.at(0).y;

           /* float ratio = fabs((gridCorners.at(3).x - gridCorners.at(2).x) / (gridCorners.at(1).x - gridCorners.at(0).x));
            float step = ((ratio - 1) / vert);
            float adjust = ratio - 1; */


            hLines[i][0] = cv::Point(gridCorners.at(0).x + (distx * (i / (float)vert)), gridCorners.at(0).y + (disty) * (float)(i / (float)vert)); // (adjust + step * i)

            distx = gridCorners.at(2).x - gridCorners.at(1).x;
            disty = gridCorners.at(2).y - gridCorners.at(1).y;

            hLines[i][1] = cv::Point(gridCorners.at(1).x + (distx * (i / (float)vert)), gridCorners.at(1).y + (disty) * (float)(i / (float)vert));
        }

        for (int i = 0; i <= horiz; i++) {
            float distx;
            float disty;

            distx = gridCorners.at(1).x - gridCorners.at(0).x;
            disty = gridCorners.at(1).y - gridCorners.at(0).y;

            vLines[i][0] = cv::Point(gridCorners.at(0).x + (distx * (i / (float)horiz)), gridCorners.at(0).y + (disty) * (float)(i / (float)horiz));

            distx = gridCorners.at(2).x - gridCorners.at(3).x;
            disty = gridCorners.at(2).y - gridCorners.at(3).y;

            vLines[i][1] = cv::Point(gridCorners.at(3).x + (distx * (i / (float)horiz)), gridCorners.at(3).y + (disty) * (float)(i / (float)horiz));
        }

        for (int i = 0; i < vert; i++) {
            for (int j = 0; j < horiz; j++) {
                grid[i][j].corner[0] = findIntersection(hLines[i][0], hLines[i][1], vLines[j][0], vLines[j][1]);
                grid[i][j].corner[1] = findIntersection(hLines[i][0], hLines[i][1], vLines[j + 1][0], vLines[j + 1][1]);
                grid[i][j].corner[2] = findIntersection(hLines[i + 1][0], hLines[i + 1][1], vLines[j + 1][0], vLines[j + 1][1]);
                grid[i][j].corner[3] = findIntersection(hLines[i + 1][0], hLines[i + 1][1], vLines[j][0], vLines[j][1]);
            }
        }

    }
}

void drawGrid(cv::Mat finalMat, int horiz, int vert){
    for (int i = 0; i <= vert; i++) {
        cv::line(finalMat, hLines[i][0], hLines[i][1], cv::Scalar(255, 255, 255));
    }

    for (int i = 0; i <= horiz; i++) {
        cv::line(finalMat, vLines[i][0], vLines[i][1], cv::Scalar(255, 255, 255));
    }
}

void init() {
    delete [] gridCount;
    delete [] grid;

    gridCount = new int*[vSize];
    for (int i = 0; i < vSize; i++) {
        gridCount[i] = new int[hSize];
    }

    for (int i = 0; i < vSize; i++){
        for (int j = 0; j < hSize; j++){
            gridCount[i][j] = 0;
        }
    }

    int vert = vSize + 1;
    int horiz = hSize + 1;

    grid = new gridSquare*[vert];
    for (int i = 0; i < vert; i++) {
        grid[i] = new gridSquare[horiz];
    }

    hLines = new cv::Point*[vert];
    for (int i = 0; i < vert; i++) {
        hLines[i] = new cv::Point[2];
    }

    vLines = new cv::Point*[horiz];
    for (int i = 0; i < horiz; i++) {
        vLines[i] = new cv::Point[2];
    }

    horiz = horiz - 1;
    vert = vert - 1;

    idGen = 0;
    tokenVec.clear();
    uTokens.clear();
}

void colourRange(int id, double alpha, cv::Mat finalMat){

    Token selToken;
    bool found = false;

    for (int i = 0; i < tokenVec.size(); i++){
        if (tokenVec.at(i).id == id){
            selToken = tokenVec.at(i);
            found = true;
        }
    }

    if (!found){
        return;
    }


    cv::Point np = selToken.location;
    int exclusion = 0;
    cv::Scalar colour[6];
    colour[0] = cv::Scalar(15, 169, 15);
    colour[1] = cv::Scalar(255, 0, 0);  // Tried with a gradiant before. The transparency made it difficult to distinguish colours without having them be vastly different. Having them vastly different looked terrible, so I chose to use a light and a much darker shade of red, alternating.
    colour[2] = cv::Scalar(150, 0, 0);  // Keeping it like this for the time being, in case I find a colour scheme that works
    colour[3] = cv::Scalar(255, 0, 0);
    colour[4] = cv::Scalar(150, 0, 0);
    colour[5] = cv::Scalar(255, 0, 0);

    Weapon selWeapon;
    bool noWeapon = true;

    if (selectedWeapon == 1) {
        noWeapon = false;
        selWeapon = selToken.w1;
    } else if (selectedWeapon == 2) {
        noWeapon = false;
        selWeapon = selToken.w2;
    } else if (selectedWeapon == 3) {
        noWeapon = false;
        selWeapon = selToken.w3;
    } else if (selectedWeapon == 4) {
        noWeapon = false;
        selWeapon = selToken.w4;
    }

    if (gridType == ATTACK) {
        if (!noWeapon) {
            if (selWeapon.reach) {
                exclusion = 5;
            }

            cv::Mat hMat;
            finalMat.copyTo(hMat);

            int itterations = 0;
            if (selWeapon.ranged){
                itterations = 4; //  5 itterations total - Technically, for projectile weapons, you can shoot up to 10 (only 5 for thrown), but that will put you well off the battle mat, considering most are at least 15ft range.
            }

            for (int l = itterations; l >= 0; l--){ // This needs to be optimized... baddly.

                if (l == 0) {
                    squaresToColour = colourRadiusFromSquare(np,selWeapon.range, exclusion);
                } else {
                    squaresToColour = colourRadiusFromSquare(np,selWeapon.range * (l + 1), selWeapon.range * (l));
                }

                if (squaresToColour.size() > 0){
                    for (int i = 0; i < squaresToColour.size(); i++){
                        int j = 0, k = 0;
                        j = squaresToColour.at(i).x;
                        k = squaresToColour.at(i).y;

                        if (j >= 0 && j <= vSize){
                            if (k >= 0 && k <= hSize){
                                grid[j][k].shadeSquare(finalMat, colour[l + 1]);
                            }
                        }

                    }
                }
            }

            addWeighted( finalMat, alpha, hMat, 1.0 - alpha, 0.0, finalMat);
        }
    } else {
        squaresToColour = colourRadiusFromSquare(np,selToken.mRange, exclusion);

        cv::Mat hMat;
        finalMat.copyTo(hMat);

        if (squaresToColour.size() > 0){
            for (int i = 0; i < squaresToColour.size(); i++){
                int j = 0, k = 0;
                j = squaresToColour.at(i).x;
                k = squaresToColour.at(i).y;

                if (j >= 0 && j <= vSize){
                    if (k >= 0 && k <= hSize){
                        grid[j][k].shadeSquare(finalMat, colour[0]);
                    }
                }

            }
        }


        addWeighted( finalMat, alpha, hMat, 1.0 - alpha, 0.0, finalMat);
    }

}

void colourPlot(int id, double alpha, cv::Mat finalMat){

    Token selToken;
    bool found = false;

    for (int i = 0; i < tokenVec.size(); i++){
        if (tokenVec.at(i).id == id){
            selToken = tokenVec.at(i);
            found = true;
        }
    }

    if (!found){
        return;
    }

    cv::Point np = selToken.location;
    cv::Scalar colour[2];
    colour[0] = cv::Scalar(15, 169, 15);
    colour[1] = cv::Scalar(7, 85, 7);

    std::vector<cv::Point> plotSquares;
    plotSquares.push_back(np);

    for (int i = 0; i < movementPieces.size(); i++){
        std::string move = movementPieces.at(i);

        //if (movementPieces.at(i) == "UL" || movementPieces.at(i) == "UR" || movementPieces.at(i) == "DL" || movementPieces.at(i) == "DR"){

        if (move == "UL"){
            plotSquares.push_back(cv::Point(plotSquares.at(i).x - 1, plotSquares.at(i).y - 1));
        } else if (move == "L"){
            plotSquares.push_back(cv::Point(plotSquares.at(i).x, plotSquares.at(i).y - 1));
        } else if (move == "DL"){
            plotSquares.push_back(cv::Point(plotSquares.at(i).x + 1, plotSquares.at(i).y - 1));
        } else if (move == "D") {
            plotSquares.push_back(cv::Point(plotSquares.at(i).x + 1, plotSquares.at(i).y));
        } else if (move == "DR") {
            plotSquares.push_back(cv::Point(plotSquares.at(i).x + 1, plotSquares.at(i).y + 1));
        } else if (move == "R"){
            plotSquares.push_back(cv::Point(plotSquares.at(i).x, plotSquares.at(i).y + 1));
        } else if (move == "UR"){
            plotSquares.push_back(cv::Point(plotSquares.at(i).x - 1, plotSquares.at(i).y + 1));
        } else if (move == "U"){
            plotSquares.push_back(cv::Point(plotSquares.at(i).x - 1, plotSquares.at(i).y));
        }
    }

    cv::Mat pMat;
    finalMat.copyTo(pMat);

    for (int i = 0; i < plotSquares.size(); i++){
        int j = 0, k = 0;
        j = plotSquares.at(i).x;
        k = plotSquares.at(i).y;

        if (j >= 0 && j <= vSize){
            if (k >= 0 && k <= hSize){
                grid[j][k].shadeSquare(finalMat, colour[0]);
            }
        }

    }

    addWeighted( finalMat, alpha, pMat, 1.0 - alpha, 0.0, finalMat);


}

cv::Point3i intersection = cv::Point3i(-1,-1,-1);
cv::Point selSquare = cv::Point(-1,-1);
void drawBlastTemplate(cv::Mat inMat, int type, int size, float alpha){

    cv::Point square;
    square.x = -1;
    square.y = -1;

    //squaresToColour.clear();

    if (type == SPHERE) {

        float dist = 1e9;
        cv::Point3i inter = cv::Point3i(-1,-1,-1);

        if (intersection.x == -1) {
            bool flag = false;

            for (int i = 0; i < vSize; i++) {
                for (int j = 0; j < hSize; j++) {
                    if (grid[i][j].contains(touchPoint)) {

                        for (int k = 0; k < 4; k++) {
                            float hold = distance(touchPoint, grid[i][j].corner[k]);

                            if (dist > hold) {
                                dist = hold;
                                inter = cv::Point3d(i, j, k);
                            }

                        }

                        flag = true;
                        break;
                    }
                }
                if (flag) {
                    break;
                }
            }

            intersection = cv::Point3i(inter.x, inter.y, inter.z);
        }

        inter.x = intersection.x;
        inter.y = intersection.y;
        inter.z = intersection.z;

        if (inter.x == -1){
            return;
        }

        cv::circle(inMat, grid[inter.x][inter.y].corner[inter.z], 5, cv::Scalar(255, 0, 0), -1);
        std::vector<cv::Point> startPos;

        if (inter.z == 0) {
            // Bottom Right
            startPos.push_back(cv::Point(inter.x - 1, inter.y - 1));
            startPos.push_back(cv::Point(inter.x - 1, inter.y));
            startPos.push_back(cv::Point(inter.x, inter.y));
            startPos.push_back(cv::Point(inter.x, inter.y - 1));
        } else if (inter.z == 1) {
            // Bottom Left
            startPos.push_back(cv::Point(inter.x - 1, inter.y));
            startPos.push_back(cv::Point(inter.x - 1, inter.y + 1));
            startPos.push_back(cv::Point(inter.x, inter.y + 1));
            startPos.push_back(cv::Point(inter.x, inter.y));
        } else if (inter.z == 2) {
            // Top Left
            startPos.push_back(cv::Point(inter.x, inter.y));
            startPos.push_back(cv::Point(inter.x, inter.y + 1));
            startPos.push_back(cv::Point(inter.x + 1, inter.y + 1));
            startPos.push_back(cv::Point(inter.x + 1, inter.y));
        } else if (inter.z == 3) {
            // Top Right
            startPos.push_back(cv::Point(inter.x, inter.y - 1));
            startPos.push_back(cv::Point(inter.x, inter.y));
            startPos.push_back(cv::Point(inter.x + 1, inter.y));
            startPos.push_back(cv::Point(inter.x + 1, inter.y - 1));
        }

        int radius = size / 5; // Convert from ft. to squares

        double countRadius = 0.0;

        int diagonal = 0;
        int holder = 0;

        cv::Point pos = startPos.at(1);
        std::vector<cv::Point> positions;

        while (floor(countRadius + 1.5) <= radius) {
            countRadius += 1.5;
            diagonal++;
        }


        // Upper Left
        pos = startPos.at(0);

        int count = 0;
        int remaining = radius;
        for (int i = 0; i < diagonal; i++){
            cv::Point current;
            current.x = pos.x - i;
            current.y = pos.y - i;

            positions.push_back(current);

            if ((count % 2) == 0){
                remaining -= 1;
            } else {
                remaining -= 2;
            }

            for (int j = 1; j <= remaining; j++){
                current.x -= 1;
                positions.push_back(current);
            }

            count++;
        }

        count = 0;
        remaining = radius;
        for (int i = 0; i < diagonal; i++){
            cv::Point current;
            current.x = pos.x - i;
            current.y = pos.y - i;

            positions.push_back(current);

            if ((count % 2) == 0){
                remaining -= 1;
            } else {
                remaining -= 2;
            }

            for (int j = 1; j <= remaining; j++){
                current.y -= 1;
                positions.push_back(current);
            }

            count++;
        }

        // Upper Right
        pos = startPos.at(1);

        count = 0;
        remaining = radius;
        for (int i = 0; i < diagonal; i++){
            cv::Point current;
            current.x = pos.x - i;
            current.y = pos.y + i;

            positions.push_back(current);

            if ((count % 2) == 0){
                remaining -= 1;
            } else {
                remaining -= 2;
            }

            for (int j = 1; j <= remaining; j++){
                current.x -= 1;
                positions.push_back(current);
            }

            count++;
        }

        count = 0;
        remaining = radius;
        for (int i = 0; i < diagonal; i++){
            cv::Point current;
            current.x = pos.x - i;
            current.y = pos.y + i;

            positions.push_back(current);

            if ((count % 2) == 0){
                remaining -= 1;
            } else {
                remaining -= 2;
            }

            for (int j = 1; j <= remaining; j++){
                current.y += 1;
                positions.push_back(current);
            }

            count++;
        }

        // Lower Right
        pos = startPos.at(2);

        count = 0;
        remaining = radius;
        for (int i = 0; i < diagonal; i++){
            cv::Point current;
            current.x = pos.x + i;
            current.y = pos.y + i;

            positions.push_back(current);

            if ((count % 2) == 0){
                remaining -= 1;
            } else {
                remaining -= 2;
            }

            for (int j = 1; j <= remaining; j++){
                current.y += 1;
                positions.push_back(current);
            }

            count++;
        }

        count = 0;
        remaining = radius;
        for (int i = 0; i < diagonal; i++){
            cv::Point current;
            current.x = pos.x + i;
            current.y = pos.y + i;

            positions.push_back(current);

            if ((count % 2) == 0){
                remaining -= 1;
            } else {
                remaining -= 2;
            }

            for (int j = 1; j <= remaining; j++){
                current.x += 1;
                positions.push_back(current);
            }

            count++;
        }


        // Lower Left

        pos = startPos.at(3);
        count = 0;
        remaining = radius;
        for (int i = 0; i < diagonal; i++){
            cv::Point current;
            current.x = pos.x + i;
            current.y = pos.y - i;

            positions.push_back(current);

            if ((count % 2) == 0){
                remaining -= 1;
            } else {
                remaining -= 2;
            }

            for (int j = 1; j <= remaining; j++){
                current.x += 1;
                positions.push_back(current);
            }

            count++;
        }

        count = 0;
        remaining = radius;
        for (int i = 0; i < diagonal; i++){
            cv::Point current;
            current.x = pos.x + i;
            current.y = pos.y - i;

            positions.push_back(current);

            if ((count % 2) == 0){
                remaining -= 1;
            } else {
                remaining -= 2;
            }

            for (int j = 1; j <= remaining; j++){
                current.y -= 1;
                positions.push_back(current);
            }

            count++;
        }





/*

        // Upper Right
        holder = diagonal - 1;
        while (diagonal >= 0) {
            cv::Point current;
            current.x = pos.x - diagonal;
            current.y = pos.y + diagonal;

            positions.push_back(current);

            countRadius = 1.5 * diagonal;

            while (countRadius < radius) {
                countRadius += 1.0;
                current.x -= 1;
                positions.push_back(current);
            }

            diagonal--;
        }

        diagonal = holder;
        while (diagonal >= 0) {
            cv::Point current;
            current.x = pos.x - diagonal;
            current.y = pos.y + diagonal;

            positions.push_back(current);

            countRadius = 1.5 * diagonal;

            while (countRadius < radius) {
                countRadius += 1.0;
                current.y += 1;
                positions.push_back(current);
            }

            diagonal--;
        }

        // Lower Right
        pos = startPos.at(2);
        diagonal = holder;
        while (diagonal >= 0) {
            cv::Point current;
            current.x = pos.x + diagonal;
            current.y = pos.y + diagonal;

            positions.push_back(current);

            countRadius = 1.5 * diagonal;

            while (countRadius < radius) {
                countRadius += 1.0;
                current.x += 1;
                positions.push_back(current);
            }

            diagonal--;
        }

        diagonal = holder;
        while (diagonal >= 0) {
            cv::Point current;
            current.x = pos.x + diagonal;
            current.y = pos.y + diagonal;


            positions.push_back(current);

            countRadius = 1.5 * diagonal;

            while (countRadius < radius) {
                countRadius += 1.0;
                current.y += 1;

                positions.push_back(current);
            }

            diagonal--;
        }


        // Lower Left
        pos = startPos.at(3);
        diagonal = holder;
        while (diagonal >= 0) {
            cv::Point current;
            current.x = pos.x + diagonal;
            current.y = pos.y - diagonal;

            positions.push_back(current);

            countRadius = 1.5 * diagonal;

            while (countRadius < radius) {
                countRadius += 1.0;
                current.x += 1;
                positions.push_back(current);
            }
            diagonal--;
        }

        diagonal = holder;
        while (diagonal >= 0) {
            cv::Point current;
            current.x = pos.x + diagonal;
            current.y = pos.y - diagonal;

            positions.push_back(current);

            countRadius = 1.5 * diagonal;

            while (countRadius < radius) {
                countRadius += 1.0;
                current.y -= 1;
                positions.push_back(current);
            }

            diagonal--;
        }

        // Upper Left
        pos = startPos.at(0);
        diagonal = holder;
        while (diagonal >= 0) {
            cv::Point current;
            current.x = pos.x - diagonal;
            current.y = pos.y - diagonal;

            positions.push_back(current);

            countRadius = 1.5 * diagonal;

            while (countRadius < radius) {
                countRadius += 1.0;
                current.x -= 1;
                positions.push_back(current);
            }

            diagonal--;
        }

        diagonal = holder;
        while (diagonal >= 0) {
            cv::Point current;
            current.x = pos.x - diagonal;
            current.y = pos.y - diagonal;

            positions.push_back(current);

            countRadius = 1.5 * diagonal;

            while (countRadius < radius) {
                countRadius += 1.0;
                current.y -= 1;
                positions.push_back(current);
            }

            diagonal--;
        }

*/


        cv::Mat hMat;
        inMat.copyTo(hMat);

        for (int i = 0; i < positions.size(); i++) {
            int j = 0, k = 0;
            j = positions.at(i).x;
            k = positions.at(i).y;

            if (j >= 0 && j <= vSize) {
                if (k >= 0 && k <= hSize) {
                    grid[j][k].shadeSquare(inMat, cv::Scalar(255, 140, 0));
                }
            }
        }

        addWeighted(inMat, alpha, hMat, 1.0 - alpha, 0.0, inMat);

    } else if (type == CONE){



        if (selectedID == -1){
            return;
        }

        Token selToken;
        bool found = false;

        for (int i = 0; i < tokenVec.size(); i++){
            if (tokenVec.at(i).id == selectedID){
                selToken = tokenVec.at(i);
                found = true;
            }
        }

        if (!found){
            return;
        }

        if (selSquare.x == -1) {
            bool flag = false;

            for (int i = 0; i < vSize; i++) {
                for (int j = 0; j < hSize; j++) {
                    if (grid[i][j].contains(touchPoint)) {

                        selSquare.x = i;
                        selSquare.y = j;

                        flag = true;
                        break;
                    }
                }
                if (flag) {
                    break;
                }
            }
        }

        std::vector<cv::Point> startPos;

        // Up Left
        startPos.push_back(cv::Point(selToken.location.x - 3, selToken.location.y - 3));

        // Up Up Left
        startPos.push_back(cv::Point(selToken.location.x - 3, selToken.location.y - 1));
        // Up Up Right
        startPos.push_back(cv::Point(selToken.location.x - 3, selToken.location.y + 1));

        // Up Right
        startPos.push_back(cv::Point(selToken.location.x - 3, selToken.location.y + 3));

        // Right Right Up
        startPos.push_back(cv::Point(selToken.location.x - 1, selToken.location.y + 3));
        // Right Right Down
        startPos.push_back(cv::Point(selToken.location.x + 1, selToken.location.y + 3));

        // Down Right
        startPos.push_back(cv::Point(selToken.location.x + 3, selToken.location.y + 3));

        // Down Down Right
        startPos.push_back(cv::Point(selToken.location.x + 3, selToken.location.y + 1));
        // Down Down Left
        startPos.push_back(cv::Point(selToken.location.x + 3, selToken.location.y - 1));

        // Down Left
        startPos.push_back(cv::Point(selToken.location.x + 3, selToken.location.y - 3));

        // Left Left Down
        startPos.push_back(cv::Point(selToken.location.x + 1, selToken.location.y - 3));
        // Left Left Up
        startPos.push_back(cv::Point(selToken.location.x - 1, selToken.location.y - 3));

        float dist = 1e9;
        int startDir = -1;

        for (int i = 0; i < startPos.size(); i++){
            float holder = distance(startPos.at(i), selSquare);
            if (holder < dist) {
                dist = holder;
                startDir = i;
            }
        }

        cv::Point startSquare = cv::Point(selToken.location.x, selToken.location.y);

        int radius = size / 5; // Convert from ft. to squares

        int countRadius = 0;
        int diagonal = 0;
        int holder = 0;
        cv::Point pos;
        std::vector<cv::Point> positions;

        int counter = 0;
        int add = 0;
        while (true) {
            if ((counter % 2) == 0){
                add = 1;
            } else {
                add = 2;
            }
            if ((countRadius + add) > radius){
                break;
            } else {
                countRadius += add;
                diagonal++;
                counter++;
            }
        }

        switch (startDir) {
            case 0: { // Upper Left
                pos = cv::Point(selToken.location.x - 1, selToken.location.y - 1);

                int count = 0;
                int remaining = radius;
                for (int i = 0; i < diagonal; i++){
                    cv::Point current;
                    current.x = pos.x - i;
                    current.y = pos.y - i;

                    positions.push_back(current);

                    if ((count % 2) == 0){
                        remaining -= 1;
                    } else {
                        remaining -= 2;
                    }

                    for (int j = 1; j <= remaining; j++){
                        current.x -= 1;
                        positions.push_back(current);
                    }

                    count++;
                }

                count = 0;
                remaining = radius;
                for (int i = 0; i < diagonal; i++){
                    cv::Point current;
                    current.x = pos.x - i;
                    current.y = pos.y - i;

                    positions.push_back(current);

                    if ((count % 2) == 0){
                        remaining -= 1;
                    } else {
                        remaining -= 2;
                    }

                    for (int j = 1; j <= remaining; j++){
                        current.y -= 1;
                        positions.push_back(current);
                    }

                    count++;
                }

                break;
            }
                ///////////////////////////////////
            case 1: { // Upper MLeft
                pos = cv::Point(selToken.location.x - 1, selToken.location.y - 1);

                int count = 0;
                int remaining = radius;
                for (int i = 0; i < diagonal; i++){
                    cv::Point current;
                    current.x = pos.x - i;
                    current.y = pos.y - i;

                    positions.push_back(current);

                    if ((count % 2) == 0){
                        remaining -= 1;
                    } else {
                        remaining -= 2;
                    }

                    for (int j = 1; j <= remaining; j++){
                        current.x -= 1;
                        positions.push_back(current);
                    }

                    count++;
                }

                pos = cv::Point(selToken.location.x - 1, selToken.location.y);

                count = 0;
                remaining = radius;
                for (int i = 0; i < diagonal; i++){
                    cv::Point current;
                    current.x = pos.x - i;
                    current.y = pos.y + i;

                    positions.push_back(current);

                    if ((count % 2) == 0){
                        remaining -= 1;
                    } else {
                        remaining -= 2;
                    }

                    for (int j = 1; j <= remaining; j++){
                        current.x -= 1;
                        positions.push_back(current);
                    }

                    count++;
                }

                break;
            }
            case 2: { // Upper MRight
                pos = cv::Point(selToken.location.x - 1, selToken.location.y);

                int count = 0;
                int remaining = radius;
                for (int i = 0; i < diagonal; i++){
                    cv::Point current;
                    current.x = pos.x - i;
                    current.y = pos.y - i;

                    positions.push_back(current);

                    if ((count % 2) == 0){
                        remaining -= 1;
                    } else {
                        remaining -= 2;
                    }

                    for (int j = 1; j <= remaining; j++){
                        current.x -= 1;
                        positions.push_back(current);
                    }

                    count++;
                }

                pos = cv::Point(selToken.location.x - 1, selToken.location.y + 1);

                count = 0;
                remaining = radius;
                for (int i = 0; i < diagonal; i++){
                    cv::Point current;
                    current.x = pos.x - i;
                    current.y = pos.y + i;

                    positions.push_back(current);

                    if ((count % 2) == 0){
                        remaining -= 1;
                    } else {
                        remaining -= 2;
                    }

                    for (int j = 1; j <= remaining; j++){
                        current.x -= 1;
                        positions.push_back(current);
                    }

                    count++;
                }

                break;
            }
            case 3: { // Upper Right
                pos = cv::Point(selToken.location.x - 1, selToken.location.y + 1);

                int count = 0;
                int remaining = radius;
                for (int i = 0; i < diagonal; i++){
                    cv::Point current;
                    current.x = pos.x - i;
                    current.y = pos.y + i;

                    positions.push_back(current);

                    if ((count % 2) == 0){
                        remaining -= 1;
                    } else {
                        remaining -= 2;
                    }

                    for (int j = 1; j <= remaining; j++){
                        current.x -= 1;
                        positions.push_back(current);
                    }

                    count++;
                }

                count = 0;
                remaining = radius;
                for (int i = 0; i < diagonal; i++){
                    cv::Point current;
                    current.x = pos.x - i;
                    current.y = pos.y + i;

                    positions.push_back(current);

                    if ((count % 2) == 0){
                        remaining -= 1;
                    } else {
                        remaining -= 2;
                    }

                    for (int j = 1; j <= remaining; j++){
                        current.y += 1;
                        positions.push_back(current);
                    }

                    count++;
                }

                break;
            }
            case 4: { // Right MUp
                pos = cv::Point(selToken.location.x -1, selToken.location.y + 1);

                int count = 0;
                int remaining = radius;
                for (int i = 0; i < diagonal; i++){
                    cv::Point current;
                    current.x = pos.x - i;
                    current.y = pos.y + i;

                    positions.push_back(current);

                    if ((count % 2) == 0){
                        remaining -= 1;
                    } else {
                        remaining -= 2;
                    }

                    for (int j = 1; j <= remaining; j++){
                        current.y += 1;
                        positions.push_back(current);
                    }

                    count++;
                }

                pos = cv::Point(selToken.location.x, selToken.location.y + 1);
                count = 0;
                remaining = radius;
                for (int i = 0; i < diagonal; i++){
                    cv::Point current;
                    current.x = pos.x + i;
                    current.y = pos.y + i;

                    positions.push_back(current);

                    if ((count % 2) == 0){
                        remaining -= 1;
                    } else {
                        remaining -= 2;
                    }

                    for (int j = 1; j <= remaining; j++){
                        current.y += 1;
                        positions.push_back(current);
                    }

                    count++;
                }

                break;
            }
            case 5: { // Right MDown
                pos = cv::Point(selToken.location.x, selToken.location.y + 1);

                int count = 0;
                int remaining = radius;
                for (int i = 0; i < diagonal; i++){
                    cv::Point current;
                    current.x = pos.x - i;
                    current.y = pos.y + i;

                    positions.push_back(current);

                    if ((count % 2) == 0){
                        remaining -= 1;
                    } else {
                        remaining -= 2;
                    }

                    for (int j = 1; j <= remaining; j++){
                        current.y += 1;
                        positions.push_back(current);
                    }

                    count++;
                }

                pos = cv::Point(selToken.location.x + 1, selToken.location.y + 1);
                count = 0;
                remaining = radius;
                for (int i = 0; i < diagonal; i++){
                    cv::Point current;
                    current.x = pos.x + i;
                    current.y = pos.y + i;

                    positions.push_back(current);

                    if ((count % 2) == 0){
                        remaining -= 1;
                    } else {
                        remaining -= 2;
                    }

                    for (int j = 1; j <= remaining; j++){
                        current.y += 1;
                        positions.push_back(current);
                    }

                    count++;
                }

                break;
            }
            case 6: { // Lower Right
                pos = cv::Point(selToken.location.x + 1, selToken.location.y + 1);

                int count = 0;
                int remaining = radius;
                for (int i = 0; i < diagonal; i++){
                    cv::Point current;
                    current.x = pos.x + i;
                    current.y = pos.y + i;

                    positions.push_back(current);

                    if ((count % 2) == 0){
                        remaining -= 1;
                    } else {
                        remaining -= 2;
                    }

                    for (int j = 1; j <= remaining; j++){
                        current.y += 1;
                        positions.push_back(current);
                    }

                    count++;
                }

                count = 0;
                remaining = radius;
                for (int i = 0; i < diagonal; i++){
                    cv::Point current;
                    current.x = pos.x + i;
                    current.y = pos.y + i;

                    positions.push_back(current);

                    if ((count % 2) == 0){
                        remaining -= 1;
                    } else {
                        remaining -= 2;
                    }

                    for (int j = 1; j <= remaining; j++){
                        current.x += 1;
                        positions.push_back(current);
                    }

                    count++;
                }

                break;
            }
            case 7: { // Down MRight
                pos = cv::Point(selToken.location.x + 1, selToken.location.y + 1);

                int count = 0;
                int remaining = radius;
                for (int i = 0; i < diagonal; i++){
                    cv::Point current;
                    current.x = pos.x + i;
                    current.y = pos.y + i;

                    positions.push_back(current);

                    if ((count % 2) == 0){
                        remaining -= 1;
                    } else {
                        remaining -= 2;
                    }

                    for (int j = 1; j <= remaining; j++){
                        current.x += 1;
                        positions.push_back(current);
                    }

                    count++;
                }

                pos = cv::Point(selToken.location.x + 1, selToken.location.y);
                count = 0;
                remaining = radius;
                for (int i = 0; i < diagonal; i++){
                    cv::Point current;
                    current.x = pos.x + i;
                    current.y = pos.y - i;

                    positions.push_back(current);

                    if ((count % 2) == 0){
                        remaining -= 1;
                    } else {
                        remaining -= 2;
                    }

                    for (int j = 1; j <= remaining; j++){
                        current.x += 1;
                        positions.push_back(current);
                    }

                    count++;
                }

                break;
            }
            case 8: { // Down MLeft
                pos = cv::Point(selToken.location.x + 1, selToken.location.y);

                int count = 0;
                int remaining = radius;
                for (int i = 0; i < diagonal; i++){
                    cv::Point current;
                    current.x = pos.x + i;
                    current.y = pos.y + i;

                    positions.push_back(current);

                    if ((count % 2) == 0){
                        remaining -= 1;
                    } else {
                        remaining -= 2;
                    }

                    for (int j = 1; j <= remaining; j++){
                        current.x += 1;
                        positions.push_back(current);
                    }

                    count++;
                }

                pos = cv::Point(selToken.location.x + 1, selToken.location.y - 1);
                count = 0;
                remaining = radius;
                for (int i = 0; i < diagonal; i++){
                    cv::Point current;
                    current.x = pos.x + i;
                    current.y = pos.y - i;

                    positions.push_back(current);

                    if ((count % 2) == 0){
                        remaining -= 1;
                    } else {
                        remaining -= 2;
                    }

                    for (int j = 1; j <= remaining; j++){
                        current.x += 1;
                        positions.push_back(current);
                    }

                    count++;
                }

                break;
            }
            case 9: { // Lower Left
                pos = cv::Point(selToken.location.x + 1, selToken.location.y - 1);

                int count = 0;
                int remaining = radius;
                for (int i = 0; i < diagonal; i++){
                    cv::Point current;
                    current.x = pos.x + i;
                    current.y = pos.y - i;

                    positions.push_back(current);

                    if ((count % 2) == 0){
                        remaining -= 1;
                    } else {
                        remaining -= 2;
                    }

                    for (int j = 1; j <= remaining; j++){
                        current.x += 1;
                        positions.push_back(current);
                    }

                    count++;
                }

                count = 0;
                remaining = radius;
                for (int i = 0; i < diagonal; i++){
                    cv::Point current;
                    current.x = pos.x + i;
                    current.y = pos.y - i;

                    positions.push_back(current);

                    if ((count % 2) == 0){
                        remaining -= 1;
                    } else {
                        remaining -= 2;
                    }

                    for (int j = 1; j <= remaining; j++){
                        current.y -= 1;
                        positions.push_back(current);
                    }

                    count++;
                }

                break;
            }
            case 10: { // Left MDown
                pos = cv::Point(selToken.location.x + 1, selToken.location.y - 1);

                int count = 0;
                int remaining = radius;
                for (int i = 0; i < diagonal; i++){
                    cv::Point current;
                    current.x = pos.x + i;
                    current.y = pos.y - i;

                    positions.push_back(current);

                    if ((count % 2) == 0){
                        remaining -= 1;
                    } else {
                        remaining -= 2;
                    }

                    for (int j = 1; j <= remaining; j++){
                        current.y -= 1;
                        positions.push_back(current);
                    }

                    count++;
                }

                pos = cv::Point(selToken.location.x, selToken.location.y - 1);
                count = 0;
                remaining = radius;
                for (int i = 0; i < diagonal; i++){
                    cv::Point current;
                    current.x = pos.x - i;
                    current.y = pos.y - i;

                    positions.push_back(current);

                    if ((count % 2) == 0){
                        remaining -= 1;
                    } else {
                        remaining -= 2;
                    }

                    for (int j = 1; j <= remaining; j++){
                        current.y -= 1;
                        positions.push_back(current);
                    }

                    count++;
                }

                break;
            }
            case 11: { // Left MUp
                pos = cv::Point(selToken.location.x, selToken.location.y - 1);

                int count = 0;
                int remaining = radius;
                for (int i = 0; i < diagonal; i++){
                    cv::Point current;
                    current.x = pos.x + i;
                    current.y = pos.y - i;

                    positions.push_back(current);

                    if ((count % 2) == 0){
                        remaining -= 1;
                    } else {
                        remaining -= 2;
                    }

                    for (int j = 1; j <= remaining; j++){
                        current.y -= 1;
                        positions.push_back(current);
                    }

                    count++;
                }

                pos = cv::Point(selToken.location.x - 1, selToken.location.y - 1);
                count = 0;
                remaining = radius;
                for (int i = 0; i < diagonal; i++){
                    cv::Point current;
                    current.x = pos.x - i;
                    current.y = pos.y - i;

                    positions.push_back(current);

                    if ((count % 2) == 0){
                        remaining -= 1;
                    } else {
                        remaining -= 2;
                    }

                    for (int j = 1; j <= remaining; j++){
                        current.y -= 1;
                        positions.push_back(current);
                    }

                    count++;
                }

                break;
            }
        }

        cv::Mat hMat;
        inMat.copyTo(hMat);

        for (int i = 0; i < positions.size(); i++) {
            int j = 0, k = 0;
            j = positions.at(i).x;
            k = positions.at(i).y;

            if (j >= 0 && j <= vSize) {
                if (k >= 0 && k <= hSize) {
                    grid[j][k].shadeSquare(inMat, cv::Scalar(255, 140, 0));
                }
            }
        }

        addWeighted(inMat, alpha, hMat, 1.0 - alpha, 0.0, inMat);

    } else if (type == LINE){


        if (selectedID == -1){
            return;
        }

        Token selToken;
        bool found = false;

        for (int i = 0; i < tokenVec.size(); i++){
            if (tokenVec.at(i).id == selectedID){
                selToken = tokenVec.at(i);
                found = true;
            }
        }

        if (!found){
            return;
        }

        if (selSquare.x == -1) {
            bool flag = false;

            for (int i = 0; i < vSize; i++) {
                for (int j = 0; j < hSize; j++) {
                    if (grid[i][j].contains(touchPoint)) {

                        selSquare.x = i;
                        selSquare.y = j;

                        flag = true;
                        break;
                    }
                }
                if (flag) {
                    break;
                }
            }
        }

        double xDist = selSquare.x - selToken.location.x;
        double yDist = selSquare.y - selToken.location.y;

        double stepX;
        if (yDist < -1e3){
            stepX = 0.0;
        } else {
            stepX = xDist /(double) (100);
        }
        double stepY;
        if (yDist < -1e3){
            stepY = 0.0;
        } else {
            stepY = yDist /(double)  (100);
        }

        cv::Point2d pos = selToken.location;
        cv::Point2d holder;
        std::vector<cv::Point> positions;
        int radius = size / 5;

        while (fabs(distance(selToken.location, pos)) <= radius){
            pos.x += stepX;
            pos.y += stepY;
            if (positions.size() > 0){
                if (positions.at(positions.size() - 1).x != floor(pos.x) || positions.at(positions.size() - 1).y != floor(pos.y)){

                    if (stepX < 0) {
                        holder.x = ceil(pos.x);
                    } else {
                        holder.x = floor(pos.x);
                    }

                    if (stepY < 0) {
                        holder.y = ceil(pos.y);
                    } else {
                        holder.y = floor(pos.y);
                    }

                    if (holder.x != selToken.location.x || holder.y != selToken.location.y){
                        positions.push_back(cv::Point(holder.x, holder.y));
                    }
                }
            } else {
                if (stepX < 0) {
                    holder.x = ceil(pos.x);
                } else {
                    holder.x = floor(pos.x);
                }

                if (stepY < 0) {
                    holder.y = ceil(pos.y);
                } else {
                    holder.y = floor(pos.y);
                }

                if (holder.x != selToken.location.x || holder.y != selToken.location.y){
                    positions.push_back(cv::Point(holder.x, holder.y));
                }
            }
        }

        cv::Mat hMat;
        inMat.copyTo(hMat);

        for (int i = 0; i < positions.size(); i++) {
            int j = 0, k = 0;
            j = positions.at(i).x;
            k = positions.at(i).y;

            if (j >= 0 && j <= vSize) {
                if (k >= 0 && k <= hSize) {
                    grid[j][k].shadeSquare(inMat, cv::Scalar(255, 140, 0));
                }
            }
        }

        addWeighted(inMat, alpha, hMat, 1.0 - alpha, 0.0, inMat);


    }

}

cv::Mat four_point_transform(cv::Mat image, std::vector<cv::Point2f> gridCorners){

    //gridCorners = gridCorners;

    float widthA = sqrt(pow((gridCorners.at(2).x - gridCorners.at(3).x), 2) + pow((gridCorners.at(2).y - gridCorners.at(3).y), 2));
    float widthB = sqrt(pow((gridCorners.at(1).x - gridCorners.at(0).x), 2) + pow((gridCorners.at(1).y - gridCorners.at(0).y), 2));
    float maxW = 0.0;

    if (widthA > widthB){
        maxW = widthA;
    } else {
        maxW = widthB;
    }

    float heightA = sqrt(pow((gridCorners.at(1).x - gridCorners.at(2).x), 2) + pow((gridCorners.at(1).y - gridCorners.at(2).y), 2));
    float heightB = sqrt(pow((gridCorners.at(0).x - gridCorners.at(3).x), 2) + pow((gridCorners.at(0).y - gridCorners.at(3).y), 2));
    float maxH = 0.0;

    if (heightA > heightB){
        maxH = heightA;
    } else {
        maxH = heightB;
    }


    cv::Point2f rect[4] = {gridCorners.at(0), gridCorners.at(1), gridCorners.at(2), gridCorners.at(3)};
    cv::Point2f dst[4] = {cv::Point2f(0,0), cv::Point2f(maxW - 1,0), cv::Point2f(maxW - 1, maxH - 1), cv::Point2f(0,maxH - 1)};

    cv::Mat M = cv::getPerspectiveTransform(rect, dst);

    cv::Mat dstM;
    cv::warpPerspective(image, dstM, M, cv::Size(maxW, maxH));

    std::vector<cv::Point2f> nCorners;
    nCorners.push_back(dst[0]);
    nCorners.push_back(dst[1]);
    nCorners.push_back(dst[2]);
    nCorners.push_back(dst[3]);

    computeGrid(dstM, nCorners, hSize, vSize );

    return dstM;
}

cv::Mat M;
extern "C"
void
Java_com_example_raven_pathfindar_MainActivity_detectMarkers(JNIEnv *env, jobject instance, jlong image_final, jlong image_editable) {

    cv::Mat& finalMat = *(cv::Mat*)image_final;
    cv::Mat& inMat = *(cv::Mat*)image_editable;

    cv::resize(inMat, inMat, cv::Size(720, 480), 0, 0, cv::INTER_CUBIC);
    cv::resize(finalMat, finalMat, cv::Size(720, 480), 0, 0, cv::INTER_CUBIC);

    cv::Mat partMat;
    finalMat.copyTo(partMat);


    std::vector<int> ids;
    std::vector<std::vector<cv::Point2f> > corners;

    cv::cvtColor(inMat,inMat,CV_RGB2GRAY);
    cv::GaussianBlur(inMat, inMat, cv::Size(3, 3), 1.5, 1.5);


    cv::Ptr<cv::aruco::Dictionary> dict = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    cv::aruco::detectMarkers(inMat, cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250), corners, ids);


    std::vector<cv::Point2f> gridCorners;

    if (ids.size() == 4) {
        start = true;
    }

    if (ids.size() == 0){
        start = false;
    }

    if (start == true){
        // if at least one marker detected
        if (ids.size() > 0) {
            gridCorners = findCorners(corners, ids);
        }

        computeGrid(finalMat, gridCorners, hSize, vSize);

        std::vector<cv::Point2f> ngC; // New Grid Corners (from markers on ground)
        ngC.push_back(grid[0][0].corner[2]);
        ngC.push_back(grid[0][hSize - 1].corner[3]);
        ngC.push_back(grid[vSize - 1][hSize - 1].corner[0]);
        ngC.push_back(grid[vSize - 1][0].corner[1]);

        cv::Mat dst = four_point_transform(finalMat, ngC);

        std::vector<cv::Point2f> sgC; // Straight Grid Corners (perfectly straight grid)

        sgC.push_back(grid[0][0].corner[2]);
        sgC.push_back(grid[0][hSize - 1].corner[3]);
        sgC.push_back(grid[vSize - 1][hSize - 1].corner[0]);
        sgC.push_back(grid[vSize - 1][0].corner[1]);

        cv::Mat H = cv::findHomography(sgC, ngC);

        cv::Mat warped;
        cv::warpPerspective(dst,warped,H,finalMat.size());

        // Project the points properly now
        std::vector<cv::Point2f> sX, rX; // start x, result x
        for (int i = 0; i <= vSize; i++){ // queue each point from the horizontal lines
            sX.push_back(hLines[i][0]);
            sX.push_back(hLines[i][1]);
        }

        // Transform them with the H matrix
        cv::perspectiveTransform(sX, rX, H);

        int j = 0;
        for (int i = 0; i <= vSize; i++){ // reset them
            hLines[i][0] = rX.at(j);
            j++;
            hLines[i][1] = rX.at(j);
            j++;
        }


        std::vector<cv::Point2f> sY, rY; // Start y, result y
        for (int i = 0; i <= hSize; i++){
            sY.push_back(vLines[i][0]);
            sY.push_back(vLines[i][1]);
        }

        cv::perspectiveTransform(sY, rY, H);

        j = 0;
        for (int i = 0; i <= hSize; i++){
            vLines[i][0] = rY.at(j);
            j++;
            vLines[i][1] = rY.at(j);
            j++;
        }

        computeGrid(finalMat, gridCorners, hSize, vSize);

        findTokens(partMat, finalMat);

        if (gridType == MOVEMENT) {
            if (selectedID != -1) {
                colourRange(selectedID, 0.5, finalMat);
            }
            drawTokens(finalMat); // No transparency for tokens. Doesn't work well, becomes muddied.
        } else if (gridType == PLOTTING){
            if (selectedID != -1) {
                colourPlot(selectedID, 0.5, finalMat);
            }
            drawTokens(finalMat); // No transparency for tokens. Doesn't work well, becomes muddied.
        } else if (gridType == ATTACK) {
            drawTokens(finalMat); // Draw overtop of tokens to show which ones are in range
            if (selectedID != -1){
                colourRange(selectedID, 0.5, finalMat);
            }
        };

        if (touchPoint.x != -1 && selectedBlast != -1){
            //cv::circle(finalMat, touchPoint, 5, cv::Scalar(255, 0, 0), -1);
            drawBlastTemplate(finalMat, selectedBlast, blastRadius, 0.5);
        }

        drawGrid(finalMat, hSize, vSize);

    }

    cv::resize(inMat, inMat, cv::Size(1280, 720), 0, 0, cv::INTER_CUBIC);
    cv::resize(finalMat, finalMat, cv::Size(1280, 720), 0, 0, cv::INTER_CUBIC);

}

extern "C"
void
Java_com_example_raven_pathfindar_MainActivity_adjustGridDimensions(JNIEnv *env, jobject instance, jint x, jint y) {
    vSize = y;
    hSize = x;

    init();
}

extern "C"
void
Java_com_example_raven_pathfindar_MainActivity_setTouchPos(JNIEnv *env, jobject instance, jint x, jint y) {
    touchPoint.y = y;
    touchPoint.x = x;

    intersection.x = -1;
    intersection.y = -1;
    selSquare.x = -1;
    selSquare.y = -1;
}

extern "C"
void
Java_com_example_raven_pathfindar_MainActivity_setSelectedToken(JNIEnv *env, jobject instance, jint tokenID) {
    selectedID = tokenID;

    for (int i = 0; i < tokenVec.size(); i++){
        if (tokenVec.at(i).id == selectedID){
            mRemain = tokenVec.at(i).mRange;
        }
    }

}

extern "C"
void
Java_com_example_raven_pathfindar_MainActivity_setSelectedWeapon(JNIEnv *env, jobject instance, jint weaponID) {
    selectedWeapon = weaponID;
}

extern "C"
void
Java_com_example_raven_pathfindar_MainActivity_setBlastTemplate(JNIEnv *env, jobject instance, jint blastID, jint radius) {

    selectedBlast = blastID;
    blastRadius = radius;

}

extern "C"
void
Java_com_example_raven_pathfindar_MainActivity_setDisplayType(JNIEnv *env, jobject instance, jint type) {
    gridType = type;
}

void split(const std::string &str, char split, std::vector<std::string> &elements) {
    std::stringstream ss;
    ss.str(str);
    std::string item;
    while (std::getline(ss, item, split)) {
        elements.push_back(item);
    }
}

extern "C"
void
Java_com_example_raven_pathfindar_MainActivity_setTokenList(JNIEnv *env, jobject instance, jstring inString) {

    const jsize len = env->GetStringUTFLength(inString);
    const char* strChars = env->GetStringUTFChars(inString, (jboolean *)0);

    std::string procString(strChars, len);

    std::vector<std::string> tokens;

    split(procString, '|', tokens);

    for (int i = 0; i < tokens.size(); i++){
        std::vector<std::string> tokenPieces;
        split(tokens.at(i), '@', tokenPieces);

        Token nTok;

        nTok.id = std::atoi(tokenPieces.at(0).c_str());
        nTok.name = tokenPieces.at(1);
        nTok.location.x = std::atoi(tokenPieces.at(2).c_str());
        nTok.location.y = std::atoi(tokenPieces.at(3).c_str());
        nTok.mRange = std::atoi(tokenPieces.at(4).c_str());
        nTok.lifespan = std::atoi(tokenPieces.at(5).c_str());

        if (tokenPieces.at(6) == "true") {
            nTok.found = true;
        } else {
            nTok.found = false;
        }

        std::vector<std::string> colourPieces;
        split(tokenPieces.at(7), ',', colourPieces);

        cv::Scalar colour = cv::Scalar(std::atoi(colourPieces.at(0).c_str()), std::atoi(colourPieces.at(1).c_str()), std::atoi(colourPieces.at(2).c_str()));
        nTok.colour = colour;

        nTok.w1.name = tokenPieces.at(8);
        nTok.w1.range = std::atoi(tokenPieces.at(9).c_str());

        if (tokenPieces.at(10) == "true") {
            nTok.w1.reach = true;
        } else {
            nTok.w1.reach = false;
        }

        if (tokenPieces.at(11) == "true") {
            nTok.w1.ranged = true;
        } else {
            nTok.w1.ranged = false;
        }

        nTok.w2.name = tokenPieces.at(12);
        nTok.w2.range = std::atoi(tokenPieces.at(13).c_str());

        if (tokenPieces.at(14) == "true") {
            nTok.w2.reach = true;
        } else {
            nTok.w2.reach = false;
        }

        if (tokenPieces.at(15) == "true") {
            nTok.w2.ranged = true;
        } else {
            nTok.w2.ranged = false;
        }

        nTok.w3.name = tokenPieces.at(16);
        nTok.w3.range = std::atoi(tokenPieces.at(17).c_str());

        if (tokenPieces.at(18) == "true") {
            nTok.w3.reach = true;
        } else {
            nTok.w3.reach = false;
        }

        if (tokenPieces.at(19) == "true") {
            nTok.w3.ranged = true;
        } else {
            nTok.w3.ranged = false;
        }

        nTok.w4.name = tokenPieces.at(20);
        nTok.w4.range = std::atoi(tokenPieces.at(21).c_str());

        if (tokenPieces.at(22) == "true") {
            nTok.w4.reach = true;
        } else {
            nTok.w4.reach = false;
        }

        if (tokenPieces.at(23) == "true") {
            nTok.w4.ranged = true;
        } else {
            nTok.w4.ranged = false;
        }

        for (int j = 0; j < tokenVec.size(); j++){
            if (tokenVec.at(j).id == nTok.id){
                tokenVec.at(j) = nTok; // test this
            }
        }

    }




}

extern "C"
JNIEXPORT jstring JNICALL Java_com_example_raven_pathfindar_MainActivity_getTokenList(JNIEnv *env, jobject obj) {
    std::string out = "";

    for (int i = 0; i < tokenVec.size(); i++){
        out = out.append(tokenVec.at(i).toString());
    }

    jstring result;

    puts(out.c_str());
    result = (*env).NewStringUTF(out.c_str());
    return result;
}

void calcMRemain(){
    int diagonal = 0;

    bool found = false;

    for (int i = 0; i < tokenVec.size(); i++){
        if (tokenVec.at(i).id == selectedID){
            mRemain = tokenVec.at(i).mRange;
            found = true;
        }
    }

    if (!found){
        return;
    }

    for (int i = 0; i < movementPieces.size(); i++){
        if (movementPieces.at(i) == "UL" || movementPieces.at(i) == "UR" || movementPieces.at(i) == "DL" || movementPieces.at(i) == "DR"){
            diagonal++;
            if (diagonal % 2 == 1){
                mRemain -= 5;
            } else {
                mRemain -= 10;
            }
        } else {
            mRemain -= 5;
        }
    }
}

extern "C"
void
Java_com_example_raven_pathfindar_MainActivity_setPlottingPath(JNIEnv *env, jobject instance, jstring inString) {
    const jsize len = env->GetStringUTFLength(inString);
    const char* strChars = env->GetStringUTFChars(inString, (jboolean *)0);

    std::string procString(strChars, len);

    movementPieces.clear();

    split(procString, '-', movementPieces);
    calcMRemain();
}

extern "C"
JNIEXPORT jint JNICALL Java_com_example_raven_pathfindar_MainActivity_getMovementRemain(JNIEnv *env, jobject obj) {
    return mRemain;
}

extern "C"
void
Java_com_example_raven_pathfindar_MainActivity_init(JNIEnv *env, jobject instance) {
    init();
}
