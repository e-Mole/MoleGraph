#ifndef CHECKBOX_H
#define CHECKBOX_H

#include <QCheckBox>
namespace bases{
    class CheckBox : public QCheckBox
    {
        Q_OBJECT
    public:
        explicit CheckBox(QWidget *parent = nullptr);

    signals:

    public slots:
    };
}
#endif // CHECKBOX_H
