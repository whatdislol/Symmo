// mainwindow.h

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QStyle>
#include <QSlider>
#include <QMessageBox>
#include "audiocontrol.h"
#include "playlistmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

signals:
    void playlistAdded(QString playlistName);

private slots:
    void setMaxDuration(qint64 duration);
    void updateSongProgress(qint64 progress);
    void updateDuration(qint64 duration);
    void updatePlaybackUI(QMediaPlayer::MediaStatus status);
    void updateMuteIcon();
    void updatePlayPauseIcon();
    void on_actionAdd_File_triggered();
    void setupIcons();
    void addSongWidgetItem(QListWidgetItem* song);
    void addPlaylistWidgetItem(QListWidgetItem* playlist);
    void getNewPlaylistName();
    void updateSongsDisplay();
    void updatePlaylistInfo();
    void updatePlaylistDisplay();
    void removePlaylist(const int& index);
    void showContextMenu(const QPoint& pos);

private:
    Ui::MainWindow* ui;
    AudioControl* m_audioControl;
    PlaylistManager* m_playlistManager;
};

#endif // MAINWINDOW_H