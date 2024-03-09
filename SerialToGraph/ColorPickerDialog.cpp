#include "ColorPickerDialog.h"
#include <QGridLayout>
#include <QPalette>
#include <bases/ClickableWidget.h>

ColorPickerDialog::ColorPickerDialog(QWidget *parent, QString const &title):
    bases::PlatformDialog(parent, title),
    m_color(Qt::black)
{
    QGridLayout *m_gridLayout = new QGridLayout();
    setLayout(m_gridLayout);

    for (unsigned i = 0; i < 10; i++)
    {
        for (unsigned j = 0; j < 10; j++)
        {
            bases::ClickableWidget *colorWidget = new bases::ClickableWidget(this);
            colorWidget->setMinimumSize(20,20);
            QColor color = (j == 0) ?
                QColor::fromHsl(0, 0, (int)((double)i * 255 / 9)) :
                QColor::fromHsl(i * 36, 255, (int)((double)j * 25.5));

            QPalette pal(palette());
            pal.setColor(QPalette::Window, color);
            colorWidget->setAutoFillBackground(true);
            colorWidget->setPalette(pal);


            m_gridLayout->addWidget(colorWidget, i, j);
            m_colors[colorWidget] = color;

            connect(colorWidget, SIGNAL(clicked()), this, SLOT(colorClicked()));
        }
    }
}

QColor ColorPickerDialog::GetSelectedColor()
{
    return m_color;
}

void ColorPickerDialog::colorClicked()
{
    m_color = m_colors[(bases::ClickableWidget*)sender()];
    accept();
}
