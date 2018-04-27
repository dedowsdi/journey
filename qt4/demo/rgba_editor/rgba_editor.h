#ifndef RGBA_EDITOR_H
#define RGBA_EDITOR_H

#include <QDialog>

class QDialogButtonBox;
class QColorDialog;
class QListWidget;

class RgbaEditor : public QDialog
{
  Q_OBJECT

  QListWidget* mListWidget;
  QDialogButtonBox* mButtonBox;
  QList<quint32>* mColors;
  QColorDialog* mColorDialog;

public:
  RgbaEditor(QList<quint32>* colors, QWidget *parent = 0);

  virtual void done(int result);

signals:

private slots:
  void addColor();
  void selectColor();

private:
  void addColor(const QColor& color);
  void updateColor(QPushButton* btn, const QColor& color);

};

#endif /* RGBA_EDITOR_H */
