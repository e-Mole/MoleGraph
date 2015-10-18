#ifndef CHANNELLAYOUT_H
#define CHANNELLAYOUT_H

#include <QGridLayout>
#include <QMap>

class QWidget;
class CentralLayout : public QGridLayout
{
    Q_OBJECT

    QMap<unsigned, QWidget*> m_widgets; //channel index as the key
    unsigned m_verticalMax;
public:
    CentralLayout(QWidget *parent, unsigned verticalMax);

    void addWidget(QWidget *widget, unsigned channelIndex);
signals:

public slots:

};

#endif // CHANNELLAYOUT_H
