#include "rgba_editor.h"
#include <QTableWidget>
#include <QListWidget>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QPushButton>
#include <iostream>
#include <QColorDialog>
#include <QColor>

//--------------------------------------------------------------------
RgbaEditor::RgbaEditor(QList<quint32>* colors, QWidget *parent/* = 0*/):
  QDialog(parent),
  mColors(colors)
{ 
  mColorDialog = new QColorDialog(this);
  mColorDialog->setOption(QColorDialog::ShowAlphaChannel);
  mColorDialog->setModal(true);
  mColorDialog->hide();

  mListWidget = new QListWidget(this);

  for (int i = 0; i < mColors->count(); ++i) {
    QColor color(mColors->at(i));
    color.setAlpha(qAlpha(mColors->at(i)));
    addColor(color);
  }

  mButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  QPushButton* addButton = mButtonBox->addButton("&Add Color", QDialogButtonBox::ActionRole);

  connect(mButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(mButtonBox, SIGNAL(rejected()), this, SLOT(reject()));
  connect(addButton, SIGNAL(clicked()), this, SLOT(addColor()));

  QVBoxLayout* layout = new QVBoxLayout();
  layout->addWidget(mListWidget);
  layout->addWidget(mButtonBox);

  setLayout(layout);

  setWindowTitle(tr("rgba editor"));
}

//--------------------------------------------------------------------
void RgbaEditor::done(int result)
{
  if (result == QDialog::Accepted) {
      mColors->clear();
      for (int i = 0; i < mListWidget->count(); ++i) {
        const QColor& color = mListWidget->itemWidget(mListWidget->item(i))->property("color").value<QColor>();
        mColors->push_back(color.rgba());
      }
  }
  QDialog::done(result);
}

//--------------------------------------------------------------------
void RgbaEditor::addColor()
{
  addColor(0xffffffff);
}

//--------------------------------------------------------------------
void RgbaEditor::selectColor()
{
  QPushButton* btn = qobject_cast<QPushButton*>(sender());
  if (!btn) return;

  const QColor& color0 = btn->property("color").value<QColor>();
  mColorDialog->setCurrentColor(color0);
  if (mColorDialog->exec() == QColorDialog::Accepted) {
    updateColor(btn, mColorDialog->selectedColor());
  }
}

//--------------------------------------------------------------------
void RgbaEditor::addColor(const QColor& color)
{
  QListWidgetItem* item = new QListWidgetItem();
  mListWidget->addItem(item);

  QPushButton* btn = new QPushButton;
  updateColor(btn, color);

  mListWidget->setItemWidget(item, btn);

  connect(btn, SIGNAL(clicked()), this, SLOT(selectColor()));
}

//--------------------------------------------------------------------
void RgbaEditor::updateColor(QPushButton* btn, const QColor& color)
{
  QString hexColor = QString("rgba(%1, %2, %3, %4)")
  .arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha());

  btn->setText(QString("0x%1").arg(color.rgba(),8, 16, QChar('0')));
  btn->setStyleSheet("font-weight: bold; background-color: " + hexColor);
  btn->setProperty("color", color);
}
