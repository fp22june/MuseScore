#ifndef __AWLRL_H__
#define __AWLRL_H__
#include <cmath>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPainter>
#include <QPixmap>
#include <QPaintEvent>
#include <QImage>
namespace Awl {
class RotatedLabel : public QWidget {
      Q_OBJECT
      private:
            const int DEFAULTR = 3;
            QLabel *w;
            int r;
      public:
            RotatedLabel(QWidget *parent) : QWidget(parent), r{DEFAULTR} {
                  QVBoxLayout *layout = new QVBoxLayout(this);
                  layout->setContentsMargins(0, 0, 0, 0);
                  w = new QLabel(this);
                  layout->addWidget(w);
                  setChildSize();
                  }
            void setText(const QString &text) { w->setText(text); }
            QString text() const { return w->text(); }
            void setStyleSheet(const QString &styleSheet) { w->setStyleSheet(styleSheet); }
            QString styleSheet() const { return w->styleSheet(); }
            void setAlignment(Qt::Alignment alignment) { w->setAlignment(alignment); }
            void setR(int x) {r = fmod(x, 4); setChildSize(); update(); }
      protected:
            void setChildSize() {
                  if (r == 1 || r == 3) {
                        w->setFixedWidth(height());
                        w->setFixedHeight(width());
                        };
                  }
            void resizeEvent(QResizeEvent* event) override {
                  QWidget::resizeEvent(event);
                  setChildSize();
                  }
            void paintEvent(QPaintEvent *event) override {
                  Q_UNUSED(event);
                  if (r==0) {
                        w->show();
                        return;
                        }
                  w->hide();
                  //QPixmap pixmap(w->size());
                  int wh = std::max(w->width(), w->height());
                  QPixmap pixmap(QSize(wh,wh));
                  w->render(&pixmap);
                  QPainter painter(this);
                  painter.setRenderHint(QPainter::SmoothPixmapTransform);
                  painter.setRenderHint(QPainter::Antialiasing);
                  painter.setRenderHint(QPainter::TextAntialiasing);
                  if (r==1) {
                        painter.translate(width(), 0);
                        }
                  else if(r==2){
                        painter.translate(width(),height());
                        }
                  else if(r==3){
                        painter.translate(0, height());
                        }
                  painter.rotate(r * 90.0);
                  painter.drawPixmap(0, 0, pixmap);
                  }
      };

}
#endif
