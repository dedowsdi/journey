#ifndef MOVIEDIALOG_H
#define MOVIEDIALOG_H
#include <QDialog>
#include <QMap>

typedef QMap<QString, double> RateMap;
typedef QMap<QString, RateMap> UserMap;

class QGridLayout;
class QTextEdit;
class MovieDialog : public QDialog
{
  Q_OBJECT

protected:
  UserMap mRatings;
  std::vector<QString> mMovies;
  QGridLayout* mGridLayout;
  QTextEdit* mRecommendEdit;

public:
  MovieDialog(QWidget *parent = 0);

signals:

private slots:
  void recommend();

};


#endif /* MOVIEDIALOG_H */
