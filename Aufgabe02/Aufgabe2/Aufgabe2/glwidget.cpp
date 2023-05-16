#include "glwidget.h"
#include <QtGui>
#include <GL/GLU.h>
#include "glut.h"
#include "mainwindow.h"
#include <iostream>
#include "point.h"
#include "BoundingBox.h"

GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent)
{
    doIntersection       = false;
    doSelfIntersection   = false;
    epsilon_draw         = (float)0.05;
    epsilon_intersection = (float)0.000005;

    // Hier Punkte hinzufügen (schönere Startpositionen!)
    points.addPoint(-1.00, -0.5);
    points.addPoint(-0.75, -0.5);
    points.addPoint(-0.50, -0.5);
    points.addPoint(-0.25, -0.5);
    points.addPoint( 0.00, -0.5);

    points.addPoint( 0.25,  0.5);
    points.addPoint( 0.50,  0.5);
    points.addPoint( 0.75,  0.5);
    points.addPoint( 1.00,  0.5);
}

GLWidget::~GLWidget()
{
}

std::vector<Point> pointSort(std::vector<Point>& points) {
    std::vector<Point> a = points;

    for (int i = 0; i < a.size(); i++) {
        for (int j = 0; j < (a.size() - i - 1); j++) {
            if (a[j].x > a[j + 1].x) {               
                Point aj = a[j];
                Point aj1= a[j + 1];
                a[j + 1] = aj;
                a[j]     = aj1;               
            }
            if (a[j].x == a[j + 1].x) {
                double tjx = a[j].x;
                double tj1 = a[j + 1].x;
                if (a[j].y > a[j + 1].y) {
                    Point aj = a[j];
                    Point aj1 = a[j + 1];
                    a[j + 1] = aj;
                    a[j] = aj1;
                }
            }
        }
    }
    return a;
}


// Berechnung des Bezier Punkts für gegebens t 
Point deCasteljau(const std::vector<Point>& points, double t) {
    std::vector<Point> kontrollpunkte = points;
    Point Q;
    std::vector<Point> neue_kontrollpunkte;
    while (kontrollpunkte.size() > 1) {
        for (int i = 0; i < kontrollpunkte.size() - 1; i++) {
            Q = kontrollpunkte[i] * (1 - t) + kontrollpunkte[i + 1] * t;
            neue_kontrollpunkte.push_back(Q);
        }
        kontrollpunkte = neue_kontrollpunkte;
        neue_kontrollpunkte.clear();
    }
    return kontrollpunkte[0];
}

std::vector<Point> drawBezierCurve(std::vector<Point> kontrollPunkte, double ti) {
    std::vector<Point> bezierPoints;
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_LINE_STRIP);
    // Bestimmen der Punkte Bezier Punkte durch den deCasteljau Algorithmus
    for (double t = 0; t <= 1; t += ti) {
        Point p1 = deCasteljau(kontrollPunkte, t); // Bestimmen der Bezier Punkte der ersten Bezier Kurve
        glVertex2f(p1.x, p1.y);
        bezierPoints.push_back(p1);
    }
    glEnd();
    return bezierPoints;
}

BoundingBox getBoundingBox(std::vector<Point> pList) {
    
    float xMin = pList[0].x;
    float yMin = pList[0].y;

    float xMax = pList[0].x;
    float yMax = pList[0].y;

    for (int i = 0; i < pList.size(); i++) {
        if (xMin > pList[i].x) {
            xMin = pList[i].x;
        }
        if (yMin > pList[i].y) {
            yMin = pList[i].y;
        }
        if (xMax < pList[i].x) {
            xMax = pList[i].x;
        }
        if (yMax < pList[i].y) {
            yMax = pList[i].y;
        }
    }
    //glColor3f(0, 1.0, 0);
    //glBegin(GL_LINE_STRIP);
    //glVertex2f(xMin, yMin);
    //glVertex2f(xMax, yMin);
    //glVertex2f(xMax, yMax);
    ///glVertex2f(xMin, yMax);
    ////glVertex2f(xMin, yMin);
    ////
    //glEnd();

    BoundingBox box = BoundingBox();
    box.xMin = xMin;
    box.yMin = yMin;
    box.xMax = xMax;
    box.yMax = yMax;
    return box;
}
bool doBoundingBoxesIntersect(BoundingBox b, BoundingBox c) {
    // Check x - Koordinates
    if (c.xMax <= b.xMin || c.xMin >= b.xMax) {
        return false;
    }
    // Check y - Koordinates
    if (c.yMax <= b.yMin || c.yMin >= b.yMax) {
        return false;
    }
    return true;

}
double maxDelta2(const std::vector<Point>& points) {
    double maxDelta = 0.0;
    for (int i = 0; i < points.size() - 2; i++) {
        double delta = std::max(std::abs(points[i + 2].x - 2 * points[i + 1].x + points[i].x),
            std::abs(points[i + 2].y - 2 * points[i + 1].y + points[i].y));
        maxDelta = std::max(maxDelta, delta);
    }
    return maxDelta;
}

Point calculateIntersectionV2(const Point& p1, const Point& p2, const Point& p3, const Point& p4) {
    // 1. Bestimmung der geraden gleichungen aus den gegebenen Punken
    // Form der Gleichung y = m*x+b;

    // 1.1 Berechnung m1 und m2
    double m1 = (p2.y - p1.y) / (p2.x - p1.x);
    double m2 = (p4.y - p3.y) / (p4.x - p3.x);

    // 1.2 Berechnung von b1 und b2
    double b1 = p1.y - m1 * p1.x;
    double b2 = p3.y - m2 * p3.x;

    // 2.1 Berechnung des Schnittpunktes der beiden geraden
    double x = (b2 - b1) / (m1 - m2);

    // 2.2 Berechnung des y Wert des schnitpunkts
    double y = m1 * x + b1;

    // Überprüfe ob der Berechnete wer auf der geraden liegt
    double maxYL1 = std::max(p1.y, p2.y);
    double maxYL2 = std::max(p3.y, p4.y);

    double minYL1 = std::min(p1.y, p2.y);
    double minYL2 = std::min(p3.y, p4.y);

    double maxXL1 = std::max(p1.x, p2.x);
    double maxXL2 = std::max(p3.x, p4.x);
              
    double minXL1 = std::min(p1.x, p2.x);
    double minXL2 = std::min(p3.x, p4.x);

    if ((y < minYL1) || (y < minYL2) || (y > maxYL1) || (y > maxYL2)) {
        return { std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN() };
    }
    if ((x < minXL1) || (x < minXL2) || (x > maxXL1) || (x > maxXL2)) {
        return { std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN() };
    }

    return Point(x, y);

   
}

Point calculateIntersection(const Point& b0, const Point& b1, const Point& c0, const Point& c1) {
    double t1 = ((c0.y - b0.y) * (c1.x - c0.x) - (c0.x - b0.x) * (c1.y - c0.y)) / ((c1.x - c0.x) * (b1.y - b0.y) - (c1.y - c0.y) * (b1.x - b0.x));
    double t2 = ((b0.y - c0.y) * (b1.x - b0.x) - (b0.x - c0.x) * (b1.y - b0.y)) / ((c1.x - c0.x) * (b1.y - b0.y) - (c1.y - c0.y) * (b1.x - b0.x));

    if (t1 >= 0 && t1 <= 1 && t2 >= 0 && t2 <= 1) {
        double Px = b0.x + t1 * (b1.x - b0.x);
        double Py = b0.y + t1 * (b1.y - b0.y);
        return { Px, Py };
    }

    // Return a point with NaN coordinates to indicate no intersection
    return { std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN() };
}
std::vector<Point> findIntersections(const std::vector<Point>& points1, const std::vector<Point>& points2) {
    std::vector<Point> intersections;

    for (size_t i = 0; i < points1.size() - 1; ++i) {
        for (size_t j = 0; j < points2.size() - 1; ++j) {
            Point intersection = calculateIntersectionV2(points1[i], points1[i + 1], points2[j], points2[j + 1]);
            if (!std::isnan(intersection.x) && !std::isnan(intersection.y)) {
                intersections.push_back(intersection);
            }
        }
    }

    return intersections;
}

void intersectBezier(std::vector<Point> b, std::vector<Point> c, double eps) {
    // Get both bounding Boxes
    BoundingBox bB = getBoundingBox(b);
    BoundingBox bC = getBoundingBox(c);
    int m = b.size() -1;
    int n = c.size() -1;

    if (doBoundingBoxesIntersect(bB, bC)) {
        if (m * (m - 1) * maxDelta2(b) > eps) {
            std::vector<Point> a1;
            std::vector<Point> a2;
            double test = 1 / (2 * (double)m);
            test = 0.5;
            for (double t = 0; t <= 1; t += test) {
                Point p1 = deCasteljau(b, t); // Bestimmen der Bezier Punkte der ersten Bezier Kurve
                if (t <= 0.5) {
                    a1.push_back(p1);
                }
                if (t >= 0.5) {
                    a2.push_back(p1);
                }
            }
            intersectBezier(a1, c, eps);
            intersectBezier(a2, c, eps);
        }
        else if (n * (n - 1) * maxDelta2(c) > eps) {
            std::vector<Point> d1;
            std::vector<Point> d2;
            double test = 1 / (2 * (double)n);
            test = 0.5;
            for (double t = 0; t <= 1; t += test) {
                Point p1 = deCasteljau(c, t); // Bestimmen der Bezier Punkte der ersten Bezier Kurve
                if (t <= 0.5) {
                    d1.push_back(p1);
                }
                if (t >= 0.5) {
                    d2.push_back(p1);
                }
            }
            intersectBezier(b, d1, eps);
            intersectBezier(b, d2, eps);
        }
        else {
            glLineWidth(0.5);
            glColor3f(1.0, 0, 0);
            glBegin(GL_LINE_STRIP);
            for (int i = 0; i < b.size(); i++) {
                glVertex2f(b[i].x, b[i].y);
            }
            glEnd();
            glColor3f(0, 1.0, 0);
            glBegin(GL_LINE_STRIP);
            for (int i = 0; i < c.size(); i++) {
                glVertex2f(c[i].x, c[i].y);
            }
            glEnd();
            
            // Get longer array
            std::vector<Point> intersections = findIntersections(b,c);

            if (!intersections.empty()) {
                glPointSize(7.0);
                glBegin(GL_POINTS);
                std::cout << "Intersection points:\n";
                for (const Point& intersection : intersections) {
                   glVertex2f(intersection.x, intersection.y);
                }
                glEnd();
            }
            else {
                std::cout << "No intersections found.\n";
            }
        }
    }

}

void selfIntersectionBezier(std::vector<Point> b, double eps) {
    // Unterteile die Kurve in Schnittfreie linien segmente
    int nrS = b.size()-1;
    std::vector <std::vector<Point >> segments;
    int c = 0;
    double intervall = (1 / (double(nrS)));
    std::vector<Point> bezierPoints;
    for (double t = 0; t <= 1; t += 0.05) {
        Point p1 = deCasteljau(b, t); // Bestimmen der Bezier Punkte der ersten Bezier Kurve
        if (t <= intervall)
        {
            bezierPoints.push_back(p1);
        }
        if (t > intervall) {
            segments.push_back(bezierPoints);
            bezierPoints.clear();
            bezierPoints.push_back(p1);
            intervall += (1 / (double(nrS)));
        }
    }   
    segments.push_back(bezierPoints);
    for (int i = 0; i < segments.size(); i++) {
        for (int j = 0; j < segments.size(); j++) {
            if (i == j)
                continue;

            intersectBezier(segments[i], segments[j], eps);

        }
    }
    std::cout << segments.size() << std::endl;
}
void GLWidget::paintGL()
{
    // clear
    glClear(GL_COLOR_BUFFER_BIT);

    // Koordinatensystem
    glColor3f(0.5,0.5,0.5);
    glBegin(GL_LINES);
    glVertex2f(-1.0, 0.0);
    glVertex2f( 1.0, 0.0);
    glVertex2f( 0.0,-1.0);
    glVertex2f( 0.0, 1.0);
    glEnd();
    glColor3f(1.0,0.0,0.0);

    // Punkte
    glPointSize(7.0);
    glBegin(GL_POINTS);
    for (int i=0; i<points.getCount(); i++) {
        glVertex2f(points.getPointX(i),points.getPointY(i));
    }
	
    glEnd();

    // Hüllpolygone zeichnen
    glColor3f(0.0,0.0,1.0);
    glBegin(GL_LINE_STRIP);
    for (int i=0; i<5; i++) {
        glVertex2f(points.getPointX(i),points.getPointY(i));
    }
    glEnd();
    glBegin(GL_LINE_STRIP);
    for (int i=5; i<points.getCount(); i++) {
        glVertex2f(points.getPointX(i),points.getPointY(i));
    }
    glEnd();

    
    // AUFGABE: Hier Kurve zeichnen
    // dabei epsilon_draw benutzen   
    
    // Umwandeln der Punkte in points zu Vector
    // 1. Teil Punkte
    std::vector<Point> points1;
    for (int i = 0; i < 5; i++) {
        double xValue = points.getPointX(i);
        double yValue = points.getPointY(i);
        points1.push_back(Point(xValue, yValue));
    }
    std::vector<Point> bezierPoints1 = drawBezierCurve(points1, epsilon_draw); // Zeichnen der 1. Bezierkurve mit t = 0.01
    // 2. Teil der Punkte
    std::vector<Point> points2;
    for (int i = 5; i < points.getCount(); i++) {
        double xValue = points.getPointX(i);
        double yValue = points.getPointY(i);
        points2.push_back(Point(xValue, yValue));
    }
    std::vector<Point> bezierPoints2 = drawBezierCurve(points2, epsilon_draw);
    	
    
    

    /*
    std::vector<Point> a1;
    std::vector<Point> a2;

    for (double t = 0; t <= 1; t += 0.1) {
        Point p1 = deCasteljau(points1, t); 
        if (t <= 0.5) {
            a1.push_back(p1);
        }
        if (t >= 0.5) {
            a2.push_back(p1);
        }
    }
    glColor3f(1.0, 0, 0);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < a1.size(); i++) {
        glVertex2f(a1[i].x, a1[i].y);
    }
    glEnd();
    glColor3f(0, 1.0, 0);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < a2.size(); i++) {
        glVertex2f(a2[i].x, a2[i].y);
    }
    glEnd();

    std::vector<Point> b1;
    std::vector<Point> b2;

    for (double t = 0; t <= 1; t += 0.1) {
        Point p1 = deCasteljau(points2, t); 
        if (t <= 0.5) {
            b1.push_back(p1);
        }
        if (t >= 0.5) {
            b2.push_back(p1);
        }
    }
    glColor3f(1.0, 0, 0);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < b1.size(); i++) {
        glVertex2f(b1[i].x,b1[i].y);
    }
    glEnd();
    glColor3f(0, 1.0, 0);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < b2.size(); i++) {
        glVertex2f(b2[i].x, b2[i].y);
    }
    glEnd();
    */

    // Schnittpunkte zeichnen
    if (doIntersection) {
        glColor3f(0.0,1.0,0.0);
        // AUFGABE: Hier Schnitte zeichnen
        // dabei epsilon_intersection benutzen
        intersectBezier(points1, points2, epsilon_intersection);
    }
    if (doSelfIntersection) {
        glColor3f(1.0,0.0,1.0);
        // AUFGABE: Hier Selbstschnitte zeichnen
        // dabei epsilon_intersection benutzen
        selfIntersectionBezier(points1, epsilon_intersection);
        selfIntersectionBezier(points2, epsilon_intersection);
    }
}


void GLWidget::initializeGL()
{
    resizeGL(width(),height());
}

void GLWidget::resizeGL(int width, int height)
{
    aspectx=1.0;
    aspecty=1.0;
    if (width>height) {
        aspectx=float(width)/height;
    }
    else {
        aspecty=float(height)/width;
    }
    glViewport(0,0,width,height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-aspectx,aspectx,-aspecty,aspecty);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

QPointF GLWidget::transformPosition(QPoint p)
{
    return QPointF((2.0*p.x()/width() - 1.0)*aspectx,-(2.0*p.y()/height() - 1.0)*aspecty);
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        QPoint pos = event->pos();
        QPointF posF = transformPosition(pos);
        points.setPointX(clickedPoint,posF.x());
        points.setPointY(clickedPoint,posF.y());
        update();
    }
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        QPoint pos = event->pos();
        QPointF posF = transformPosition(pos);
        clickedPoint = points.getClosestPoint(posF.x(),posF.y());
        points.setPointX(clickedPoint,posF.x());
        points.setPointY(clickedPoint,posF.y());
        update();
    }
}

void GLWidget::mouseDoubleClickEvent(QMouseEvent *)
{
}

void GLWidget::setIntersection(int state)
{
    doIntersection = (state == Qt::Checked);
    update();
}

void GLWidget::setSelfIntersection(int state)
{
    doSelfIntersection = (state == Qt::Checked);
    update();
}

void GLWidget::setEpsilonDraw(double value)
{
    epsilon_draw = value;
    paintGL();
    update();

}

void GLWidget::setEpsilonIntersection(double value)
{
    epsilon_intersection = value;
    paintGL();
    update();
}
