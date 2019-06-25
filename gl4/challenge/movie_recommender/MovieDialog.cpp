#include "MovieDialog.h"
#include <QJsonDocument>
#include <QFile>
#include <QJsonObject>
#include <QDebug>
#include <QJsonArray>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QTextEdit>
#include <QLineEdit>
#include <QGridLayout>
#include <QPushButton>
#include <QDoubleValidator>
#include <algorithm>

class StrictDoubleValidator : public QDoubleValidator
{
public:
  StrictDoubleValidator(double bottom, double top, int decimals, QObject *parent = 0)
  :QDoubleValidator( bottom, top, decimals, parent) {};

  QValidator::State validate(QString &input, int &pos) const
  {
    const QValidator::State origState = QDoubleValidator::validate( input, pos );
    if( ( origState == QValidator::Intermediate ) 
        && ( input.toDouble() > top()  || input.toDouble() < bottom()) )
    {
      return QValidator::Invalid;
    }
    else
    {
      return origState;
    }
  }
};

//--------------------------------------------------------------------
MovieDialog::MovieDialog(QWidget *parent/* = 0*/)
{
  // load users
  QFile file("ratings.json");

  if(!file.open(QFile::ReadOnly))
  {
    qWarning("could't open ratings.json");
    return;
  }

  QByteArray data = file.readAll();
  QJsonDocument doc = QJsonDocument::fromJson(data);
  QJsonObject rootObj = doc.object();

  // read movie list
  QJsonArray movies = rootObj["movies"].toArray();
  for(auto iter = movies.begin(); iter != movies.end(); ++iter)
    mMovies.push_back(iter->toString());

  // build rating map
  QJsonObject users = rootObj["ratings"].toObject();
  auto keys = users.keys();
  for(auto iter = keys.begin(); iter != keys.end(); ++iter)
  {
    //qDebug() << *iter;
    auto& scores = mRatings.insert(*iter, RateMap()).value();
    QJsonObject movieRatings = users[*iter].toObject();
    QStringList movieNames = movieRatings.keys();
    for(auto it = movieNames.begin(); it != movieNames.end(); ++it)
    {
      scores.insert(*it, movieRatings[*it].toDouble());
    }
  }

  //qDebug() << doc.object()["info"].toString();
  QVBoxLayout* layout = new QVBoxLayout;
  this->setLayout(layout);
  // build ui
  mGridLayout = new QGridLayout;
  layout->addLayout(mGridLayout);

  for (int i = 0; i < mMovies.size(); ++i)
  {
    QLabel* lbl = new QLabel(mMovies[i]);
    QLineEdit* score = new QLineEdit;
    lbl->setBuddy(score);
    StrictDoubleValidator* validator = new StrictDoubleValidator(0.0, 10.0, 2);
    validator->setNotation(QDoubleValidator::StandardNotation);
    score->setValidator(validator);

    mGridLayout->addWidget(lbl, i, 0);
    mGridLayout->addWidget(score, i, 1);
  }

  QPushButton* submit = new QPushButton("recommend");
  mGridLayout->addWidget(submit, mMovies.size(), 0, 1, 2);

  mRecommendEdit = new QTextEdit;
  layout->addWidget(mRecommendEdit);
  mRecommendEdit->hide();
  mRecommendEdit->setReadOnly(true);

  connect(submit, SIGNAL(clicked()), this, SLOT(recommend()));

}

//--------------------------------------------------------------------
void MovieDialog::recommend()
{
  bool needRecommend = false;
  RateMap rateMap0;
  for (int i = 0; i < mGridLayout->rowCount() - 1; ++i)
  {
    QWidgetItem* item0 = static_cast<QWidgetItem*>(mGridLayout->itemAtPosition(i, 0));
    QLabel* lbl = static_cast<QLabel*>(item0->widget());
    QWidgetItem* item1 = static_cast<QWidgetItem*>(mGridLayout->itemAtPosition(i, 1));
    QLineEdit* score = static_cast<QLineEdit*>(item1->widget());
    if(score->text() == "")
    {
      needRecommend = true;
      continue;
    }
    rateMap0.insert(lbl->text(), score->text().toDouble());
    //qDebug() << lbl->text() << " : " << score->text();
  }

  if(!needRecommend)
    return;

  mRecommendEdit->show();

  /*
   * find k nearest neighbour, if there are too many datas, you might need to
   * build some precalculated table to avoid realtime calculate.
   */
  QVector<QPair<QString, double>> similarUsers;
  for(auto iter = mRatings.begin(); iter != mRatings.end(); ++iter)
  {
    const QString& userName = iter.key();
    const RateMap& rateMap1 = iter.value();

    float difference = 0;
    for(auto it = rateMap0.begin(); it != rateMap0.end(); ++it)
    {
      const QString& movie = it.key();
      double score0 = it.value();

      auto it1 = rateMap1.find(movie);
      if(it1 == rateMap1.end())
        difference += 100;
      else
      {
        float d = it1.value() - score0;
        difference += d*d;
      }
    }
    // add 1, so 0 map to 1, infinity map to 0
    difference = 1/(1+ sqrt(difference));
    similarUsers.push_back(qMakePair(userName, difference));
  }


  auto sortBySimilar = [](const QPair<QString, double>& lhs, const QPair<QString, double>& rhs)->bool
  {
    return lhs.second > rhs.second;
  };
  std::partial_sort(similarUsers.begin(), similarUsers.begin() + 3,  similarUsers.end(), sortBySimilar);

  QString s;
  for(auto iter = mMovies.begin(); iter != mMovies.end(); ++iter)
  {
    const QString& movie = *iter;
    auto it = rateMap0.find(movie);
    if(it != rateMap0.end())
      continue;

    double score = 0;
    double weightSum = 0;

    for (int i = 0; i < 3; ++i) 
    {
      const QString& neightbour = similarUsers[i].first;
      double similarity = similarUsers[i].second;
      weightSum += similarity;
      score += similarity * mRatings[neightbour][movie];
    }
    score /= weightSum;

    s += QString("%1 : %2\n").arg(movie).arg(score);
  }

  mRecommendEdit->setText(s);

  // get k nearest neighbour for current input rating
}
