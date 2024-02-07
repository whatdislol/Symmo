#ifndef PLAYLISTADDER_H
#define PLAYLISTADDER_H

#include <QDialog>

namespace Ui {
    class PlaylistAdder;
}

class PlaylistAdder : public QDialog
{
    Q_OBJECT

public:
    explicit PlaylistAdder(QWidget* parent = nullptr);
    ~PlaylistAdder();

private:
    Ui::PlaylistAdder* ui;
};

#endif // PLAYLISTADDER_H