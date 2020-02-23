#ifndef MYGRAPHICSSCENE_H
#define MYGRAPHICSSCENE_H

#include <QGraphicsItem>
#include <QGraphicsScene>

class Scene : public QGraphicsScene {
    friend class MainWindow;

public:
    explicit Scene(QObject* parent = nullptr);
    ~Scene() override;
    void RenderPdf();
    QRectF itemsBoundingRect();

    static bool drawPdf();
    static QList<QGraphicsItem*> selectedItems();
    static void addItem(QGraphicsItem* item);
    static QList<QGraphicsItem*> items(Qt::SortOrder order = Qt::DescendingOrder);
    static void update();

    void setCross1(const QPointF& cross);
    void setCross2(const QPointF& cross2);
    void setDrawRuller(bool drawRuller);

private:
    bool m_drawPdf = false;
    bool m_drawRuller = false;
    QPointF m_cross1;
    QPointF m_cross2;
    double m_scale = std::numeric_limits<double>::max();
    QRectF m_rect;
    QMap<long, long> hGrid;
    QMap<long, long> vGrid;
    void drawRuller(QPainter* painter);
    static Scene* m_instance;
    // QGraphicsScene interface
protected:
    void drawBackground(QPainter* painter, const QRectF& rect) override;
    void drawForeground(QPainter* painter, const QRectF& rect) override;
};

#endif // MYGRAPHICSSCENE_H
