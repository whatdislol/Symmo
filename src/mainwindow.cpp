#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QIcon>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    int iconSize = 25;

    QIcon playIcon = style()->standardIcon(QStyle::SP_MediaPlay);
    QIcon skipIcon = style()->standardIcon(QStyle::SP_MediaSeekForward);
    QIcon backIcon = style()->standardIcon(QStyle::SP_MediaSeekBackward);
    QIcon volumnIcon = style()->standardIcon((QStyle::SP_MediaVolume));

    // Get the actual size of the icon without scaling
    QSize actualSize = playIcon.actualSize(QSize(iconSize, iconSize));

    // set size for button
    ui->pushButton_Play->setIcon(playIcon);
    ui->pushButton_Play->setIconSize(actualSize);
    ui->pushButton_Skip->setIcon(skipIcon);
    ui->pushButton_Skip->setIconSize(actualSize);
    ui->pushButton_Back->setIcon(backIcon);
    ui->pushButton_Back->setIconSize(actualSize);
    ui->pushButton_Volumn->setIcon(volumnIcon);
    ui->pushButton_Volumn->setIconSize(actualSize);

    M_Player = new QMediaPlayer();


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_Volumn_clicked()
{
    if (is_muted == false) {
        ui->pushButton_Volumn->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
        is_muted = true;
    }
    else {
        ui->pushButton_Volumn->setIcon(style()->standardIcon(QStyle::SP_MediaVolumeMuted));
        is_muted = false;
    }
}


void MainWindow::on_pushButton_Play_clicked()
{
    if (!is_paused) {
        ui->pushButton_Play->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        M_Player->pause();
        is_paused = true;
    }
    else {
        ui->pushButton_Play->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        M_Player->play();
        is_paused = false;
    }
}


void MainWindow::on_actionAdd_File_triggered()
{
    QString file_name = QFileDialog::getOpenFileName(this, "Open a file", "", "Audio File (*.mp3)");
    M_Player->setSource(QUrl::fromLocalFile(file_name));
    QFileInfo File(file_name);

    ui->label_filename->setText(File.fileName());
}


void MainWindow::on_horizontalSlider_song_duration_valueChanged(int value)
{

}