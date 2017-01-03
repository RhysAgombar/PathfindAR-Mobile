#include <jni.h>
#include <string>
#include <opencv2/aruco.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d.hpp>
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

std::string to_stringB(bool value)
{
    if (value) {
        return "1";
    } else {
        return "0";
    }

}

int hSize = 3;
int vSize = 3;
int** gridCount; //gridCount[11][11];
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
    // add  shading functions
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
    int mRange = 0;
    //int mRemain;
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

    for (int i = 0; i < contour.size() - 1; i++) {
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

        if (uTokens.size() > 5) {

            int k = 0;
            k++;

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

            if (tokenVec.at(i).lifespan > 100) {
                tokenVec.at(i).lifespan = 100;
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

                    std::vector<cv::Point> testVec = grid[j][k].getPointsArray();

                    cv::Point polyPoints[1][4];
                    polyPoints[0][0] = grid[j][k].corner[0];
                    polyPoints[0][1] = grid[j][k].corner[1];
                    polyPoints[0][2] = grid[j][k].corner[2];
                    polyPoints[0][3] = grid[j][k].corner[3];

                    const cv::Point* ppt[1] = { polyPoints[0] };
                    int npt[] = { 4 };

                    cv::fillPoly(imageOut, ppt, npt, 1, cv::Scalar(255, 255, 255));

                    gridCount[j][k] = 0;

                    if (!(find(uTokens.begin(), uTokens.end(), cv::Point(j, k)) != uTokens.end())) {
                        uTokens.push_back(cv::Point(j, k));
                    }

                }

            }
        }

        cv::circle(imageOut, cent, 5, cv::Scalar(255, 0, 255), -1);

    }

    bool pauseTracking = false;
    float squareAverage = 0;

    idTokens();

    uTokens.clear();

    pauseTracking = false;

    for (int i = 0; i < tokenVec.size(); i++) {

        int j = tokenVec.at(i).location.x;
        int k = tokenVec.at(i).location.y;

        cv::Point polyPoints[1][4];
        polyPoints[0][0] = grid[j][k].corner[0];
        polyPoints[0][1] = grid[j][k].corner[1];
        polyPoints[0][2] = grid[j][k].corner[2];
        polyPoints[0][3] = grid[j][k].corner[3];

        const cv::Point* ppt[1] = { polyPoints[0] };
        int npt[] = { 4 };

        cv::fillPoly(imageOut, ppt, npt, 1, tokenVec.at(i).colour);
    }

    //cv::imshow("Contour Centers", drawing);

}

void drawGrid(cv::Mat imageCopy, std::vector<cv::Point2f> gridCorners, int horiz, int vert) {
    cv::Mat dirVec[4]; // top left -> top right, bottom left -> bottom right, top left -> bottom left, top right -> bottom right
    float hDist, vDist, dist;


    if (gridCorners.size() >= 3) {

        for (int i = 0; i <= vert; i++) {
            float distx;
            float disty;

            distx = gridCorners.at(3).x - gridCorners.at(0).x;
            disty = gridCorners.at(3).y - gridCorners.at(0).y;

            hLines[i][0] = cv::Point(gridCorners.at(0).x + (distx * (i / (float)vert)), gridCorners.at(0).y + (disty) * (float)(i / (float)vert));

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

        for (int i = 0; i <= vert; i++) {
            cv::line(imageCopy, hLines[i][0], hLines[i][1], cv::Scalar(255, 255, 255));
        }

        for (int i = 0; i <= horiz; i++) {
            cv::line(imageCopy, vLines[i][0], vLines[i][1], cv::Scalar(255, 255, 255));
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

extern "C"
void
Java_com_example_raven_pathfindar_MainActivity_detectMarkers(JNIEnv *env, jobject instance, jlong image_final, jlong image_editable) {

    cv::Mat& finalMat = *(cv::Mat*)image_final;
    cv::Mat& inMat = *(cv::Mat*)image_editable;
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

    if (start == true){
        // if at least one marker detected
        if (ids.size() > 0) {
            gridCorners = findCorners(corners, ids);
        }

        drawGrid(finalMat, gridCorners, hSize, vSize);

        // refine findTokens
        findTokens(partMat, finalMat);

    }

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
Java_com_example_raven_pathfindar_MainActivity_setTokenList(JNIEnv *env, jobject instance, jlong x, jlong y) {



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

extern "C"
void
Java_com_example_raven_pathfindar_MainActivity_init(JNIEnv *env, jobject instance) {
    init();
}