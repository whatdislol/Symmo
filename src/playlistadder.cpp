#include "playlistadder.h"
#include "ui_playlistadder.h"

PlaylistAdder::PlaylistAdder(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::PlaylistAdder)
{
    ui->setupUi(this);
}

PlaylistAdder::~PlaylistAdder()
{
    delete ui;
}