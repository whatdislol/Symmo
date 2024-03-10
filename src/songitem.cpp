// SongItem.cpp
#include "SongItem.h"

SongItem::SongItem(int index, const QString& songName, const QString& duration, QWidget* parent) : QWidget(parent) {
    indexLabel = new QLabel(QString::number(index));
    nameLabel = new QLabel(songName);
    durationLabel = new QLabel(duration);
    durationLabel->setFixedWidth(60); // Set a fixed width for the duration label

    QHBoxLayout* layout = new QHBoxLayout;
    layout->addWidget(indexLabel);
    layout->addSpacing(10); // Set spacing between index and name labels
    layout->addWidget(nameLabel);
    layout->addStretch(); // Add stretch to make the name label take up all remaining space
    layout->addWidget(durationLabel);
    setLayout(layout);
}