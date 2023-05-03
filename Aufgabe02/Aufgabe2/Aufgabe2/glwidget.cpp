
#include "glwidget.h"
#include <QtGui>
#include <GL/GLU.h>
#include "glut.h"
#include "mainwindow.h"
#include <iostream>
#include "point.h"

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

void drawBezierCurve(std::vector<Point> kontrollPunkte, double ti) {
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_LINE_STRIP);
    // Bestimmen der Punkte Bezier Punkte durch den deCasteljau Algorithmus
    for (double t = 0; t <= 1; t += ti) {
        Point p1 = deCasteljau(kontrollPunkte, t); // Bestimmen der Bezier Punkte der ersten Bezier Kurve
        glVertex2f(p1.x, p1.y);
    }
    glEnd();
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
    // TODO: Was macht die Methode epsilon_draw ????
    // dabei epsilon_draw benutzen   
    // Umwandeln der Punkte in points zu Vector
    // 1. Teil Punkte
    std::vector<Point> points1;
    for (int i = 0; i < 5; i++) {
        double xValue = points.getPointX(i);
        double yValue = points.getPointY(i);
        points1.push_back(Point(xValue, yValue));
    }
    drawBezierCurve(points1, epsilon_draw); // Zeichnen der 1. Bezierkurve mit t = 0.01
    // 2. Teil der Punkte
    std::vector<Point> points2;
    for (int i = 5; i < points.getCount(); i++) {
        double xValue = points.getPointX(i);
        double yValue = points.getPointY(i);
        points2.push_back(Point(xValue, yValue));
    }
    drawBezierCurve(points2, epsilon_draw);
    



	
    // Schnittpunkte zeichnen
    if (doIntersection) {
        glColor3f(0.0,1.0,0.0);
        // AUFGABE: Hier Schnitte zeichnen
        // dabei epsilon_intersection benutzen
    }
    if (doSelfIntersection) {
        glColor3f(1.0,0.0,1.0);
        // AUFGABE: Hier Selbstschnitte zeichnen
        // dabei epsilon_intersection benutzen
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
}

void GLWidget::setEpsilonIntersection(double value)
{
    epsilon_intersection = value;
}
