// SongItem.h
#ifndef SONGITEM_H
#define SONGITEM_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>

class SongItem : public QWidget {
    Q_OBJECT
public:
    explicit SongItem(int index, const QString& songName, const QString& duration, QWidget* parent = nullptr);

signals:

private:
    QLabel* indexLabel;
    QLabel* nameLabel;
    QLabel* durationLabel;

};

#endif // SONGITEM_H