#include "polarchart.h"
#include <QtGui/QMouseEvent>
#include <QtCore/QDebug>
#include <QtCharts/QAbstractAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLineSeries>
#include <QTimer>

QT_CHARTS_USE_NAMESPACE

const int MITTELWERT = 0;
const int ABWEICHUNG = 5;

long frameZaehler = 0;

PolarChart::PolarChart(QWidget *parent)
    : QChartView(parent)
{

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(1);


}
void PolarChart::update()
{
    static std::random_device rd;
    static std::mt19937 e2(rd());
    static std::normal_distribution<> dist(MITTELWERT, ABWEICHUNG);

    QChart *oldChart = chart();
//entferne bisherige linien
    QList<QAbstractSeries *> oldseriesList = oldChart->series();
    foreach (QAbstractSeries *series, oldseriesList)
    {
       oldChart->removeSeries(series);
    }
//addiere zufallszahlen zu Druckwert
    QList<QPointF> zlist;
    foreach (QPointF pt, oriWerte)
    {
       int zufallsZahl = std::round(dist(e2));
       //  qDebug() << zufallsZahl;
       zlist.append(QPointF( pt.rx(),pt.ry()+zufallsZahl));
    }
//f�ge neue punkte hinzu
    QLineSeries *druckWerte2 = new QLineSeries();
    druckWerte2->append(zlist);
    chart()->addSeries(druckWerte2);

 //   qDebug() << "repaint";
    repaint();
    qDebug() << "Frame: " << ++frameZaehler;
}
void PolarChart::initBasisWerte(QValueAxis *angularAxis, QValueAxis *radialAxis)
{
    QLineSeries *marker1 = new QLineSeries();

            marker1->append(0, 0);
            marker1->append(90, 100);

  //  druckWerte = new QLineSeries();
  //druckWerte->setName("Druck");
// erzeuge testpunkte
  for (float phi=0; phi<360; phi+=0.05)
  {
      float druck = phi;
      oriWerte.append( QPointF (phi, druck));
     // oriWerte.append( QPointF (phi, 2*r));

  }
  qDebug() << oriWerte.size() << " Punkte generiert!";


  /*
  oriWerte.append( QPointF (0,10));
  oriWerte.append( QPointF (45, 7));
  oriWerte.append( QPointF (90, 5));
  oriWerte.append( QPointF (135, 6));
  oriWerte.append( QPointF (180, 8));
  oriWerte.append( QPointF (210, 14));
  oriWerte.append( QPointF (240, 22));
  oriWerte.append( QPointF (270, 30));
  oriWerte.append( QPointF (300, 40));
  oriWerte.append( QPointF (320, 50));
  oriWerte.append( QPointF (350, 70));
  oriWerte.append( QPointF (360, 100));
  oriWerte.append( QPointF (0, 100));
  oriWerte.append( QPointF (5, 90));
  oriWerte.append( QPointF (10, 80));
  oriWerte.append( QPointF (20, 60));
  oriWerte.append( QPointF (50, 45));
  oriWerte.append( QPointF (60, 40));
  oriWerte.append( QPointF (90, 35));
  oriWerte.append( QPointF (110, 30));
  oriWerte.append( QPointF (130, 26));
  oriWerte.append( QPointF (150, 23));
  oriWerte.append( QPointF (180, 20));
  oriWerte.append( QPointF (210, 20));
  oriWerte.append( QPointF (240, 20));
  oriWerte.append( QPointF (270, 20));
  oriWerte.append( QPointF (290, 18));
  oriWerte.append( QPointF (310, 15));
  oriWerte.append( QPointF (340, 12));
  oriWerte.append( QPointF (360, 10));
  */
/*
  druckWerte->append(oriWerte);
  chart()->addSeries(druckWerte);
  druckWerte->attachAxis(radialAxis);
  druckWerte->attachAxis(angularAxis);
*/
}

void PolarChart::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Plus:
        chart()->zoomIn();
        break;
    case Qt::Key_Minus:
        chart()->zoomOut();
        break;
    case Qt::Key_Left:
        chart()->scroll(-1.0, 0);
        break;
    case Qt::Key_Right:
        chart()->scroll(1.0, 0);
        break;
    case Qt::Key_Up:
        chart()->scroll(0, 1.0);
        break;
    case Qt::Key_Down:
        chart()->scroll(0, -1.0);
        break;
    case Qt::Key_Space:
        switchChartType();
        break;



    default:
        QGraphicsView::keyPressEvent(event);
        break;
    }
}
void PolarChart::switchChartType()
{
    QChart *newChart;
    QChart *oldChart = chart();

    if (oldChart->chartType() == QChart::ChartTypeCartesian)
        newChart = new QPolarChart();
    else
        newChart = new QChart();

    // Move series and axes from old chart to new one
    QList<QAbstractSeries *> seriesList = oldChart->series();
    QList<QAbstractAxis *> axisList = oldChart->axes();
    QList<QPair<qreal, qreal> > axisRanges;

    foreach (QAbstractAxis *axis, axisList) {
        QValueAxis *valueAxis = static_cast<QValueAxis *>(axis);
        axisRanges.append(QPair<qreal, qreal>(valueAxis->min(), valueAxis->max()));
    }

    foreach (QAbstractSeries *series, seriesList)
        oldChart->removeSeries(series);

    foreach (QAbstractAxis *axis, axisList) {
        oldChart->removeAxis(axis);
        newChart->addAxis(axis, axis->alignment());
    }


    foreach (QAbstractSeries *series, seriesList) {
        newChart->addSeries(series);
        foreach (QAbstractAxis *axis, axisList)
            series->attachAxis(axis);
    }

    int count = 0;
    foreach (QAbstractAxis *axis, axisList) {
        axis->setRange(axisRanges[count].first, axisRanges[count].second);
        count++;
    }

    newChart->setTitle(oldChart->title());
    setChart(newChart);
    delete oldChart;
}
