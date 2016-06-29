#ifndef LISTBOX_H
#define LISTBOX_H

#include <QComboBox>
namespace bases
{

class ComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit ComboBox(QWidget *parent = 0);

signals:

public slots:
};

} //namespace bases

#endif // LISTBOX_H
