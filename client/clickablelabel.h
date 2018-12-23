#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>

// 封装一个可点击的QLabel
class ClickableLabel : public QLabel
{
    Q_OBJECT

public:
    explicit ClickableLabel(const QString& text, QWidget* parent);
    ~ClickableLabel();
signals:
    void clicked();
protected:
    void mousePressEvent(QMouseEvent* event);
private:

};

#endif // CLICKABLELABEL_H
